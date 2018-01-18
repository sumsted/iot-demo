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

#define DEFAULT_SERIAL "DV01";
#define DEFAULT_DEVICE_ID "001";
#define DEFAULT_MODEL "Dropbox";
#define DEFAULT_FIRMWARE_REVISION "1.0"
#define DEFAULT_WIFI_SSID "testwifi"
#define DEFAULT_WIFI_PASSWORD "testpassword"
#define DEFAULT_LOCATION "123 Urgent Ave."
#define DEFAULT_GATEWAY_PROTOCOL "http";
#define DEFAULT_GATEWAY_HOST "iot.purplepromise.xyz";
#define DEFAULT_GATEWAY_PORT "443";
#define DEFAULT_GATEWAY_PATH "/";

#define DEFAULT_STATE_0 "Happy"
#define DEFAULT_STATE_1 "Sad"
#define DEFAULT_STATE_2 "Groovy"
#define DEFAULT_STATE_3 "Bad"

#define STATUS_LED 10

#define BUTTON_1 11
#define BUTTON_2 12

bool getEeprom = false
bool setEeprom = false

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
        postTelemetry = false;
    }
    if(setEeprom){
        writeDefaultConfig();
        blink(4);
        getConfiguration = false;
    }
}

void blink(times){
    for(byte i=0; i<times; i++){
        digitalWrite(STATUS_LED, 1);
        delay(100);
        digitalWrite(STATUS_LED, 0);
        delay(100);
    }
}

void initializeDefaultConfig(){
    memset(defaultConfig, '\0', sizeof(config));
    strcpy(defaultConfig.serial, DEFAULT_SERIAL);
    strcpy(defaultConfig.deviceId, DEFAULT_DEVICE_ID);
    strcpy(defaultConfig.model, DEFAULT_MODEL);
    strcpy(defaultConfig.firmware, DEFAULT_FIRMWARE_REVISION);
    strcpy(defaultConfig.wifiSsid, DEFAULT_WIFI_SSID);
    strcpy(defaultConfig.wifiPassword, DEFAULT_WIFI_PASSWORD);
    strcpy(defaultConfig.location, DEFAULT_LOCATION);
    strcpy(defaultConfig.gatewayProtocol,DEFAULT_GATEWAY_PROTOCOL);
    strcpy(defaultConfig.gatewayHost,DEFAULT_GATEWAY_HOST);
    strcpy(defaultConfig.gatewayPort,DEFAULT_GATEWAY_PORT);
    strcpy(defaultConfig.gatewayPath,DEFAULT_GATEWAY_PATH);
    strcpy(defaultConfig.state[0],DEFAULT_STATE_0);
    strcpy(defaultConfig.state[1],DEFAULT_STATE_1);
    strcpy(defaultConfig.state[2],DEFAULT_STATE_2);
    strcpy(defaultConfig.state[3],DEFAULT_STATE_3);
    Serial.println("default config initialized");
    printConfig("default", defaultConfig);
}

void readEepromConfig(){
    int i = 0;
    memset(eepromConfig, '\0', sizeof(config));
    for(i=0;i<sizeof(eepromConfig);i++){
        eepromConfig.bytes[i] = EEPROM.read(i);
    }
    Serial.println("eeprom configuration read");
    printConfig("eeprom", eepromConfig);
    Serial.println("default configuration is");
    printConfig("default", defaultConfig);
}

void writeDefaultConfig(){
    Serial.println("writing default to eeprom");
    printConfig("default", defaultConfig);
    for(i=0;i<sizeof(eepromConfig);i++){
        EPROM.write(i, defaultConfig.bytes[i]);
    }
    Serial.println("write complete");
    readEepromConfig();
    Serial.println("confirmed eeprom config");
    printConfig("eeprom", eepromConfig);
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

void button1Handler(){
    long buttonPress = millis();
    if(buttonPress > button1LastPress+1000){
        getEeprom = true;
        button1LastPress = buttonPress;
    }
}

void button1Handler(){
    long buttonPress = millis();
    if(buttonPress > button2LastPress+1000){
        setEeprom = true;
        button2LastPress = buttonPress;
    }
}
