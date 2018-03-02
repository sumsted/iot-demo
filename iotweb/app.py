from bottle import TEMPLATE_PATH, run, debug

from settings import Settings


settings = Settings()

debug(settings.WEB['DEBUG'])

TEMPLATE_PATH.insert(0, settings.WEB['TEMPLATE_FOLDER'])

import routes

if __name__ == '__main__':
    run(host=settings.WEB['HOST'], port=settings.WEB['PORT'], debug=settings.WEB['DEBUG'])
