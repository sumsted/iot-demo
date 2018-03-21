import json

from azure.servicebus import ServiceBusService, Message, Queue
from settings import Settings
from logit import logit
from sf_helper import SfHelper

settings = Settings()


# todo post messages to log
# todo route robot messages to robot queue
# todo call salesforce with other messages

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

    def start(self):
        logit("START")
        try:
            while True:
                logit("LOOKING: 4 message")
                message = self.bus_service.receive_queue_message(self.queue_name, peek_lock=False)
                if message.body is not None:
                    logit("MESSAGE: %s" % str(message.body))
                    self.route(message)
                else:
                    logit("MESSAGE: is empty")

        except KeyboardInterrupt:
            logit("STOP: IoTHubClient daemon stopped")
        except Exception as e:
            logit("ERROR: exception %s" % str(e))

    def route(self, message):
        device_id = message.custom_properties['iothub-connection-device-id']
        event_object = json.loads(message.body)
        if device_id == 'WEB01':
            sh = SfHelper()
            sh.test_access_token()
            # url = "/services/data/v41.0/query/?q=SELECT+Dropbox_ID__c+FROM+Asset"
            # response = sh.get_data(url, True)

            url = '/services/data/v41.0/sobjects/Dropbox_Event__e'
            event_object = {
                "dropbox_id__c": "SN002",
                "state__c": 'bad'
            }
            response = sh.post_json(url, event_object)


if __name__ == '__main__':
    sd = SubscriberDaemon()
    sd.start()
