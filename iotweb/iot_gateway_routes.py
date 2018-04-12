import datetime
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


@get('/iot_hub_log/peek')
def get_iot_hub_log_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_hub_log_key)
    return result


@get('/iot_subscriber_log/peek')
def get_iot_subscriber_log_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_subscriber_log_key)
    return result


@get('/iot_salesforce_log/peek')
def get_iot_salesforce_log_peek():
    result = {}
    rh = RedisHelper()
    result['data'] = rh.peek_queue(RedisHelper.iot_salesforce_log_key)
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


@post('/iot_gateway_queue/<device_key>')
def post_iot_gateway_queue(device_key):
    result = {'success': False, 'message': 'not authorized'}
    status_code = 400
    if device_key in settings.IOT_GW_DEVICE_KEYS:
        message = request.json
        message['timestamp'] = '{:%Y-%m-%d %H:%M:%S.%f}'.format(datetime.datetime.now())
        queue_object = {
            'device_id': settings.IOT_GW_DEVICE_KEYS[device_key],
            'message': message
        }
        rh = RedisHelper()
        rh.push_queue(RedisHelper.iot_gateway_queue_key, queue_object)
        rh.push_log(RedisHelper.iot_gateway_log_key, message)
        result = {"success": True, "status": "iot gateway queue posted", "message": message}
        status_code = 200
    else:
        logit("unauthorized")
        status_code = 403
    return bottle.HTTPResponse(status=status_code, body=json.dumps(result))
