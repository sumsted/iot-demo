import json

import bottle
from bottle import post, request

from giot_helper import GiotHelper
from logit import logit
from redis_helper import RedisHelper
from settings import Settings

settings = Settings()


@post('/giot/state/<device_id>/<key>')
def post_state(device_id, key):
    state = request.json
    logit(str(state))
    state_str = json.dumps(state)
    result = {'success': False, 'message': 'state not posted'}
    status_code = 400
    if key == settings.IOT_GW_KEYS[device_id]:
        if state is not None:
            if settings.GCP['ACTIVE']:
                g = GiotHelper(device_id)
                r = g.post_state(state_str)
                if r.status_code == 200:
                    # result['data'] = json.loads(r.content.decode("utf-8"))
                    result['data'] = state
                    result['success'] = True
                    result['message'] = 'state posted'
                    status_code = 200
                else:
                    result['data'] = json.loads(r.content)
                    result['success'] = False
                    result['message'] = 'state not posted'
                    status_code = 400
            rh = RedisHelper()
            rh.push_queue(settings.QUEUES[device_id]+"-"+RedisHelper.state_queue_key, state)
        else:
            logit("state missing")
            status_code = 400
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@post('/giot/telemetry/<device_id>/<key>')
def post_telemetry(device_id, key):
    telemetry = request.json
    result = {'success': False, 'message': 'telemetry not posted'}
    status_code = 400
    if key == settings.IOT_GW_KEYS[device_id]:
        if telemetry is not None:
            if settings.GCP['ACTIVE']:
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
            rh = RedisHelper()
            rh.push_queue(settings.QUEUES[device_id]+"-"+RedisHelper.telemetry_queue_key, telemetry)
        else:
            logit("telemetry missing")
            status_code = 400
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))

