import json
import redis
from settings import Settings

settings = Settings()


class RedisHelper:
    state_queue_key = 'iot-state-queue'
    telemetry_queue_key = 'iot-telemetry-queue'
    robot_command_queue_key = 'robot-queue'
    robot_location_key = 'robot-location'
    
    def __init__(self):
        self.r = redis.Redis(settings.REDIS['HOST'], int(settings.REDIS['PORT']))

    def clear(self):
        self.r.delete("simon-"+RedisHelper.state_queue_key)
        self.r.delete("touch-"+RedisHelper.state_queue_key)
        self.r.delete("hall-"+RedisHelper.state_queue_key)
        self.r.delete("web-"+RedisHelper.state_queue_key)
        self.r.delete(RedisHelper.telemetry_queue_key)
        self.r.delete(RedisHelper.robot_command_queue_key)
        self.update_key(RedisHelper.robot_location_key, {"location": ""})

    def push_queue(self, key, state, prefix=None):
        data = {'data': state}
        serial_json = json.dumps(data)
        full_key = key if prefix is None else prefix + "-" + key
        return self.r.lpush(full_key, serial_json)

    def pop_queue(self, key, prefix=None):
        try:
            full_key = key if prefix is None else prefix + "-" + key
            serial_json = self.r.rpop(full_key).decode()
            return json.loads(serial_json.decode("utf-8"))['data']
        except AttributeError:
            return None

    def peek_queue(self, key, count=None, prefix=None):
        if count is None:
            count = 4
        result = []
        try:
            full_key = key if prefix is None else prefix + "-" + key
            queue_list = self.r.lrange(full_key, 0, count)
            for serial_json in queue_list:
                result.append(json.loads(serial_json.decode("utf-8"))['data'])
        except AttributeError:
            return None
        return result

    def update_key(self, key, value):
        data = {'data': value}
        serial_json = json.dumps(data)
        return self.r.set(key, serial_json)

    def get_key(self, key):
        try:
            serial_json = self.r.get(key).decode()
            return json.loads(serial_json.decode("utf-8"))['data']
        except AttributeError:
            return None


if __name__ == '__main__':

    poster = RedisHelper()
    peeler = RedisHelper()

    # poster.clear()

    state = {"color": "red"}
    poster.push_queue(RedisHelper.state_queue_key, state, "simon")
    state = {"color": "blue"}
    poster.push_queue(RedisHelper.state_queue_key, state, "simon")
    state = {"color": "green"}
    poster.push_queue(RedisHelper.state_queue_key, state, "touch")
    state = {"color": "yellow"}
    poster.push_queue(RedisHelper.state_queue_key, state, "touch")
    state = {"color": "orange"}
    poster.push_queue(RedisHelper.state_queue_key, state, "hall")
    state = {"color": "black"}
    poster.push_queue(RedisHelper.state_queue_key, state, "hall")
    state = {"color": "ping"}
    poster.push_queue(RedisHelper.state_queue_key, state, "web")
    state = {"color": "purple"}
    poster.push_queue(RedisHelper.state_queue_key, state, "web")

    telemetry = {"height": "wow"}
    poster.push_queue(RedisHelper.telemetry_queue_key, telemetry)
    telemetry = {"deptch": "-3"}
    poster.push_queue(RedisHelper.telemetry_queue_key, telemetry)
    telemetry = {"slant": "four"}
    poster.push_queue(RedisHelper.telemetry_queue_key, telemetry)

    command = {"forward": "4"}
    poster.push_queue(RedisHelper.robot_command_queue_key, command)
    command = {"right": "8"}
    poster.push_queue(RedisHelper.robot_command_queue_key, command)
    command = {"left": "2"}
    poster.push_queue(RedisHelper.robot_command_queue_key, command)
    command = {"rotateleft": "45"}
    poster.push_queue(RedisHelper.robot_command_queue_key, command)
    command = {"back": "3"}
    poster.push_queue(RedisHelper.robot_command_queue_key, command)

    # print(peeler.pop_queue(RedisHelper.state_queue_key))
    # print(peeler.pop_queue(RedisHelper.state_queue_key))
    # print(peeler.pop_queue(RedisHelper.state_queue_key))

    # telemetry = {"direction": 23, "velocity": 14}
    # poster.push_queue(RedisHelper.telemetry_queue_key, telemetry)
    # telemetry = {"direction": 45, "velocity": 32}
    # poster.push_queue(RedisHelper.telemetry_queue_key, telemetry)
    # print(peeler.pop_queue(RedisHelper.telemetry_queue_key))
    # print(peeler.pop_queue(RedisHelper.telemetry_queue_key))
    #
    # robot_command = {"direction": 10, "action": "forward"}
    # poster.push_queue(RedisHelper.robot_command_queue_key, robot_command)
    # robot_command = {"direction": 45, "action": "reverse"}
    # poster.push_queue(RedisHelper.robot_command_queue_key, robot_command)
    # print(peeler.pop_queue(RedisHelper.robot_command_queue_key))
    # print(peeler.pop_queue(RedisHelper.robot_command_queue_key))
    #
    robot_location_key = {"location": "http://192.168.2.3/robot"}
    poster.update_key(RedisHelper.robot_location_key, robot_location_key)
    # print(peeler.get_key(RedisHelper.robot_location_key))

