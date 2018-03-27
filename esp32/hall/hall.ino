#include "requests.h"
#include "devices.h"

#define TICK_LIGHT 25
#define LED_BLINK_DELAY 50
#define RPM_WINDOW 5000
#define POST_WINDOW 30000
#define ENCODER_PIN 12

unsigned long ticks = 0;

unsigned long lastTicks = 0;
unsigned long lastRpmMillis = 0;
unsigned long lastPostMillis = 0;
unsigned long currentMillis = 0;
int rpm = 0;

Requests *r;

void setup() {
    Serial.begin(9600);
    pinMode(TICK_LIGHT, OUTPUT);
    digitalWrite(TICK_LIGHT, 0);

    lastRpmMillis = millis();
    lastPostMillis = millis();

    pinMode(ENCODER_PIN, INPUT);
    digitalWrite(ENCODER_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoderCb, RISING);

    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
    } else {
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
    }
}

void loop() {
    currentMillis = millis();
    if(currentMillis - lastRpmMillis  > RPM_WINDOW){
        rpm = ((ticks - lastTicks)  * 60000) / (currentMillis - lastRpmMillis);
        Serial.print("ticks: ");
        Serial.print(ticks);
        Serial.print(" lastTicks: ");
        Serial.print(lastTicks);
        Serial.print(" currentMillis: ");
        Serial.print(currentMillis);
        Serial.print(" lastRpmMillis: ");
        Serial.print(lastRpmMillis);
        Serial.print(" rpm: ");
        Serial.println(rpm);
        lastTicks = ticks;
        lastRpmMillis = currentMillis;
    }

    currentMillis = millis();
    if(currentMillis - lastPostMillis  > POST_WINDOW){
        r->postEvent(rpm);
        Serial.println("post");
        lastPostMillis = currentMillis;
    }
}

void encoderCb(){
    ticks++;
}

void blink(byte pin){
    digitalWrite(pin, 1);
    delay(LED_BLINK_DELAY);
    digitalWrite(pin, 0);
    delay(LED_BLINK_DELAY/4);
}

ConfigurationUnion *defaultConfig(){
    ConfigurationUnion *cu = new ConfigurationUnion;
    memset(cu, '\0', sizeof(cu));
    strcpy(cu->configuration.serial, "HA001");
    strcpy(cu->configuration.deviceId, "HALL01");
    strcpy(cu->configuration.model, "ESP32");
    strcpy(cu->configuration.firmware, "1.0");
    strcpy(cu->configuration.wifiSsid, GATEWAY_WIFI_SSID);
    strcpy(cu->configuration.wifiPassword, GATEWAY_WIFI_PASSWORD);
    strcpy(cu->configuration.location, "Home");
//    strcpy(cu->configuration.gatewayProtocol,"https");
//    strcpy(cu->configuration.gatewayHost,"iotis.fun");
    strcpy(cu->configuration.gatewayProtocol,"http");
    strcpy(cu->configuration.gatewayHost,"192.168.2.32:8080");
    cu->configuration.gatewayPort = 443;
    strcpy(cu->configuration.gatewayPath,"iot_gateway_queue");
    strcpy(cu->configuration.gatewayDeviceKey, HALL01_GATEWAY_KEY);
    strcpy(cu->configuration.state[1],"1");
    strcpy(cu->configuration.state[2],"2");
    strcpy(cu->configuration.state[3],"3");
    Serial.println("default config initialized");
//    r->printConfig("default", &cu);
    return cu;
}