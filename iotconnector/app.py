from bottle import TEMPLATE_PATH, run

from iotconnector.settings import Settings

settings = Settings()

TEMPLATE_PATH.insert(0, settings.WEB['TEMPLATE_FOLDER'])

import iotconnector.routes

if __name__ == '__main__':
    run(host=settings.WEB['HOST'], port=settings.WEB['PORT'], debug=True)
