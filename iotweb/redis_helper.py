import json
import redis
from settings import Settings

settings = Settings()


class RedisHelper:

    iot_gateway_queue_key = settings.IOT_GATEWAY['iot_gateway_queue']
    iot_gateway_log_key = settings.IOT_GATEWAY['iot_gateway_log']

    iot_subscriber_log_key = settings.IOT_SUBSCRIBER['iot_subscriber_log']
    robot_location_key = settings.IOT_SUBSCRIBER['robot_location']
    robot_queue_key = settings.IOT_SUBSCRIBER['robot_queue']

    def __init__(self):
        self.r = redis.Redis(settings.REDIS['HOST'], int(settings.REDIS['PORT']))

    def clear(self):
        self.r.delete(RedisHelper.iot_gateway_queue_key)
        self.r.delete(RedisHelper.iot_gateway_log_key)
        self.r.delete(RedisHelper.iot_subscriber_log_key)
        self.r.delete(RedisHelper.robot_queue_key)
        self.update_key(RedisHelper.robot_location_key, {"location": ""})

    def push_queue(self, key, payload, prefix=None):
        data = {'data': payload}
        serial_json = json.dumps(data)
        full_key = key if prefix is None else prefix + "-" + key
        return self.r.lpush(full_key, serial_json)

    def push_log(self, key, payload, prefix=None):
        data = {'data': payload}
        serial_json = json.dumps(data)
        full_key = key if prefix is None else prefix + "-" + key
        if self.r.llen(full_key) > 20:
            self.r.rpop(full_key)
        self.r.lpush(full_key, serial_json)

    def pop_queue(self, key, prefix=None):
        try:
            full_key = key if prefix is None else prefix + "-" + key
            serial_json = self.r.rpop(full_key).decode()
            # locally getting str but on DO getting bytes
            try:
                return json.loads(serial_json.decode("utf-8"))['data']
            except AttributeError:
                return json.loads(serial_json)['data']
        except Exception:
            return None

    def peek_queue(self, key, count=None, prefix=None):
        if count is None:
            count = 4
        result = []
        try:
            full_key = key if prefix is None else prefix + "-" + key
            queue_list = self.r.lrange(full_key, 0, count)
            for serial_json in queue_list:
                try:
                    result.append(json.loads(serial_json.decode("utf-8"))['data'])
                except AttributeError:
                    result.append(json.loads(serial_json)['data'])
        except Exception:
            return None
        return result

    def update_key(self, key, value):
        data = {'data': value}
        serial_json = json.dumps(data)
        return self.r.set(key, serial_json)

    def get_key(self, key):
        try:
            serial_json = self.r.get(key).decode()
            try:
                return json.loads(serial_json.decode("utf-8"))['data']
            except AttributeError:
                return json.loads(serial_json)['data']
        except Exception:
            return None


if __name__ == '__main__':

    poster = RedisHelper()
    peeler = RedisHelper()

    # poster.clear()

    state = {"color": "red"}
    # poster.push_queue(RedisHelper.iot_web_hook_log, state)
    # state = {"color": "scott"}
    # poster.push_queue(RedisHelper.iot_web_hook_log, state)
    # state = {"color": "green"}
    poster.push_queue(RedisHelper.robot_queue_key, state)
    state = {"color": "yellow"}
    poster.push_queue(RedisHelper.robot_queue_key, state)
    state = {"color": "orange"}
    poster.push_queue(RedisHelper.iot_gateway_queue_key, state)
    state = {"color": "black"}
    poster.push_queue(RedisHelper.iot_gateway_queue_key, state)
    state = {"color": "ping"}
    poster.push_queue(RedisHelper.iot_gateway_log_key, state)
    state = {"color": "purple"}
    poster.push_queue(RedisHelper.iot_gateway_log_key, state)

    # print(peeler.pop_queue(RedisHelper.iot_web_hook_log))
    # print(peeler.pop_queue(RedisHelper.iot_web_hook_log))
    #
    # print(peeler.pop_queue(RedisHelper.robot_queue_key))
    # print(peeler.pop_queue(RedisHelper.robot_queue_key))
    #
    # print(peeler.pop_queue(RedisHelper.iot_gateway_queue_key))
    # print(peeler.pop_queue(RedisHelper.iot_gateway_queue_key))
    #
    # print(peeler.pop_queue(RedisHelper.iot_gateway_log_key))
    # print(peeler.pop_queue(RedisHelper.iot_gateway_log_key))

    robot_location_key = {"location": "http://192.168.2.3/robot"}
    poster.update_key(RedisHelper.robot_location_key, robot_location_key)
    print(peeler.get_key(RedisHelper.robot_location_key))

