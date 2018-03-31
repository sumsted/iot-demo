import json
import os
import sys

sys.path.insert(0, os.path.abspath('..'))

try:
    import serial
except ModuleNotFoundError:
    import mock_serial as serial


class Motor:
    PACKET = "%1s%04d%04d!"
    TEST_SPEED = 50

    def __init__(self):
        self.address = self.get_usb_device('ch341', device_id='motor')
        self.device = serial.Serial(self.address, 9600, timeout=.5)
        self.read_packet()
        self.read_packet()
        self.read_packet()

    @staticmethod
    def get_usb_device(device_name, default='/dev/ttyUSB0', device_id=None):
        try:
            results = os.popen('dmesg |grep -i "ttyUSB"| grep -i "now attached"').read().split('\n')
            for line in reversed(results):
                print('line: %s' % line)
                if device_name in line:
                    address = '/dev/' + line.split(' ')[-1]
                    if device_id is not None:
                        device = serial.Serial(address, 9600, timeout=.5)
                        device.write('I!'.encode())
                        result = device.readline().decode()
                        device_info = json.loads(result)
                        if device_info['id'] == device_id:
                            return address
                    else:
                        return address
        except Exception as e:
            print('exception: ' + str(e))
        return default

    def write_packet(self, packet):
        print('writing to device: %s' % packet)
        num_bytes = self.device.write(packet.encode())
        print('bytes written: %d' % num_bytes)
        return num_bytes

    def read_packet(self):
        return self.device.readline()

    def move(self, left, right):
        if 100 < left < -100 or 100 < right < -100:
            return None
        else:
            packet = Motor.PACKET % ('F', left, right)
            self.write_packet(packet)
            result = self.read_packet()
            print("result: %s" % result)
            return json.loads(result.decode())

    def stop(self):
        return self.move(0, 0)

    def forward(self):
        return self.move(Motor.TEST_SPEED, Motor.TEST_SPEED)

    def backward(self):
        return self.move(-Motor.TEST_SPEED, -Motor.TEST_SPEED)

    def left(self):
        return self.move(0, Motor.TEST_SPEED)

    def right(self):
        return self.move(Motor.TEST_SPEED, 0)

    def rotate_left(self):
        return self.move(-Motor.TEST_SPEED, Motor.TEST_SPEED)

    def rotate_right(self):
        return self.move(Motor.TEST_SPEED, -Motor.TEST_SPEED)


if __name__ == '__main__':
    m = Motor()

    for l in range(-100, 101, 10):
        for r in range(-100, 101, 10):
            print(m.move(l, r))
