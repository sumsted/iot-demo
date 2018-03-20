from azure.servicebus import ServiceBusService, Message, Queue
from settings import Settings
from logit import logit

settings = Settings()


class SubscriberDaemon:

    def __init__(self):
        self.bus_service = ServiceBusService(
            service_namespace=settings.IOT_SUBSCRIBER['SERVICE_BUS_NAME'],
            shared_access_key_name=settings.IOT_SUBSCRIBER['SHARED_ACCESS_POLICY_NAME'],
            shared_access_key_value=settings.IOT_SUBSCRIBER['SHARED_ACCESS_POLICY_KEY_VALUE']
        )
        self.queue_name = settings.IOT_SUBSCRIBER['QUEUE_NAME']

    def start(self):
        try:
            while True:
                message = self.bus_service.receive_queue_message(self.queue_name, peek_lock=False)
                if message is not None:
                    logit("MESSAGE: %s" % str(message))
                else:
                    logit("MESSAGE: is empty")
        except KeyboardInterrupt:
            logit("STOP: IoTHubClient daemon stopped")


if __name__ == '__main__':
    sd = SubscriberDaemon()
    sd.start()
