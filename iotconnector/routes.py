from bottle import static_file, route, get, post, template

from sf_helper import SfHelper
from settings import Settings

settings = Settings()
from logit import logit


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
    return static_file(filepath, root=settings.DF17['STATIC_FOLDER'])


@get('/')
def get_index():
    return template('index.html')


@get('/service')
def get_bad():
    logit("service")
    post_dropbox_event('bad')
    return {"status": "success"}