from pprint import pprint

import requests

from settings import Settings
from logit import logit

logit('importing sf_helpers.py')

settings = Settings()

from logit import logit

import logging

try:
    import http.client as http_client
except ImportError:
    # Python 2
    import httplib as http_client
http_client.HTTPConnection.debuglevel = 1

# You must initialize logging, otherwise you'll not see debug output.
logging.basicConfig()
logging.getLogger().setLevel(logging.DEBUG)
requests_log = logging.getLogger("requests.packages.urllib3")
requests_log.setLevel(logging.DEBUG)
requests_log.propagate = True


class SfHelper:
    instance = None

    class __SfHelper:

        def __init__(self):
            self.sf_access = {}
            self.authorize()

        def authorize(self):
            logit('AUTHORIZING')
            credentials = {
                'grant_type': 'password',
                'client_id': settings.SALESFORCE['CONSUMER_KEY'],
                'client_secret': str(settings.SALESFORCE['CONSUMER_SECRET']),
                'username': settings.SALESFORCE['USERNAME'],
                'password': settings.SALESFORCE['PASSWORD'] + settings.SALESFORCE['SECURITY_TOKEN'],
                'request_uri': 'http://localhost'
            }

            pprint(credentials)

            headers = {
                'Content-Type': 'application/x-www-form-urlencoded'
            }

            response = requests.post(settings.SALESFORCE['LOGIN_URL'], data=credentials, headers=headers)
            if response.status_code == 200:
                logit('AUTHORIZE SUCCESS')
                self.sf_access = response.json()
            else:
                logit('AUTHORIZE FAIL, CODE: %d, RESPONSE: %s' % (response.status_code, response.content))
                self.sf_access = {}

        def test_access_token(self):
            headers = {
                'Authorization': 'Bearer ' + self.sf_access['access_token']
            }

            response = requests.get(settings.SALESFORCE['BASE_URL'] + '/services/data/v26.0/sobjects', headers=headers)
            if response.status_code == 200:
                logit('sf good', 'INFO')
            else:
                logit('sf bad', 'ERROR')

        def get_data(self, url, reauthorize=None):
            reauthorize = True if reauthorize is None else reauthorize
            headers = {'Authorization': 'Bearer ' + self.sf_access['access_token']}
            request_url = self.sf_access['instance_url'] + url
            response = requests.get(request_url, headers=headers)
            logit("URL: %s\nREAUTHORIZE: %s\nSTATUS_CODE: %d\nRESPONSE_CONTENT: %s\n" % (
                url, reauthorize, response.status_code, str(response.content)))
            if response.status_code == 200:
                return response.json()
            elif reauthorize is True:
                logit('** REAUTHORIZING **')
                self.authorize()
                return self.get_data(url, False)
            else:
                return {'success': False}

        def post_json(self, url, data, reauthorize=None):
            reauthorize = True if reauthorize is None else reauthorize
            headers = {'Authorization': 'Bearer ' + self.sf_access['access_token']}
            request_url = self.sf_access['instance_url'] + url
            response = requests.post(request_url, json=data, headers=headers)
            logit("URL: %s\nREAUTHORIZE: %s\nSTATUS_CODE: %d\nRESPONSE_CONTENT: %s\n" % (
                url, reauthorize, response.status_code, str(response.content)))
            if response.status_code == 200:
                return response.json()
            elif reauthorize is True:
                logit('** REAUTHORIZING **')
                self.authorize()
                return self.get_data(url, False)
            else:
                return {'success': False}

    def __init__(self):
        if SfHelper.instance is None:
            SfHelper.instance = self.__SfHelper()
        else:
            pass

    def __getattr__(self, item):
        try:
            print('use existing instance attribute: %s' % repr(self.instance))
            return getattr(self.instance, item)
        except Exception as e:
            print('cannot access existing attribute on inner object: %s' % str(e))

    def __setattr__(self, item, value):
        try:
            print('use existing instance attribute: %s' % repr(self.instance))
            return setattr(self.instance, item, value)
        except Exception as e:
            print('cannot access existing attribute on inner object: %s' % str(e))

    def get(self, item, default=None):
        if self.instance is not None:
            if item in self.instance.settings:
                return self.instance.settings[item]
            else:
                return default
        else:
            return default

    def set(self, key, value):
        if self.instance is not None:
            self.instance.settings[key] = value

    def get_dict(self):
        if self.instance is not None:
            return self.instance.settings


if __name__ == '__main__':
    sh = SfHelper()
    sh.test_access_token()
    # url = "/services/data/v41.0/query/?q=SELECT+Dropbox_ID__c+FROM+Asset"
    # response = sh.get_data(url, True)

    url = '/services/data/v41.0/sobjects/Dropbox_Event__e'
    event_object = {
        "dropbox_id__c": "SN002",
        "state__c": 'yes'
    }
    response = sh.post_json(url, event_object)

    pass
