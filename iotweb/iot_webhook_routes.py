import json

import bottle
from bottle import get, post, request

from settings import Settings
from logit import logit
from redis_helper import RedisHelper

settings = Settings()


@post('/iot_web_hook')
def post_iot_web_hook():
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    key = request.query.key
    payload = request.json
    azure_event_grid_header = request.get_header('Aeg-Event-Type')
    logit("IOT WEB HOOK: payload: %s, header%s" % (str(payload), azure_event_grid_header))
    if azure_event_grid_header == 'SubscriptionValidation':
        validation_code = payload[0]['data']['validationCode']
        validation_response = {'validationResponse': validation_code}
        logit("AZURE VALIDATION RESPONSE: %s" % str(validation_response))
    elif key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.iot_web_hook_log, command)
        result = {"success": True, "message": "iot web hook posted"}
        status_code = 200

        # todo do something with webhook call
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/iot_web_hook_log/peek')
def get_robot_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_web_hook_log)
    return result


@post('/iot_web_hook_log/<key>')
def post_iot_web_hook_log(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.iot_web_hook_log, command)
        result = {"success": True, "message": "iot web hook log posted"}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@post('/robot/location')
def post_robot_location():
    location = request.json
    rh = RedisHelper()
    rh.update_key(RedisHelper.robot_location_key, location)
    result = {"success": True, "message": "location posted"}
    return result


@get('/robot/location')
def get_robot_location():
    rh = RedisHelper()
    location = rh.get_key(RedisHelper.robot_location_key)
    result = {'data': location}
    return result


@get('/robot/peek')
def get_robot_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.robot_queue_key)
    return result


@post('/robot/<key>')
def post_robot(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.robot_queue_key, command)
        result = {"success": True, "message": "robot posted"}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/robot/<key>')
def get_robot(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        rh = RedisHelper()
        result = rh.pop_queue(RedisHelper.robot_queue_key)
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))
