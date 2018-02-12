#include "requests.h"
#define RED_LIGHT 27
#define YELLOW_LIGHT 25
#define GREEN_LIGHT 33
#define EMA_A .125
#define LED_BLINK_DELAY 50

int emaS;
long current = 0;
long last = 0;
long total = 0;
long start = 0;
Requests *r;

void setup(){
    pinMode(RED_LIGHT, OUTPUT);
    pinMode(YELLOW_LIGHT, OUTPUT);
    pinMode(GREEN_LIGHT, OUTPUT);
    Serial.begin(9600);
    emaS = touchRead(T4);

    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        blink(GREEN_LIGHT);
        blink(GREEN_LIGHT);
    } else {
        blink(RED_LIGHT);
        blink(RED_LIGHT);
    }
}

// 5% of last 30
void loop(){
    int touchiness = touchRead(T4);
    int filtered = lowPassFilter(touchiness);
    delay(50);
    if(filtered<15){
        digitalWrite(RED_LIGHT, 1);
        digitalWrite(YELLOW_LIGHT, 0);
        digitalWrite(GREEN_LIGHT, 0);
        startContact();
    } else if(filtered > 45){
        digitalWrite(RED_LIGHT, 0);
        digitalWrite(YELLOW_LIGHT, 0);
        digitalWrite(GREEN_LIGHT, 1);
        endContact();
    } else {
        digitalWrite(RED_LIGHT, 0);
        digitalWrite(YELLOW_LIGHT, 1);
        digitalWrite(GREEN_LIGHT, 0);
        endContact();
    }

    current = millis();
    if((current - last) > 15000 ){
fi        last = current;
    }
    Serial.print(start);
    Serial.print(", ");
    Serial.println(total);
}

void startContact(){
    if(start == 0){
        start = millis();
    }
}

void endContact(){
    if(start > 0){
        long end = millis();
        total += end - start;
        start = 0;
    }
}

int checkContact(){
    int result = 0;
    endContact();
    result = total / 1000;
    total = 0;
    return result;
}

int lowPassFilter(int touchiness){
    emaS = (EMA_A*touchiness) + ((1-EMA_A)*emaS);
    return emaS;
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
    strcpy(cu->configuration.serial, "TC001");
    strcpy(cu->configuration.deviceId, "TOUCH01");
    strcpy(cu->configuration.model, "ESP32");
    strcpy(cu->configuration.firmware, "1.0");
    strcpy(cu->configuration.wifiSsid, "u1002");
    strcpy(cu->configuration.wifiPassword, "e4fac6c7");
    strcpy(cu->configuration.location, "Home");
    strcpy(cu->configuration.gatewayProtocol,"https");
    strcpy(cu->configuration.gatewayHost,"io.adafruit.com");
    cu->configuration.gatewayPort = 443;
    strcpy(cu->configuration.gatewayPath,"/users");
    strcpy(cu->configuration.state[1],"1");
    strcpy(cu->configuration.state[2],"2");
    strcpy(cu->configuration.state[3],"3");
    Serial.println("default config initialized");
//    r->printConfig("default", &cu);
    return cu;
}