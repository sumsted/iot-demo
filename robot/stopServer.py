import os
import subprocess
import sys

from bottle import get, run

sys.path.insert(0, os.path.abspath('..'))


@get('/')
def get_index():
    return 'ss'


@get('/stop4321')
def get_stop():
    subprocess.call(['sudo', 'shutdown', 'now'])
    return 'ok'


run(host='0.0.0.0', port=8090, debug=True)
