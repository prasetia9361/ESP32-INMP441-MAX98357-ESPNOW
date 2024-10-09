#ifndef SPIFFS_HANDLER_H
#define SPIFFS_HANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"

// class EspNowHandler;
class spiffs_handler{
private:
    // EspNowHandler* espNow;
    String receivedData;
    uint8_t macAddr[6];
    typedef struct config {
        uint8_t macAddress[6];
    } config;
    config configData;
    
    // unsigned long currentMillis;
    // unsigned long lastSend = 0;
    // const char *output;
    // uint8_t* incomingData;
    bool exists(){return SPIFFS.exists("/receiverMAC.bin");}
    File readData(){return SPIFFS.open("/receiverMAC.bin", FILE_READ);}
public:
    spiffs_handler();
    void init();
    void writeMacAddress(const uint8_t *mac);
    void readMacAddress();
    uint8_t *getMac(){return configData.macAddress;}
    void write(const uint8_t* reciveMac, const uint8_t* incomingData);
    void readClose(uint8_t *mac);
};

#endif