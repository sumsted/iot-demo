#include "Requests.h"


Requests::Requests(){
//    readEepromConfig();
    establishWifiConnection();
}

Requests::Requests(ConfigurationUnion *newConfig){
//    writeNewConfig(newConfig);
//    readEepromConfig();
    config = newConfig;
    establishWifiConnection();
}

int Requests::getRequest(char *payload){
    WiFiClient client;
    char buffer[500];
    char *action = "GET";
    sprintf(buffer, "%s %s%s %s/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
            action, config->configuration.gatewayPath, payload, config->configuration.gatewayProtocol,
            config->configuration.model, config->configuration.deviceId);
    if(!client.connect(config->configuration.gatewayHost, config->configuration.gatewayPort)){
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

int Requests::postRequest(char *payload){
    WiFiClient client;
    char buffer[500];
    char *action = "POST";
    sprintf(buffer, "%s %s %s/1.1\r\nHost: %s%s\r\nConnection: close\r\nContent-length: %d\r\n\r\n%s\r\n",
            action, config->configuration.gatewayPath, config->configuration.gatewayProtocol,
            config->configuration.model, config->configuration.deviceId, strlen(payload), payload);
    if(!client.connect(config->configuration.gatewayHost, config->configuration.gatewayPort)){
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


void Requests::establishWifiConnection(){
    byte tries=0;
    Serial.print("establishwificonnection: ");
    Serial.print(config->configuration.wifiSsid);
    Serial.print(", ");
    Serial.print(config->configuration.wifiPassword);
    WiFi.begin(config->configuration.wifiSsid, config->configuration.wifiPassword);
    while(WiFi.status() != WL_CONNECTED){
        Serial.print("attempting to connect, try: ");
        Serial.println(tries);
        tries++;
        if(tries > 10){
            Serial.print("failed to establish connection to wifi: ");
            Serial.println(config->configuration.wifiSsid);
        }
        delay(500);
    }
    Serial.print("connected to wifi with IP address: ");
    localIp = WiFi.localIP();
}

//void Requests::readEepromConfig(){
//    int i = 0;
//    memset(&config, '\0', sizeof(config));
//    for(i=0;i<sizeof(config);i++){
//        config.bytes[i] = EEPROM.read(i);
//    }
//    Serial.println("eeprom configuration read");
//    printConfig("eeprom", &config);
//}
//
//void Requests::writeNewConfig(ConfigurationUnion *newConfig){
//    int i=0;
//    Serial.println("writing new config to eeprom");
//    for(i=0;i<sizeof(*newConfig);i++){
//        EEPROM.write(i, newConfig->bytes[i]);
//    }
//    Serial.println("write complete");
//    readEepromConfig();
//    Serial.println("confirmed eeprom config");
//    printConfig("eeprom", &config);
//}

void Requests::printConfig(char *label, ConfigurationUnion *config){
    Serial.print("\nConfiguration: ");
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