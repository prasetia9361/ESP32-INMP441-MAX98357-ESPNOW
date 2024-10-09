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
    typedef struct config {
        uint8_t macAddress[6];
    } config;
    config configData;
    
    String receivedData;
    uint8_t macAddr[6];

public:
    spiffs_handler();

    uint8_t *getMac(){return configData.macAddress;}

    void init();
    void writeMacAddress(const uint8_t *mac);
};
#endif