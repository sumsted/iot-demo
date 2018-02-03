#define RED_BUTTON 39
#define GREEN_BUTTON 34
#define BLUE_BUTTON 38
#define YELLOW_BUTTON 37
#define WHITE_BUTTON 36

#define RED_LED 26
#define GREEN_LED 14
#define BLUE_LED 27
#define YELLOW_LED 25

#define SUCCESS_LED 13
#define FAIL_LED 12

#define NUM_COLORS 4
#define BUTTON_WAIT 250

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
    blink(SUCCESS_LED);
    blink(FAIL_LED);
    blink(GREEN_LED);
    blink(RED_LED);
    blink(BLUE_LED);
    blink(YELLOW_LED);
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
    if((current - last) > 10000){
        postUpdate();
    }
    last = current;

    if(redAction==true){
        blink(RED_LED);
        redAction = false;
    }
    if(greenAction==true){
        blink(GREEN_LED);
        greenAction = false;
    }
    if(blueAction ==true){
        blink(BLUE_LED);
        blueAction = false;
    }
    if(yellowAction==true){
        blink(YELLOW_LED);
        yellowAction = false;
    }
    if(whiteAction==true){
        blink(SUCCESS_LED);
        blink(FAIL_LED);
        whiteAction = false;
    }

//    delay(100);
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
            digitalWrite(leds[j], 1);
            delay(100);
            digitalWrite(leds[j], 0);
            delay(100);
        }
    }
}

void postUpdate(){
}

void greenButtonPush(){
    long buttonPress = millis();
    Serial.println("greenbuttonpush");
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
    Serial.println("redbuttonpush");
    if(buttonPress > presses[red]+BUTTON_WAIT){
        redAction = true;
        presses[red] = buttonPress;
    }
}

void blueButtonPush(){
    long buttonPress = millis();
    Serial.println("bluebuttonpush");
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
    Serial.println("yellowbuttonpush");
    if(buttonPress > presses[yellow]+BUTTON_WAIT){
        yellowAction = true;
        presses[yellow] = buttonPress;
    }
}

void whiteButtonPush(){
    long buttonPress = millis();
    Serial.println("whitebuttonpush");
    if(buttonPress > lastWhiteButtonPress+BUTTON_WAIT){
        whiteAction = true;
        lastWhiteButtonPress = buttonPress;
    }
}

void blink(byte pin){
    Serial.print("blink:");
    Serial.println(pin);
    digitalWrite(pin, 1);
    delay(50);
    digitalWrite(pin, 0);
}