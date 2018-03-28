#include "requests.h"
#include "devices.h"

#define BLUE_BUTTON 0
#define BLUE_LED 1
#define LED_BLINK_DELAY 200
#define BUTTON_WAIT 250

long lastButtonPress = 0;
bool blueAction = false;

Requests *r;
unsigned long current = 0;
unsigned long last = 0;

void setup(){
    Serial.begin(9600);

    byte i;
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BLUE_BUTTON, INPUT);
    attachInterrupt(BLUE_BUTTON, blueButtonPush,CHANGE);

    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        blink(BLUE_LED);
        blink(BLUE_LED);
    } else {
        blink(BLUE_LED);
        blink(BLUE_LED);
        blink(BLUE_LED);
    }
}


void loop(){
    if(blueAction){
        dropBoxProblem();
        blueAction = false;
    }
    current = millis();
    if((current - last) > 30000 ){
        dropBoxNoProblem();
        last = current;
    }
}


void dropBoxProblem(){
    r->postEvent(1);
}


void dropBoxNoProblem(){
    r->postEvent(0);
}

void blink(byte pin){
    digitalWrite(pin, 1);
    delay(LED_BLINK_DELAY);
    digitalWrite(pin, 0);
    delay(LED_BLINK_DELAY/4);
}


void blueButtonPush(){
    long buttonPress = millis();
    if(buttonPress > lastButtonPress+BUTTON_WAIT){
        blueAction = true;
        lastButtonPress = buttonPress;
    }
}


ConfigurationUnion *defaultConfig(){
    ConfigurationUnion *cu = new ConfigurationUnion;
    memset(cu, '\0', sizeof(cu));
    strcpy(cu->configuration.serial, "SC001");
    strcpy(cu->configuration.deviceId, "DBOX01");
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
    strcpy(cu->configuration.gatewayDeviceKey, DBOX01_GATEWAY_KEY);
    strcpy(cu->configuration.state[1],"1");
    strcpy(cu->configuration.state[2],"2");
    strcpy(cu->configuration.state[3],"3");
    Serial.println("default config initialized");
//    r->printConfig("default", &cu);
    return cu;
}
