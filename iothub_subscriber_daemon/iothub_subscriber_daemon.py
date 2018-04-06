import json
from logit import logit
from redis_helper import RedisHelper
from settings import Settings
from sf_helper import SfHelper
from azure.servicebus import ServiceBusService


settings = Settings()


class SubscriberDaemon:

    def __init__(self):
        logit("INIT")
        self.bus_service = ServiceBusService(
            service_namespace=settings.IOT_SUBSCRIBER['SERVICE_BUS_NAME'],
            shared_access_key_name=settings.IOT_SUBSCRIBER['SHARED_ACCESS_POLICY_NAME'],
            shared_access_key_value=settings.IOT_SUBSCRIBER['SHARED_ACCESS_POLICY_KEY_VALUE']
        )
        self.queue_name = settings.IOT_SUBSCRIBER['QUEUE_NAME']
        logit("SUBSCRIBER INIT: bus service and queue identified")
        self.rh = RedisHelper()

    def start(self):
        logit("START")
        try:
            while True:
                logit("LOOKING: 4 message")
                message = self.bus_service.receive_queue_message(self.queue_name, peek_lock=False)
                if message.body is not None:
                    try:
                        message_object = json.loads(message.body.decode())
                    except Exception as e:
                        message_object = json.loads(message.body)
                    logit("MESSAGE: %s" % str(message_object))
                    self.rh.push_log(RedisHelper.iot_subscriber_log_key, message_object)
                    self.route(message_object)
                else:
                    logit("MESSAGE: is empty")

        except KeyboardInterrupt:
            logit("STOP: IoTHubClient daemon stopped")
        except Exception as e:
            logit("ERROR: exception %s" % str(e))

    def route(self, message_object):
        device_id = message_object['device']
        # {"device":"DBOX01","value":0,"timestamp":"2018-04-05 07:39:26.032771"}
        try:
            if device_id == 'DBOX01':
                sh = SfHelper()
                sh.test_access_token()
                state = 'bad' if message_object['value'] == 1 else 'good'
                url = '/services/data/v41.0/sobjects/Dropbox_Event__e'
                event_object = {
                    "dropbox_id__c": device_id,
                    "state__c": state
                }
                response = sh.post_json(url, event_object)
                self.rh.push_log(RedisHelper.iot_salesforce_log_key, message_object)
        except Exception as e:
            logit("ERROR: exception %s" % str(e))


if __name__ == '__main__':
    sd = SubscriberDaemon()
    sd.start()
