import os
import sys

from bottle import get, request, response, run

sys.path.insert(0, os.path.abspath('..'))

import motor

robot = motor.Motor()


ROBOT_STOP = 0
ROBOT_FORWARD = 1
ROBOT_ROTATE_LEFT = 2
ROBOT_ROTATE_RIGHT = 3
ROBOT_BACKWARD = 4


@get('/robot/<direction>')
def get_robot(direction):
    if direction == ROBOT_STOP:
        robot.stop()
    elif direction == ROBOT_FORWARD:
        robot.forward()
    elif direction == ROBOT_ROTATE_LEFT:
        robot.rotate_left()
    elif direction == ROBOT_ROTATE_RIGHT:
        robot.rotate_right()
    elif direction == ROBOT_BACKWARD:
        robot.backward()


run(host='0.0.0.0', port=8080, debug=True)
