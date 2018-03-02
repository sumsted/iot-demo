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


int Requests::postAdaIo(const char *user, const char *feed, int value){
    HTTPClient http;
    char payload[1000];
    char url[1000];
    const char *payloadPattern = "{\"value\":%d}";
    const char *urlPattern = "https://io.adafruit.com/api/v2/%s/feeds/%s/data";
    int code;

    sprintf(payload, payloadPattern, value);
    sprintf(url, urlPattern, user, feed);

    http.begin(url, rootCa);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-AIO-Key", "c8d50f2a58754014abe65dee32b78655");
    code = http.POST(payload);

    if(code > 0){
        String response = http.getString();
        Serial.println(code);
        Serial.println(response);
    }else{
        Serial.print("problem posting to adaio: ");
        Serial.println(code);
        Serial.print("url: ");
        Serial.println(url);
        Serial.print("payload: ");
        Serial.println(payload);
    }
    http.end();
}


void Requests::establishWifiConnection(){
    wifiConnected = 0;
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
    wifiConnected = 1;
}

void Requests::printConfig(const char *label, ConfigurationUnion *config){
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