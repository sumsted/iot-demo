#!/usr/bin/env python

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for
# full license information.
import json
import sys
import time

from iothub_client import IoTHubClient, IoTHubClientError, IoTHubTransportProvider, IoTHubClientResult
from iothub_client import IoTHubClientRetryPolicy
from iothub_client import IoTHubMessage, IoTHubMessageDispositionResult, IoTHubError, DeviceMethodReturnValue

from settings import Settings
from redis_helper import RedisHelper
from logit import logit

settings = Settings()

QUEUE_CHECK_DELAY = 5

# HTTP options
# Because it can poll "after 9 seconds" polls will happen effectively
# at ~10 seconds.
# Note that for scalabilty, the default value of minimumPollingTime
# is 25 minutes. For more information, see:
# https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
TIMEOUT = 241000
MINIMUM_POLLING_TIME = 9

# messageTimeout - the maximum time in milliseconds until a message times out.
# The timeout period starts at IoTHubClient.send_event_async.
# By default, messages do not expire.
MESSAGE_TIMEOUT = 10000

RECEIVE_CONTEXT = 0
AVG_WIND_SPEED = 10.0
MIN_TEMPERATURE = 20.0
MIN_HUMIDITY = 60.0
MESSAGE_COUNT = 5
RECEIVED_COUNT = 0
CONNECTION_STATUS_CONTEXT = 0
TWIN_CONTEXT = 0
SEND_REPORTED_STATE_CONTEXT = 0
METHOD_CONTEXT = 0

# global counters
RECEIVE_CALLBACKS = 0
SEND_CALLBACKS = 0
BLOB_CALLBACKS = 0
CONNECTION_STATUS_CALLBACKS = 0
TWIN_CALLBACKS = 0
SEND_REPORTED_STATE_CALLBACKS = 0
METHOD_CALLBACKS = 0

# chose HTTP, AMQP, AMQP_WS or MQTT as transport protocol
PROTOCOL = IoTHubTransportProvider.MQTT


# some embedded platforms need certificate information
def set_certificates(client):
    from iothub_client_cert import CERTIFICATES
    try:
        client.set_option("TrustedCerts", CERTIFICATES)
        logit("set_option TrustedCerts successful")
    except IoTHubClientError as iothub_client_error:
        logit("set_option TrustedCerts failed (%s)" % iothub_client_error)


def receive_message_callback(message, counter):
    global RECEIVE_CALLBACKS
    message_buffer = message.get_bytearray()
    size = len(message_buffer)
    logit("Received Message [%d]:" % counter)
    logit("    Data: <<<%s>>> & Size=%d" % (message_buffer[:size].decode('utf-8'), size))
    map_properties = message.properties()
    key_value_pair = map_properties.get_internals()
    logit("    Properties: %s" % key_value_pair)
    counter += 1
    RECEIVE_CALLBACKS += 1
    logit("    Total calls received: %d" % RECEIVE_CALLBACKS)
    return IoTHubMessageDispositionResult.ACCEPTED


def send_confirmation_callback(message, result, user_context):
    global SEND_CALLBACKS
    logit("Confirmation[%d] received for message with result = %s" % (user_context, result))
    map_properties = message.properties()
    logit("    message_id: %s" % message.message_id)
    logit("    correlation_id: %s" % message.correlation_id)
    key_value_pair = map_properties.get_internals()
    logit("    Properties: %s" % key_value_pair)
    SEND_CALLBACKS += 1
    logit("    Total calls confirmed: %d" % SEND_CALLBACKS)


def connection_status_callback(result, reason, user_context):
    global CONNECTION_STATUS_CALLBACKS
    logit("Connection status changed[%d] with:" % user_context)
    logit("    reason: %d" % reason)
    logit("    result: %s" % result)
    CONNECTION_STATUS_CALLBACKS += 1
    logit("    Total calls confirmed: %d" % CONNECTION_STATUS_CALLBACKS)


def device_twin_callback(update_state, payload, user_context):
    global TWIN_CALLBACKS
    logit("")
    logit("Twin callback called with:")
    logit("updateStatus: %s" % update_state)
    logit("context: %s" % user_context)
    logit("payload: %s" % payload)
    TWIN_CALLBACKS += 1
    logit("Total calls confirmed: %d\n" % TWIN_CALLBACKS)


def send_reported_state_callback(status_code, user_context):
    global SEND_REPORTED_STATE_CALLBACKS
    logit("Confirmation[%d] for reported state received with:" % user_context)
    logit("    status_code: %d" % status_code)
    SEND_REPORTED_STATE_CALLBACKS += 1
    logit("    Total calls confirmed: %d" % SEND_REPORTED_STATE_CALLBACKS)


def device_method_callback(method_name, payload, user_context):
    global METHOD_CALLBACKS
    logit("\nMethod callback called with:\nmethodName = %s\npayload = %s\ncontext = %s" % (
        method_name, payload, user_context))
    METHOD_CALLBACKS += 1
    logit("Total calls confirmed: %d\n" % METHOD_CALLBACKS)
    device_method_return_value = DeviceMethodReturnValue()
    device_method_return_value.response = "{ \"Response\": \"This is the response from the device\" }"
    device_method_return_value.status = 200
    return device_method_return_value


