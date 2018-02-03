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
    ledFlash();
    resetGame();
}


void loop(){
    current = millis();
    if((current - last) > 10000){
        postUpdate();
    }
    last = current;

    if(redAction==true){
        testButtonPush(red);
        redAction = false;
    }
    if(greenAction==true){
        testButtonPush(green);
        greenAction = false;
    }
    if(blueAction ==true){
        testButtonPush(blue);
        blueAction = false;
    }
    if(yellowAction==true){
        testButtonPush(yellow);
        yellowAction = false;
    }
    if(whiteAction==true){
        blink(SUCCESS_LED);
        blink(FAIL_LED);
        gameSize = 0;
        resetGame();
        whiteAction = false;
    }

}


/*
 * Game code
 */
void createSequence(byte s){
    memset(sequence,'\0',MAX_SEQUENCE_SIZE);
    byte i=0;
    long nextColor = 0;
    for(i=0;i < s && s < MAX_SEQUENCE_SIZE; i++){
        nextColor = random(0, 4);
        sequence[i] = nextColor;
        Serial.println(nextColor);
    }
}

void resetGame(){
    playerIndex = 0;
    gameSize++;
    if(gameSize == MAX_SEQUENCE_SIZE){
        gameSize = 1;
    }
    createSequence(gameSize);
    showSequence();
}

bool testButtonPush(byte colorChosen){
    if(colorChosen == sequence[playerIndex]){
        blink(leds[colorChosen]);
        playerIndex++;
        if(playerIndex == gameSize){
            blink(SUCCESS_LED);
            resetGame();
        }
    } else {
        blink(FAIL_LED);
        blink(FAIL_LED);
        gameSize = 0;
        resetGame();
    }
}

void showSequence(){
    byte i;
    blink(SUCCESS_LED);
    for(i=0;i<gameSize;i++){
        blink(leds[sequence[i]]);
    }
}

/*
 * Send the state out
 */
void postUpdate(){
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
