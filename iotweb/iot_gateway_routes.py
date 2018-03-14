import json

import bottle
from bottle import get, post, request

from logit import logit
from redis_helper import RedisHelper
from settings import Settings

settings = Settings()


@get('/iot_gateway_log/peek')
def get_iot_gateway_log_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_gateway_log_key)
    return result


@post('/iot_gateway_log/<key>')
def post_iot_gateway_log(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.iot_gateway_log_key, command)
        result = {"success": True, "message": "iot gateway log posted"}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@get('/iot_gateway_queue/peek')
def get_iot_gateway_queue_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_gateway_queue_key)
    return result


@get('/iot_gateway_queue/<key>')
def get_iot_gateway_queue(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        rh = RedisHelper()
        result = rh.pop_queue(RedisHelper.iot_gateway_queue_key)
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))


@post('/iot_gateway_queue/<key>')
def post_iot_gateway_queue(key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if key == settings.WEB['ACCESS']:
        command = request.json
        rh = RedisHelper()
        rh.push_queue(RedisHelper.iot_gateway_queue_key, command)
        rh.push_log(RedisHelper.iot_gateway_log_key, command)
        result = {"success": True, "message": "iot gateway queue posted"}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))
