#include "requests.h"
#include "devices.h"

#define DANGER_RPM 500
#define MODERATE_RPM 300
#define RED_LIGHT 25
#define GREEN_LIGHT 33
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
int maxRpm = 0;

Requests *r;

void setup() {
    Serial.begin(9600);
    pinMode(GREEN_LIGHT, OUTPUT);
    pinMode(RED_LIGHT, OUTPUT);
    digitalWrite(GREEN_LIGHT, 0);
    digitalWrite(RED_LIGHT, 0);

    lastRpmMillis = millis();
    lastPostMillis = millis();

    pinMode(ENCODER_PIN, INPUT);
    digitalWrite(ENCODER_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoderCb, RISING);

    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        blink(GREEN_LIGHT);
        blink(GREEN_LIGHT);
    } else {
        blink(RED_LIGHT);
        blink(RED_LIGHT);
        blink(RED_LIGHT);
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
        maxRpm = rpm > maxRpm ? rpm : maxRpm;
        if(rpm > DANGER_RPM){
            digitalWrite(GREEN_LIGHT, 1);
            digitalWrite(RED_LIGHT, 1);
        } else if( rpm > MODERATE_RPM) {
            digitalWrite(GREEN_LIGHT, 1);
            digitalWrite(RED_LIGHT, 0);
        } else {
            digitalWrite(GREEN_LIGHT, 0);
            digitalWrite(RED_LIGHT, 0);
        }
    }

    currentMillis = millis();
    if(currentMillis - lastPostMillis  > POST_WINDOW){
        r->postEvent(maxRpm);
        Serial.println("post");
        maxRpm = 0;
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
    strcpy(cu->configuration.gatewayProtocol,GATEWAY_PROTOCOL);
    strcpy(cu->configuration.gatewayHost,GATEWAY_HOST);
    cu->configuration.gatewayPort = GATEWAY_PORT;
    strcpy(cu->configuration.gatewayPath,GATEWAY_PATH);
    strcpy(cu->configuration.gatewayDeviceKey, HALL01_GATEWAY_KEY);
    strcpy(cu->configuration.state[1],"1");
    strcpy(cu->configuration.state[2],"2");
    strcpy(cu->configuration.state[3],"3");
    Serial.println("default config initialized");
//    r->printConfig("default", &cu);
    return cu;
}