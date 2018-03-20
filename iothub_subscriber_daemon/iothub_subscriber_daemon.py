from azure.servicebus import ServiceBusService, Message, Queue
from settings import Settings
from logit import logit

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
                else:
                    logit("MESSAGE: is empty")
        except KeyboardInterrupt:
            logit("STOP: IoTHubClient daemon stopped")
        except Exception as e:
            logit("ERROR: exception %s" % str(e))


if __name__ == '__main__':
    sd = SubscriberDaemon()
    sd.start()
