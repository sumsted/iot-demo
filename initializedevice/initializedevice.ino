/*
 * initialize device - write settings to eeprom of device
 * button 1 reads from eeprom and displays both default and eeprom configuration
 * button 2 writes default data to eeprom
 *
 * steps to use:
 * 1. set default macros to desired values
 * 2. compile and upload sketch to device
 * 3. check current eeprom data (button 1 left) (2 blinks for successful read)
 * 4. verify default settings are correct
 * 5. write default settings to eeprom (button 2 right) (4 blinks successful write)
 * 6. verify eeprom data (button 1 again)
 */
#include <EEPROM.h>

#define DEFAULT_SERIAL "DV01"
#define DEFAULT_DEVICE_ID "001"
#define DEFAULT_MODEL "Simon"
#define DEFAULT_FIRMWARE_REVISION "1.0"
#define DEFAULT_WIFI_SSID "testwifi"
#define DEFAULT_WIFI_PASSWORD "testpassword"
#define DEFAULT_LOCATION "123 Urgent Ave."
#define DEFAULT_GATEWAY_PROTOCOL "http"
#define DEFAULT_GATEWAY_HOST "jsonplaceholder.typicode.com"
#define DEFAULT_GATEWAY_PORT 80
#define DEFAULT_GATEWAY_PATH "/users"

#define DEFAULT_STATE_0 "Happy"
#define DEFAULT_STATE_1 "Sad"
#define DEFAULT_STATE_2 "Groovy"
#define DEFAULT_STATE_3 "Bad"

#define STATUS_LED 10

#define BUTTON_1 11
#define BUTTON_2 12

bool getEeprom = false;
bool setEeprom = false;

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
    uint16_t gatewayPort;
    char gatewayPath[51];
    char state[4][11];
    char padding[15];
} ConfigurationType;

union ConfigurationUnion{
    ConfigurationType configuration;
    unsigned char bytes[250];
};


ConfigurationUnion defaultConfig;
ConfigurationUnion eepromConfig;

void setup() {
    Serial.begin(57600);
    Serial.println("start");

    initializeDefaultConfig();
    readEepromConfig();

    pinMode(STATUS_LED, OUTPUT);

    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);

    attachInterrupt(INT0, button1Handler, CHANGE);
    attachInterrupt(INT1, button2Handler, CHANGE);
}

void loop() {
    if(getEeprom){
        readEepromConfig();
        blink(2);
        getEeprom = false;
    }
    if(setEeprom){
        writeDefaultConfig();
        blink(4);
        setEeprom= false;
    }
}

void blink(byte times){
    for(byte i=0; i<times; i++){
        digitalWrite(STATUS_LED, 1);
        delay(100);
        digitalWrite(STATUS_LED, 0);
        delay(100);
    }
}

void initializeDefaultConfig(){
    memset(&defaultConfig, '\0', sizeof(defaultConfig));
    strcpy(defaultConfig.configuration.serial, DEFAULT_SERIAL);
    strcpy(defaultConfig.configuration.deviceId, DEFAULT_DEVICE_ID);
    strcpy(defaultConfig.configuration.model, DEFAULT_MODEL);
    strcpy(defaultConfig.configuration.firmware, DEFAULT_FIRMWARE_REVISION);
    strcpy(defaultConfig.configuration.wifiSsid, DEFAULT_WIFI_SSID);
    strcpy(defaultConfig.configuration.wifiPassword, DEFAULT_WIFI_PASSWORD);
    strcpy(defaultConfig.configuration.location, DEFAULT_LOCATION);
    strcpy(defaultConfig.configuration.gatewayProtocol,DEFAULT_GATEWAY_PROTOCOL);
    strcpy(defaultConfig.configuration.gatewayHost,DEFAULT_GATEWAY_HOST);
    defaultConfig.configuration.gatewayPort = DEFAULT_GATEWAY_PORT;
    strcpy(defaultConfig.configuration.gatewayPath,DEFAULT_GATEWAY_PATH);
    strcpy(defaultConfig.configuration.state[0],DEFAULT_STATE_0);
    strcpy(defaultConfig.configuration.state[1],DEFAULT_STATE_1);
    strcpy(defaultConfig.configuration.state[2],DEFAULT_STATE_2);
    strcpy(defaultConfig.configuration.state[3],DEFAULT_STATE_3);
    Serial.println("default config initialized");
    printConfig("default", &defaultConfig);
}

void readEepromConfig(){
    int i = 0;
    memset(&eepromConfig, '\0', sizeof(eepromConfig));
    for(i=0;i<sizeof(eepromConfig);i++){
        eepromConfig.bytes[i] = EEPROM.read(i);
    }
    Serial.println("eeprom configuration read");
    printConfig("eeprom", &eepromConfig);
    Serial.println("default configuration is");
    printConfig("default", &defaultConfig);
}

void writeDefaultConfig(){
    int i = 0;
    Serial.println("writing default to eeprom");
    printConfig("default", &defaultConfig);
    for(i=0;i<sizeof(eepromConfig);i++){
        EEPROM.write(i, defaultConfig.bytes[i]);
    }
    Serial.println("write complete");
    readEepromConfig();
    Serial.println("confirmed eeprom config");
    printConfig("eeprom", &eepromConfig);
}

void printConfig(char *label, ConfigurationUnion *config){
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
    Serial.print("gatewayProtocol: ");
    Serial.println(config->configuration.gatewayProtocol);
    Serial.print("gatewayHost: ");
    Serial.println(config->configuration.gatewayHost);
    Serial.print("gatewayPort: ");
    Serial.println(config->configuration.gatewayPort);
    Serial.print("gatewayPath: ");
    Serial.println(config->configuration.gatewayPath);
    Serial.print("State 0: ");
    Serial.println(config->configuration.state[0]);
    Serial.print("State 1: ");
    Serial.println(config->configuration.state[1]);
    Serial.print("State 2: ");
    Serial.println(config->configuration.state[2]);
    Serial.print("State 3: ");
    Serial.println(config->configuration.state[3]);
}

void button1Handler(){
    long buttonPress = millis();
    if(buttonPress > button1LastPress+1000){
        getEeprom = true;
        button1LastPress = buttonPress;
    }
}

void button2Handler(){
    long buttonPress = millis();
    if(buttonPress > button2LastPress+1000){
        setEeprom = true;
        button2LastPress = buttonPress;
    }
}
