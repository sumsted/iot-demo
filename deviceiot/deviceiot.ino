#define LED_1 8
#define LED_2 9
#define STATUS_LED 10

#define BUTTON_1 11
#define BUTTON_2 12

#define WIFI_SSD = "something"
#define WIFI_PASSWORD = "else"

#define IOT_GATEWAY = "http://iotgateway.purplepromise.xyz/"

bool postTelemetry = false;
bool postToGateway = false;
long button1LastPress = 0;
long button2LastPress = 0;

typedef struct {
    char serial[4];
    char device_id[4];
    char model[4];
    char state[4];
    int data;
    char action[4];
} telemetry_type;

union TelemetryUnion {
    TelemetryType telemetry;
    byte bytes[22];
};

TelemetryUnion telem;

telem.telemetry = {"AB01", "0001", 0, "    ", "    "};

void setup() {
    Serial.begin(57600);
    Serial.print(telemetry.serial);
    Serial.println(" start");

    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);

    attachInterrupt(INT0, button1Handler, CHANGE);
    attachInterrupt(INT1, button2Handler, CHANGE);
}

void loop() {
    if(postTelemetry){
        postToGateway();
        postTelemetry = false;
    }
    if(getConfiguration){
        getFromGateway();
        getConfiguration = false;
    }
}

void postToGateway(){
    // todo prepare request

    // todo post request
    int status = 1

    for(byte i=0; i<5; i++){
        digitalWrite(STATUS_LED, 1);
        delay(100);
        digitalWrite(STATUS_LED, 0);
        delay(100);
    }
}

void getFromGateway(){
    // todo call config
    int status = 1;
    // todo set eprom config
}

byte counter = 0;
byte state = 0;

void button1Handler(){
    long buttonPress = millis();
    if(buttonPress > button1LastPress+1000){
        state = counter % 4;
        switch(state){
            case 0:
                digitalWrite(LED_1, 0);
                digitalWrite(LED_2, 0);
                break;
            case 1:
                digitalWrite(LED_1, 1);
                digitalWrite(LED_2, 0);
                break;
            case 2:
                digitalWrite(LED_1, 0);
                digitalWrite(LED_2, 1);
                break;
            case 3:
                digitalWrite(LED_1, 1);
                digitalWrite(LED_2, 1);
                break;
        }
        button1LastPress = buttonPress;
    }
}

void button1Handler(){
    long buttonPress = millis();
    if(buttonPress > button2LastPress+1000){
        postTelemetry = true;
        button2LastPress = buttonPress;
    }
}
