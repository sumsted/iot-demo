/*
 * device iot - act as an iot device on a company network
 * button 1 alternates between four states, 0 to 3, state data stored in eeprom
 * button 2 writes state to iot gateway
 *
 * This code may be used on a number of esp32 devices. behavior is determined by eeprom config
 */
#include <EEPROM.h>
#include <WifFi.h>

#define LED_1 8
#define LED_2 9
#define STATUS_LED 10

#define BUTTON_1 11
#define BUTTON_2 12

bool postTelemetry = false;
bool postToGateway = false;
long button1LastPress = 0;
long button2LastPress = 0;

typedef struct {
    char serial[11];
    char deviceId[11];
    char model[11];
    char firmware[11];
    char wifiSsid[11];
    char wifiPassword[21];
    char location[51];
    char gatewayProtocol[11];
    char gatewayHost[51];
    char gatewayPort[11];
    char gatewayPath[51];
    char state[4][11];
    char padding[6];
} ConfigurationType;

union ConfigurationUnion{
    ConfigurationType configuration;
    byte bytes[250];
};

ConfigurationUnion config;

typedef struct {
    char serial[11];
    char deviceId[11];
    char model[11];
    char state[11];
    int data;
    char padding[54];
} telemetry_type;

union TelemetryUnion {
    TelemetryType telemetry;
    byte bytes[100];
};

TelemetryUnion telem;

void setup() {
    Serial.begin(57600);
    Serial.print(telemetry.serial);
    Serial.println(" start");

    config = readEepromConfig();

    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);

    attachInterrupt(INT0, button1Handler, CHANGE);
    attachInterrupt(INT1, button2Handler, CHANGE);

    establishWifiConnection();
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

char *establishWifiConnection(){
    byte tries =0
    WiFi.begin(config.wifiSsid, config.wifiPassword);
    while(Wifi.status() != WL_CONNECTED){
        blink(1);
        Serial.print("attempting to connect, try: ");
        Serial.println(tries);
        tries++;
        if(tries > 10){
            Serial.print("failed to establish connection to wifi: ");
            Serial.println(config.wifiSsid);
            return NULL;
        }
        delay(500);
    }
    Serial.print("connected to wifi with IP address: ");
    Serial.println(config.WiFi.localIP());
    return Wifi.localIP();
}

int getRequest(char *payload){
    WiFiClient client;
    char buffer[500];
    char *action = "GET";
    sprintf(buffer, "%s %s%s %s/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
            action, config.gatewayPath, payload, config.gatewayProtocol, config.model, config.deviceId);
    if(!client.connect(config.gatewayHost, config.gatewayPort)){
        Serial.println("client connection failed");
        return NULL;
    }
    client.print(buffer);
    unsigned long sendTime = millis();
    while(client.available() == 0){
        if(millis() - sendTime > 5000){
            Serial.println("client timeout");
            return NULL;
        }
    }
    while(client.available()){
        //read data
        String response = client.readStringUntil('\r');
        Serial.println(response);
    }
    client.stop();
    Serial.println("connection closed");
}

int postRequest(char *payload){
    WiFiClient client;
    char buffer[500];
    char *action = "POST";
    sprintf(buffer, "%s %s %s/1.1\r\nHost: %s%s\r\nConnection: close\r\nContent-length: %d\r\n\r\n%s\r\n",
            action, config.gatewayPath, config.gatewayProtocol, config.model, deviceId, strlen(payload), payload);
    if(!client.connect(config.gatewayHost, config.gatewayPort)){
        Serial.println("client connection failed");
        return NULL;
    }
    client.print(buffer);
    unsigned long sendTime = millis();
    while(client.available() == 0){
        if(millis() - sendTime > 5000){
            Serial.println("client timeout");
            return NULL;
        }
    }
    while(client.available()){
        //read data
        String response = client.readStringUntil('\r');
        Serial.println(response);
    }
    client.stop();
    Serial.println("connection closed");
}

void postToGateway(){
    // todo prepare request

    // todo post request
    int status = 1

    blink(STATUS_LED, 5);
}

void getFromGateway(){
    // todo call config
    int status = 1;
    // todo set eprom config
    blink(STATUS_LED, 2);
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

void readEepromConfig(){
    int i = 0;
    memset(eepromConfig, '\0', sizeof(config));
    for(i=0;i<sizeof(eepromConfig);i++){
        eepromConfig.bytes[i] = EEPROM.read(i);
    }
    Serial.println("eeprom configuration read");
    printConfig("eeprom", eepromConfig);
}


void writeNewConfig(ConfigurationUnion *newConfig){
    Serial.println("writing new config to eeprom");
    for(i=0;i<sizeof(newConfig);i++){
        EPROM.write(i, newConfig.bytes[i]);
    }
    Serial.println("write complete");
    config = readEepromConfig();
    Serial.println("confirmed eeprom config");
    printConfig("eeprom", config);
}

void printConfig(*label, *config){
    Serial.print("Configuration: ");
    Serial.println(label);
    Serial.print("serial: ");
    Serial.println(config->configuration.serial);
    Serial.print("deviceId: ");
    Serial.println(config->configuration.deviceId);
    Serial.print("model: ");
    Serial.println(config->configuration.model);
    Serial.print("firmware: ");
    Serial.println(config->configuration.firmware);
    Serial.print("location: ");
    Serial.println(config->configuration.location);
    Serial.print("gatewayUrl: ");
    Serial.println(config->configuration.gatewayUrl);
}


void blink(led, times){
    for(byte i=0; i<times; i++){
        digitalWrite(led, 1);
        delay(100);
        digitalWrite(led, 0);
        delay(100);
    }
}