def print_last_message_time(client):
    try:
        last_message = client.get_last_message_receive_time()
        logit("Last Message: %s" % time.asctime(time.localtime(last_message)))
        logit("Actual time : %s" % time.asctime())
    except IoTHubClientError as iothub_client_error:
        if iothub_client_error.args[0].result == IoTHubClientResult.INDEFINITE_TIME:
            logit("No message received")
        else:
            logit(iothub_client_error)


def iothub_client_init(connection_string, protocol):
    # prepare iothub client
    client = IoTHubClient(connection_string, protocol)
    if client.protocol == IoTHubTransportProvider.HTTP:
        client.set_option("timeout", TIMEOUT)
        client.set_option("MinimumPollingTime", MINIMUM_POLLING_TIME)
        # set the time until a message times out
        client.set_option("messageTimeout", MESSAGE_TIMEOUT)
    # some embedded platforms need certificate information
    set_certificates(client)
    # to enable MQTT logging set to 1
    if client.protocol == IoTHubTransportProvider.MQTT:
        client.set_option("logtrace", 0)
        client.set_message_callback(
            receive_message_callback, RECEIVE_CONTEXT)
    if client.protocol == IoTHubTransportProvider.MQTT or client.protocol == IoTHubTransportProvider.MQTT_WS:
        client.set_device_twin_callback(
            device_twin_callback, TWIN_CONTEXT)
        client.set_device_method_callback(
            device_method_callback, METHOD_CONTEXT)
    if client.protocol == IoTHubTransportProvider.AMQP or client.protocol == IoTHubTransportProvider.AMQP_WS:
        client.set_connection_status_callback(
            connection_status_callback, CONNECTION_STATUS_CONTEXT)

    retry_policy = IoTHubClientRetryPolicy.RETRY_INTERVAL
    retry_interval = 100
    client.set_retry_policy(retry_policy, retry_interval)
    logit("SetRetryPolicy to: retry_policy = %d" % retry_policy)
    logit("SetRetryPolicy to: retryTimeoutLimitInSeconds = %d" % retry_interval)
    retry_policy_return = client.get_retry_policy()
    logit("GetRetryPolicy returned: retry_policy = %d" % retry_policy_return.retryPolicy)
    logit("GetRetryPolicy returned: retryTimeoutLimitInSeconds = %d" % retry_policy_return.retryTimeoutLimitInSeconds)
    return client


def iothub_client_daemon_run():
    try:
        rh = RedisHelper()
        queue_name = settings.IOT_GATEWAY['iot_gateway_queue']
        while True:
            queue_object = rh.pop_queue(queue_name)

            try:
                connection_string = queue_object['connection_string']
                message_serial = json.dumps(queue_object['message'])
            except KeyError:
                logit("improperly formatted gateway queue object %s" % str(queue_object))
                time.sleep(QUEUE_CHECK_DELAY)
                continue
            except TypeError:
                logit("gateway queue is empty")
                time.sleep(QUEUE_CHECK_DELAY)
                continue

            client = iothub_client_init(connection_string, PROTOCOL)
            if client.protocol == IoTHubTransportProvider.MQTT:
                logit("IoTHubClient is reporting state")
                reported_state = "{\"newState\":\"standBy\"}"
                client.send_reported_state(reported_state,
                                           len(reported_state),
                                           send_reported_state_callback,
                                           SEND_REPORTED_STATE_CONTEXT)

            message = IoTHubMessage(message_serial)
            message_id = int(str(time.time()).replace('.', ''))
            message.message_id = "message_%d" % message_id

            client.send_event_async(message, send_confirmation_callback, message_id)
            logit(
                "IoTHubClient.send_event_async accepted message [%d] for transmission to IoT Hub." % message_id)
            status = client.get_send_status()
            logit("Send status: %s" % status)

            # Wait for Commands or exit
            logit("IoTHubClient waiting for commands, press Ctrl-C to exit")

            time.sleep(QUEUE_CHECK_DELAY)

    except IoTHubError as iothub_error:
        logit("Unexpected error %s from IoTHub" % iothub_error)
        return
    except KeyboardInterrupt:
        logit("IoTHubClient daemon stopped")


if __name__ == '__main__':

    if settings.IOT_GATEWAY['protocol'] == 'HTTP':
        PROTOCOL = IoTHubTransportProvider.HTTP
    elif settings.IOT_GATEWAY['protocol'] == 'MQTT':
        PROTOCOL = IoTHubTransportProvider.MQTT
    else:
        PROTOCOL = IoTHubTransportProvider.HTTP

    logit("\nPython %s" % sys.version)
    logit("IoT Hub client gateway for constrained devices.")
    logit("    Protocol %s" % PROTOCOL)
    logit("    Queue %s:%s" % (settings.REDIS['HOST'], settings.REDIS['PORT']))

    iothub_client_daemon_run()
