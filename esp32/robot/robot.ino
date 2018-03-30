#include "requests.h"

#define ROBOT_STOP 0
#define ROBOT_FORWARD 1
#define ROBOT_ROTATE_LEFT 2
#define ROBOT_ROTATE_RIGHT 3
#define ROBOT_BACKWARD 4

#define GREEN_BUTTON 34
#define RED_BUTTON 39
#define BLUE_BUTTON 38
#define YELLOW_BUTTON 37

#define WHITE_BUTTON 36

#define SUCCESS_LED 13
#define FAIL_LED 12

#define GREEN_LED 14
#define RED_LED 26
#define BLUE_LED 27
#define YELLOW_LED 25

#define NUM_COLORS 4
#define BUTTON_WAIT 250
#define LED_BLINK_DELAY 200

enum colors {
    green,
    red,
    blue,
    yellow
};

byte leds[NUM_COLORS] = {GREEN_LED,RED_LED,BLUE_LED, YELLOW_LED};
byte buttons[NUM_COLORS] = {GREEN_BUTTON, RED_BUTTON, BLUE_BUTTON, YELLOW_BUTTON};
long presses[NUM_COLORS] = {0, 0, 0, 0};
long lastWhiteButtonPress = 0;
long last = 0;
long current = 0;

bool greenAction = false;
bool redAction = false;
bool blueAction = false;
bool yellowAction = false;
bool whiteAction = false;

#define MAX_SEQUENCE_SIZE 10
byte sequence[MAX_SEQUENCE_SIZE];
byte playerIndex = 0;
byte gameSize = 0;

int highScore = 0;
int lastScore = 0;
int averageScore = 0;
int numGames = 0;
int totalScore = 0;

Requests *r;

void setup(){
    Serial.begin(9600);

    byte i;
    for(i=0;i<NUM_COLORS;i++){
        pinMode(leds[i], OUTPUT);
        pinMode(buttons[i], INPUT);
    }
    pinMode(WHITE_BUTTON, INPUT);
    pinMode(SUCCESS_LED, OUTPUT);
    pinMode(FAIL_LED, OUTPUT);
    attachInterrupt(GREEN_BUTTON, greenButtonPush,CHANGE);
    attachInterrupt(RED_BUTTON, redBlueButtonPush,CHANGE);
    attachInterrupt(YELLOW_BUTTON, yellowWhiteButtonPush,CHANGE);

    ConfigurationUnion *pcu = defaultConfig();
    r = new Requests(pcu);
    if(r->wifiConnected){
        ledFlash();
    } else {
        blink(FAIL_LED);
        blink(FAIL_LED);
        blink(FAIL_LED);
    }

}

long lastButtonPush = 0;

void loop(){

    if(greenAction==true){
        r->getEvent(ROBOT_FORWARD);
        greenAction = false;
    }
    if(redAction==true){
        r->getEvent(ROBOT_BACKWARD);
        redAction = false;
    }
    if(blueAction ==true){
        r->getEvent(ROBOT_LEFT);
        blueAction = false;
    }
    if(yellowAction==true){
        r->getEvent(ROBOT_RIGHT);
        yellowAction = false;
    }

    if(whiteAction==true){
        r->getEvent(ROBOT_STOP);
        blink(SUCCESS_LED);
        blink(FAIL_LED);
        whiteAction = false;
    }

}




/*
 * light up the leds
 */
void ledFlash(){
    byte i,j;
    for(i=0;i<1;i++){
        digitalWrite(SUCCESS_LED, 1);
        delay(100);
        digitalWrite(SUCCESS_LED, 0);
        delay(100);
        digitalWrite(FAIL_LED, 1);
        delay(100);
        digitalWrite(FAIL_LED, 0);
        delay(100);
        for(j=0;j<NUM_COLORS;j++){
            digitalWrite(leds[j], 1);
            delay(100);
            digitalWrite(leds[j], 0);
            delay(100);
        }
    }
}

void blink(byte pin){
    digitalWrite(pin, 1);
    delay(LED_BLINK_DELAY);
    digitalWrite(pin, 0);
    delay(LED_BLINK_DELAY/4);
}


/*
 * Capture button pushes. Some buttons share an interrupt so check for that as well.
 */
void greenButtonPush(){
    long buttonPress = millis();
    if(buttonPress > presses[green]+BUTTON_WAIT){
        greenAction = true;
        presses[green] = buttonPress;
    }
}

void redBlueButtonPush(){
    // shared interrupt
    int redButtonState = digitalRead(RED_BUTTON);
    int blueButtonState = digitalRead(BLUE_BUTTON);
    if(redButtonState == HIGH){
        redButtonPush();
    } else if(blueButtonState == HIGH){
        blueButtonPush();
    }
}

void redButtonPush(){
    long buttonPress = millis();
    if(buttonPress > presses[red]+BUTTON_WAIT){
        redAction = true;
        presses[red] = buttonPress;
    }
}

void blueButtonPush(){
    long buttonPress = millis();
    if(buttonPress > presses[blue]+BUTTON_WAIT){
        blueAction = true;
        presses[blue] = buttonPress;
    }
}

void yellowWhiteButtonPush(){
    // shared interrupt
    int yellowButtonState = digitalRead(YELLOW_BUTTON);
    int whiteButtonState = digitalRead(WHITE_BUTTON);
    if(yellowButtonState == HIGH){
        yellowButtonPush();
    } else if(whiteButtonState == HIGH) {
        whiteButtonPush();
    }
}

void yellowButtonPush(){
    long buttonPress = millis();
    if(buttonPress > presses[yellow]+BUTTON_WAIT){
        yellowAction = true;
        presses[yellow] = buttonPress;
    }
}

void whiteButtonPush(){
    long buttonPress = millis();
    if(buttonPress > lastWhiteButtonPress+BUTTON_WAIT){
        whiteAction = true;
        lastWhiteButtonPress = buttonPress;
    }
}

ConfigurationUnion *defaultConfig(){
    ConfigurationUnion *cu = new ConfigurationUnion;
    memset(cu, '\0', sizeof(cu));
    strcpy(cu->configuration.serial, "RB001");
    strcpy(cu->configuration.deviceId, "ROBOT01");
    strcpy(cu->configuration.model, "ESP32");
    strcpy(cu->configuration.firmware, "1.0");
    strcpy(cu->configuration.wifiSsid, GATEWAY_WIFI_SSID);
    strcpy(cu->configuration.wifiPassword, GATEWAY_WIFI_PASSWORD);
    strcpy(cu->configuration.location, "Home");
    strcpy(cu->configuration.gatewayProtocol,GATEWAY_PROTOCOL);
    strcpy(cu->configuration.gatewayHost,GATEWAY_HOST);
    cu->configuration.gatewayPort = GATEWAY_PORT;
    strcpy(cu->configuration.gatewayPath,GATEWAY_PATH);
    strcpy(cu->configuration.gatewayDeviceKey, SIMON01_GATEWAY_KEY);
    strcpy(cu->configuration.state[1],"1");
    strcpy(cu->configuration.state[2],"2");
    strcpy(cu->configuration.state[3],"3");
    Serial.println("default config initialized");
//    r->printConfig("default", &cu);
    return cu;
}