#ifndef SPIFFSHANDLER_H
#define SPIFFSHANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"

class spiffsHandler{
private:
    typedef struct config {
        uint8_t macAddress[6] = {0,0,0,0,0,0};
    } config;
    config configData;

    String receivedData;
    uint8_t macAddr[6];

public:
    spiffsHandler();

    uint8_t *getMac(){return configData.macAddress;}

    void init();
    void writeMacAddress(const uint8_t *mac);
};
#endif