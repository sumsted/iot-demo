from bottle import TEMPLATE_PATH, run, debug

from redis_helper import RedisHelper
from settings import Settings


settings = Settings()

debug(settings.WEB['DEBUG'])

TEMPLATE_PATH.insert(0, settings.WEB['TEMPLATE_FOLDER'])

import web_routes
import iot_gateway_routes

rh = RedisHelper()
# rh.clear()

if __name__ == '__main__':
    run(host=settings.WEB['HOST'], port=settings.WEB['PORT'], debug=settings.WEB['DEBUG'])
