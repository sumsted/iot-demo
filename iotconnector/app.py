from bottle import TEMPLATE_PATH, run

# from sf_helper import SfHelper
from settings import Settings

settings = Settings()
# sf_helper = SfHelper()
TEMPLATE_PATH.insert(0, settings.SEACON2018['TEMPLATE_FOLDER'])

import routes

if __name__ == '__main__':
    run(host=settings.SEACON2018['HOST'], port=settings.SEACON2018['PORT'], debug=True)
