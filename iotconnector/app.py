from bottle import TEMPLATE_PATH, run, debug

from settings import Settings

debug(True)

settings = Settings()

TEMPLATE_PATH.insert(0, settings.WEB['TEMPLATE_FOLDER'])

import routes

if __name__ == '__main__':
    run(host=settings.WEB['HOST'], port=settings.WEB['PORT'], debug=True)
