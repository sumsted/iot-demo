# IoT Demo

This demonstration can be found running at https://iotis.fun. 

![Overview](/iotweb/static/images/overview.png)

## IoT Devices

All devices here are built using the ESP32 from Espressif. This microcontroller has wireless capabilities, WiFi, Bluetooth and BLE. The ESP32 can act as a WiFi access point or client. The ESP32 has a large number of GPIO pins, a hall sensor, temperature, button, led, capacitive touch pins and more. The code for the demo boards may be found in /esp32.

![ESP32](/iotweb/static/images/esp32b.png)

### Hall

The Hall board shows how a hall sensor may be used to measure revolutions per minute. Spin the spinner. The sketch counts the number of revolutions over a 5 second period. The lights light up as the revolutions increase. The maximum number of RPMs in a 30 second period are sent up to the IoT Hub.

![Hall](/iotweb/static/images/hall.png)

### Touch

The Touch board uses the built in capacitive touch capabilities of the ESP32 to measure changes in capacitance across a circuit. Use enough of these sensors and you can build your own touch pad or display. If you are able to light the red light, the board will send an event to the IoT Hub.

![Touch](/iotweb/static/images/touch.png)

### Dropbox

The drop board uses the onboard button on the ESP32. When the right button is pressed a "bad" event is sent to the IoT hub indicating that the dropbox needs service. The subscriber should send this event on to Salesforce so that a technician may be dispatched. Every 30 seconds the dropbox board will send a "good" even indicating that the the dropbox is healthy.

![Dropbox](/iotweb/static/images/dropbox.png)

### Robot Controller

The robot board sends commands to the robot. The board communicates directly to the robot WiFi access point and sends UDP packets to the robot controller indicating the action the robot should take.

![Robot Controller](/iotweb/static/images/robotcontroller.png)

### Web

The web device simulates an IoT device. It pushes events to IoT Hub. These events are picked up by the subscriber. The web device may be found at https://iotis.fun/device.

![Web](/iotweb/static/images/web.png)

## Constrained Device Gateway

A constrained device gateway is needed for those devices unable to directly connect to the IoT Hub, either because of network routing, security, or the devices is too simple to create the required request. The gateway is made up of three components. There's a web route that captures events from each device. This route captures device id, key, and a state. Once captured the web route places the event on a queue. In this example we use Redis to act as a queue for our events. The queue is needed to help the gateway throttle activity into the hub and to ensure that no events are lost should network access to the IoT Hub be lost. The IoT Hub client pops events off the queue and posts them to the IoT Hub. Thing I'd change: Run the gateway closer to the devices not remotely.

![Constrained Device Gateway](/iotweb/static/images/gateway.png)

## IoT Hub

The IoT Hub provides a complete device management solution. Devices may be created programatically or through the console. Once defined the devices are assigned keys and endpoints that allow the device to send events over a number of different protocols including HTTP and MQTT. MQTT is lighter weight protocol than HTTP and is focused on data transmission. An MQTT connection is bi-directional allowing a connection to publish and subscribe. This makes it useful for pushing updates down to devices. In HTTP devices must pole and pull for updates. Along with IoT Hub, Azure provides many ways to consume the device events. Service Bus Queues may be established to filter the relevant events needed by subscribers.  IoT Hub may capture events from a variety of sources supporting a variety of use cases allowing centralized management of all device assets. Separate queues may be established for each use case allowing filter of events specific for that use case.

![IoT Hub](/iotweb/static/images/iothub.png)

## Event Subscription

The subscription process uses a library provided by Azure to pull events from a service bus queue.  This python library is a wrapper to IoT Hub Client C Library and must be compiled for the platform that it will run on. The subscriber runs continually waiting for events to be posted on the Azure Service Bus Queue being watched. As events are picked up they are passed along to the appropriate subscribers. Thing i'd change:  Take advantage of the service bus queue to filter and route specific events to specific queues. I'd setup each consumer with it's own subscriber.

![Event Subscription](/iotweb/static/images/subscription.png)

