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


@get('/robot/<robot_direction>')
def get_robot(robot_direction):
    result = {'success': -1}
    direction = int(robot_direction)
    if direction == ROBOT_STOP:
        result = robot.stop()
    elif direction == ROBOT_FORWARD:
        result = robot.forward()
    elif direction == ROBOT_ROTATE_LEFT:
        result = robot.rotate_left()
    elif direction == ROBOT_ROTATE_RIGHT:
        result = robot.rotate_right()
    elif direction == ROBOT_BACKWARD:
        result = robot.backward()
    return result


run(host='0.0.0.0', port=8080, debug=True)
