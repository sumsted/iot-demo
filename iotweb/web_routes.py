from bottle import static_file, route, get, template

from logit import logit
from redis_helper import RedisHelper
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


@get('/device')
def get_index():
    return template('device.html')


@get('/watch')
def get_index():
    return template('watch.html')


@get('/about')
def get_index():
    return template('about.html')


@get('/service')
def get_bad():
    logit("service")
    post_dropbox_event('bad')
    return {"status": "success"}
