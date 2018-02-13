import requests
from iotconnector.settings import Settings
from iotconnector.jwt_helper import JwtHelper

settings = Settings()


class GiotHelper:
    def __init__(self, device_id):
        self.device_id = device_id
        self.jwt = JwtHelper(settings.get('GCP_PROJECT_ID'), settings.get('SIMON_PRIVATE_KEY'), 'RS256')

    def send_state(self, state):
        pass
        # todo: add state call
