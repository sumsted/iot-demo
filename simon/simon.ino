#define RED_BUTTON 1
#define GREEN_BUTTON 1
#define BLUE_BUTTON 1
#define YELLOW_BUTTON 1
#define WHITE_BUTTON 1

#define RED_LED 1
#define GREEN_LED 1
#define BLUE_LED 1
#define YELLOW_LED 1

#define SUCCESS_LED 1
#define FAIL_LED 1

enum {
    red,
    green,
    blue,
    yellow
} colors;

byte leds[sizeof(colors)] = {RED_LED,GREEN_LED,BLUE_BUTTON,4};
byte buttons[sizeof(colors)] = {1,2,3,4};

#define MAX_SEQUENCE_SIZE 10
byte sequence[MAX_SEQUENCE_SIZE];

void setup(){



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

}