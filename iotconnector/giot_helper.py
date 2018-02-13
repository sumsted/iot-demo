import base64
import json

import requests
from iotconnector.settings import Settings
from iotconnector.jwt_helper import JwtHelper

settings = Settings()


class GiotHelper:
    STATE_URL = "https://cloudiot-device.googleapis.com/v1beta1/projects/%(project_id)s/locations/%(cloud_region)s/registries/%(registry_id)s/devices/%(device_id)s:setState"
    TELEMETRY_URL = "https://cloudiot-device.googleapis.com/v1beta1/projects/%(project_id)s/locations/%(cloud-region)s/registries/%(registry_id)s/devices/%(device_id)s:publishEvent'"

    def __init__(self, device_id):
        self.device_id = device_id
        self.jwt = JwtHelper(settings.get('GCP_PROJECT_ID'), settings.get('SIMON_PRIVATE_KEY'), 'RS256')

    def post_state(self, state):
        url = self.STATE_URL % {
            'project_id': settings.get('GCP_PROJECT_ID'),
            'cloud_region': settings.get('GCP_CLOUD_REGION'),
            'registry_id': settings.get('GCP_REGISTRY_ID'),
            'device_id': self.device_id
        }
        headers = {
            'authorization': 'Bearer ' + self.jwt.get_valid_jwt(),
            'cache-control': 'no-cache'
        }
        b64_state = base64.b64encode(str(state).encode()).decode()
        payload = {"state": {"binary_data": b64_state}}
        r = requests.post(url, data=json.dumps(payload), headers=headers)

        return r

    def post_telemetry(self, telemetry):
        url = self.TELEMETRY_URL % {
            'project_id': settings.get('GCP_PROJECT_ID'),
            'cloud_region': settings.get('GCP_CLOUD_REGION'),
            'registry_id': settings.get('GCP_REGISTRY_ID'),
            'device_id': self.device_id
        }
        headers = {
            'authorization': 'Bearer ' + self.jwt.get_valid_jwt(),
            'cache-control': 'no-cache'
        }
        b64_telemtry = base64.b64encode(str(telemetry).encode()).decode()
        payload = {"state": {"binary_data": b64_telemtry}}
        r = requests.post(url, data=json.dumps(payload), headers=headers)

        return r


if __name__ == '__main__':
    g = GiotHelper('simon01')
    r = g.post_state('42')
    print(r)
