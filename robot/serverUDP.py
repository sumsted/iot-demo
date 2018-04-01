import socket
import sys
import os

sys.path.insert(0, os.path.abspath('..'))
import motor

UDP_SERVER = "0.0.0.0"
UDP_PORT = 8080

robot = motor.Motor()

ROBOT_STOP = 0
ROBOT_FORWARD = 1
ROBOT_ROTATE_LEFT = 2
ROBOT_ROTATE_RIGHT = 3
ROBOT_BACKWARD = 4


def start_server():
    result = {'success': -1}
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((UDP_SERVER, UDP_PORT))
    while True:
        data, client_address = server.recvfrom(1024)
        direction = int(data)
        try:
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
        except Exception as e:
            print("Exception talking to motor controller: %s" % str(e))
        print(client_address, str(result))


if __name__ == "__main__":
    start_server()
