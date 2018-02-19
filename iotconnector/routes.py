import json

import bottle
from bottle import static_file, route, get, post, template, request

from giot_helper import GiotHelper
from logit import logit
from settings import Settings
from sf_helper import SfHelper

settings = Settings()


def post_dropbox_event(state):
    url = '/services/data/v41.0/sobjects/Dropbox_Event__e'
    event_object = {
        "dropbox_id__c": "SN002",
        "state__c": state
    }
    sf = SfHelper()
    return sf.post_json(url, event_object)


@route('/static/<filepath:path>')
def server_static(filepath):
    return static_file(filepath, root=settings.WEB['STATIC_FOLDER'])


@get('/')
def get_index():
    return template('index.html')


@post('/iotgw/state/<device_id>/<key>')
def post_state(device_id, key):
    state = request.json
    result = {'success': False, 'message': 'state not posted'}
    status_code = 400
    if key == settings.IOT_GW_KEYS[device_id]:
        if state is not None:
            g = GiotHelper(device_id)
            r = g.post_state(json.dumps(state))
            if r.status_code == 200:
                result['data'] = json.loads(r.content)
                result['success'] = True
                result['message'] = 'state posted'
                status_code = 200
            else:
                result['data'] = json.loads(r.content)
                result['success'] = False
                result['message'] = 'state not posted'
                status_code = 400
        else:
            logit("state missing")
            status_code = 400
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@post('/iotgw/telemetry/<device_id>/<key>')
def post_telemetry(device_id, key):
    telemetry = request.json
    result = {'success': False, 'message': 'telemetrynot posted'}
    status_code = 400
    if key == settings.IOT_GW_KEYS[device_id]:
        if telemetry is not None:
            g = GiotHelper(device_id)
            r = g.post_telemetry(json.dumps(telemetry))
            if r.status_code == 200:
                result['data'] = json.loads(r.content)
                result['success'] = True
                result['message'] = 'telemetry posted'
                status_code = 200
            else:
                result['data'] = json.loads(r.content)
                result['success'] = False
                result['message'] = 'telemetry not posted'
                status_code = 400
        else:
            logit("telemetry missing")
            status_code = 400
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/service')
def get_bad():
    logit("service")
    post_dropbox_event('bad')
    return {"status": "success"}
