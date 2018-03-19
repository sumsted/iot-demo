import json

import bottle
from bottle import get, post, request

from settings import Settings
from logit import logit
from redis_helper import RedisHelper

settings = Settings()


@post('/webhook')
def post_webhook():
    payload = request.json
    logit("WEBHOOK"+str(payload))


@post('/iot_web_hook/<key>')
def post_iot_web_hook(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
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
