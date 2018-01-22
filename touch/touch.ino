
#define RED_LIGHT 27
#define YELLOW_LIGHT 25
#define GREEN_LIGHT 33
#define EMA_A .125

int emaS;

void setup(){
    pinMode(RED_LIGHT, OUTPUT);
    pinMode(YELLOW_LIGHT, OUTPUT);
    pinMode(GREEN_LIGHT, OUTPUT);
    Serial.begin(9600);
    emaS = touchRead(T4);
}

void loop(){
    int touchiness = touchRead(T4);
    int filtered = lowPassFilter(touchiness);
    Serial.print(touchiness);
    Serial.print(", ");
    Serial.println(filtered);
    delay(50);
    if(filtered<20){
        digitalWrite(RED_LIGHT, 0);
        digitalWrite(YELLOW_LIGHT, 0);
        digitalWrite(GREEN_LIGHT, 1);
    } else if(filtered > 40){
        digitalWrite(RED_LIGHT, 1);
        digitalWrite(YELLOW_LIGHT, 0);
        digitalWrite(GREEN_LIGHT, 0);
    } else {
        digitalWrite(RED_LIGHT, 0);
        digitalWrite(YELLOW_LIGHT, 1);
        digitalWrite(GREEN_LIGHT, 0);
    }
}

int lowPassFilter(int touchiness){
    emaS = (EMA_A*touchiness) + ((1-EMA_A)*emaS);
    return emaS;
}
