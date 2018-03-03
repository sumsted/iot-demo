import json

import bottle
from bottle import get, post, request

from logit import logit
from redis_helper import RedisHelper
from settings import Settings

settings = Settings()


@post('/robot/location')
def post_robot_location():
    location = request.json
    rh = RedisHelper()
    rh.update_key(RedisHelper.robot_location_key, location)
    result = {"success": True, "message": "location posted"}
    return result


@post('/robot/command/<key>')
def post_robot_command(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.robot_command_queue_key, command)
        result = {"success": True, "message": "command posted"}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/state/peek')
def get_state_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.state_queue_key)
    return result


@get('/telemetry/peek')
def get_telemetry_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.telemetry_queue_key)
    return result


@get('/robotcommand/peek')
def get_robotcommand_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.robot_command_queue_key)
    return result


@get('/state/pop/<key>')
def get_state_pop(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        rh = RedisHelper()
        result = rh.pop_queue(RedisHelper.state_queue_key)
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/telemetry/pop/<key>')
def get_telemetry_pop(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        rh = RedisHelper()
        result = rh.pop_queue(RedisHelper.telemetry_queue_key)
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/robotcommand/pop/<key>')
def get_robotcommand_pop(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        rh = RedisHelper()
        result = rh.pop_queue(RedisHelper.robot_command_queue_key)
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))
