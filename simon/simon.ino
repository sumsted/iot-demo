#define RED_BUTTON 25
#define GREEN_BUTTON 27
#define BLUE_BUTTON 34
#define YELLOW_BUTTON 32
#define WHITE_BUTTON 39

#define RED_LED 26
#define GREEN_LED 14
#define BLUE_LED 35
#define YELLOW_LED 33

#define SUCCESS_LED 13
#define FAIL_LED 12

#define NUM_COLORS 4
enum {
    red,
    green,
    blue,
    yellow
} colors;

byte leds[NUM_COLORS] = {RED_LED,GREEN_LED,BLUE_BUTTON, YELLOW_BUTTON};
byte buttons[NUM_COLORS] = {RED_BUTTON, GREEN_BUTTON, BLUE_BUTTON, YELLOW_BUTTON};

long last = 0;
long current = 0;

#define MAX_SEQUENCE_SIZE 10
byte sequence[MAX_SEQUENCE_SIZE];

void setup(){
    Serial.begin(9600);
    byte i;
    for(i=0;i<NUM_COLORS;i++){
        pinMode(leds[i], OUTPUT);
        pinMode(buttons[0], INPUT);
    }
    pinMode(WHITE_BUTTON, INPUT);
    pinMode(SUCCESS_LED, OUTPUT);
    pinMode(FAIL_LED, OUTPUT);
}

void newSequence(byte s){
    memset(sequence,'\0',MAX_SEQUENCE_SIZE);
    byte i=0;
    long nextColor = 0;
    for(i=0;i < s && s < MAX_SEQUENCE_SIZE; i++){
        nextColor = random(0, 4);
        sequence[i] = nextColor;
    }
}

void loop(){
    current = millis();

    simonButtonHandler();
    restartButtonHandler();

    ledFlash();

    if((current - last) > 10000){
        postUpdate();
    }
    last = current;
    delay(100);
}

void simonButtonHandler(){
    byte i;
    int buttonState = 0;

    for(i=0;i<NUM_COLORS;i++){
        buttonState = digitalRead(buttons[i]);
        if(buttonState == HIGH){
            Serial.print("Button press: ");
            Serial.println(i);
        }
    }
}

void restartButtonHandler(){
    int buttonState = 0;
    buttonState = digitalRead(WHITE_BUTTON);
    if(buttonState == HIGH){
        Serial.print("Button press: ");
        Serial.println(WHITE_BUTTON);
    }
}

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
            digitalWrite(leds[i], 1);
            delay(100);
            digitalWrite(leds[i], 0);
            delay(100);
        }
    }
}

void postUpdate(){
}

