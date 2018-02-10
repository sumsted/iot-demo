#include "requests.h"

#define EMA_A .65
#define INACTIVE 42
#define THRESHOLD -2
#define DELAY 0
#define TICK_LIGHT 25
#define LED_BLINK_DELAY 50

int emaS1;
int emaS2;
unsigned long ticks = 0;
int val = 0;
int hpv = 0;
int lastVal = 0;
bool inTick = false;

unsigned long lastTicks = 0;
unsigned long lastMillis = 0;
unsigned long checkMillis = 0;
int rpm = 0;

Requests *r;

void setup() {
    Serial.begin(9600);
    pinMode(TICK_LIGHT, OUTPUT);
    digitalWrite(TICK_LIGHT, 0);
    val = hallRead();
    lastVal = hallRead();
    emaS1 = lastVal;
    lastMillis = millis();
    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
    } else {
        blink(TICK_LIGHT);
        blink(TICK_LIGHT);
    }
}

void loop() {
    val = hallRead();
    lowPassFilter(&emaS1, val);
    hpv = val - emaS1;

    if(!inTick && hpv < THRESHOLD){
        ticks++;
        inTick = true;
        digitalWrite(TICK_LIGHT, 1);
    } else if(hpv > THRESHOLD){
        inTick = false;
        digitalWrite(TICK_LIGHT, 0);
    }

    checkMillis = millis();
    if(checkMillis - lastMillis  > 30000){
        rpm = (ticks - lastTicks)  * 60000 / (checkMillis - lastMillis);
        r->postAdaIo("scottumsted", "simon.rpm", rpm);
        lastTicks = ticks;
        lastMillis = checkMillis;
    }

    Serial.print(inTick+20);
    Serial.print(", ");
    Serial.print(rpm);
    Serial.print(", ");
    Serial.println(hpv);
    delay(0);
}

int lowPassFilter(int *pems, int val){
    *pems = (EMA_A*val) + ((1-EMA_A) * *pems);
    return *pems;
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