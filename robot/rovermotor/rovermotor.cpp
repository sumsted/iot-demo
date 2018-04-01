#include <Arduino.h>
#include <Servo.h>

#define PWM_PIN_LF 5
#define DIR_PIN_LF 10
#define PWM_PIN_RF 9
#define DIR_PIN_RF 6
#define LED_PIN 13
#define US_FRONT_TRIG_PIN 11
#define US_FRONT_ECHO_PIN 12

#define MOTOR_ORIENTATION_LEFT -1
#define MOTOR_ORIENTATION_RIGHT 1

// should be duty range for cytron mdd10a
#define PWM_FULL_FORWARD 0
#define PWM_STOP 127
#define PWM_FULL_BACKWARD 255
#define PWM_DELTA 127
#define PWM_TUNE_PERCENTAGE .75  // use to limit duty signal .5 = 50%

#define MOTOR_SMOOTHING_ITERATIONS 5
#define MOTOR_SMOOTHING_WAIT 10 // ms
#define MAX_BUFFER_SIZE 50

#define MINIMUM_SAFE_DISTANCE 15 // cm

bool commandProcessed = false; // check used by safety timer to tell if command issued
byte ledVal = HIGH; // safety timer flips the led on and off

int lastLeftPulse = PWM_STOP;
int lastRightPulse = PWM_STOP;


int runMotor(int leftSpeed, int rightSpeed, int sendSerial){
    int leftPulse = PWM_STOP;
    int rightPulse = PWM_STOP;
    int i;
    int deltaLeftPulse;
    int deltaRightPulse;

    leftPulse = PWM_STOP;
    rightPulse = PWM_STOP;

    if(leftSpeed > 100 || leftSpeed < -100 || rightSpeed > 100 || rightSpeed < -100){
        analogWrite(DIR_PIN_LF, lastLeftPulse);
        analogWrite(DIR_PIN_RF, lastRightPulse);
    } else {
        leftPulse = PWM_STOP + (PWM_DELTA * (leftSpeed * PWM_TUNE_PERCENTAGE)/100 * MOTOR_ORIENTATION_LEFT);
        rightPulse = PWM_STOP + (PWM_DELTA * (rightSpeed * PWM_TUNE_PERCENTAGE)/100 * MOTOR_ORIENTATION_RIGHT);
        deltaLeftPulse = (leftPulse - lastLeftPulse) / MOTOR_SMOOTHING_ITERATIONS;
        deltaRightPulse = (rightPulse - lastRightPulse) / MOTOR_SMOOTHING_ITERATIONS;
        for(i=0;i<MOTOR_SMOOTHING_ITERATIONS;i++){
            analogWrite(DIR_PIN_LF, lastLeftPulse+(deltaLeftPulse*i));
            analogWrite(DIR_PIN_RF, lastRightPulse+(deltaRightPulse*i));
            delay(MOTOR_SMOOTHING_WAIT);
        }
        analogWrite(DIR_PIN_LF, leftPulse);
        analogWrite(DIR_PIN_RF, rightPulse);
        commandProcessed = true;
    }

    lastLeftPulse = leftPulse;
    lastRightPulse = rightPulse;
    if(sendSerial){
        char result[100];
        sprintf(result, "{\"leftSpeed\":%d,\"rightSpeed\":%d,\"leftPulse\":%d,\"rightPulse\":%d}\n",
                leftSpeed, rightSpeed, leftPulse, rightPulse);
        Serial.write(result);
    }
    return 0;
}


int getDistance(int trigPin, int echoPin){
    long duration = 0;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH, 8730);
    if(duration < 0){
        return 0;
    } else if(duration >= 8730 || duration == 0){
        return 150;
    } else {
        return (duration/2) / 29.1;
    }
}

unsigned long lastDistanceCheckMs = 0;
void checkMinimumSafeDistance(){
    unsigned long currentMs = millis();
    if((currentMs - lastDistanceCheckMs) > 2000){
        int distance = getDistance(US_FRONT_TRIG_PIN, US_FRONT_ECHO_PIN);
        Serial.println(distance);
        if(distance < 15.0){
            runMotor(-50, -50, 0);
            delay(1000);
            runMotor(0, 0, 0);
        }
        lastDistanceCheckMs = currentMs;
    }
}


void serialHandler(){
    char readBuffer[MAX_BUFFER_SIZE] = "";
    String readString;

    while(Serial.available() > 0){ 
        readString = Serial.readStringUntil('!');
    }

    if(readString.length() > 0){

        readString.toCharArray(readBuffer, MAX_BUFFER_SIZE);
        char command = readBuffer[0];
        switch(command){
            case 'I':
                Serial.write("{\"id\":\"motor\"}\n");
                break;
            default:
                char leftSpeedBuffer[5];
                memcpy(leftSpeedBuffer, readBuffer+1, 4);
                leftSpeedBuffer[4] = '\0';
                int leftSpeed = atoi(leftSpeedBuffer);

                char rightSpeedBuffer[5];
                memcpy(rightSpeedBuffer, readBuffer+5, 4);
                rightSpeedBuffer[4] = '\0';
                int rightSpeed = atoi(rightSpeedBuffer);
                runMotor(leftSpeed, rightSpeed, 1);
        }

    }
}


void setup() {
    Serial.begin(9600);
    Serial.setTimeout(100);
    while(!Serial){}
    Serial.write("{\"id\":\"motor\"}\n");
    pinMode(LED_PIN, OUTPUT);

    // setting up for locked antiphase for cytron mdd10a
    // in this way it works like a frc pwm, one pwm signal
    // to control forward and reverse
    // the pulse is sent along the DIR channels while the PWM channel is always high
    pinMode(DIR_PIN_LF, OUTPUT);
    pinMode(PWM_PIN_LF, OUTPUT);
    pinMode(DIR_PIN_RF, OUTPUT);
    pinMode(PWM_PIN_RF, OUTPUT);
    pinMode(US_FRONT_TRIG_PIN, OUTPUT);
    pinMode(US_FRONT_ECHO_PIN, INPUT);

    digitalWrite(PWM_PIN_LF, HIGH);
    digitalWrite(PWM_PIN_RF, HIGH);
    analogWrite(DIR_PIN_LF, PWM_STOP);
    analogWrite(DIR_PIN_RF, PWM_STOP);
}

void testPwm(){
    int pwm_value = 0;
    int opposite_i;
    int pulse;
    digitalWrite(PWM_PIN_LF, HIGH);
    digitalWrite(PWM_PIN_RF, HIGH);
    for(pulse=0;pulse<=255;pulse++){
        analogWrite(DIR_PIN_LF, pulse);
        analogWrite(DIR_PIN_RF, pulse);
        delay(50);
        Serial.println(pulse);        
    }    
    for(pulse=255;pulse>=0;pulse--){
        analogWrite(DIR_PIN_LF, pulse);
        analogWrite(DIR_PIN_RF, pulse);
        delay(50);
        Serial.println(pulse);        
    }    
}


void loop() {
    // testPwm();
    digitalWrite(PWM_PIN_LF, HIGH);
    digitalWrite(PWM_PIN_RF, HIGH);

    serialHandler();
    checkMinimumSafeDistance();
}
