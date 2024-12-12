#ifndef MEMORY_H
#define MEMORY_H

// #include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"

class memory{
private:
    typedef struct config {
        uint8_t macAddress[6] = {0,0,0,0,0,0};
        uint8_t macAddress1[6] = {0,0,0,0,0,0};
    } config;
    config configData;

    String receivedData;
    uint8_t macAddr[6];

public:
    memory();
    ~memory();

    uint8_t *getMac(){return configData.macAddress;}
    uint8_t *getMac1(){return configData.macAddress1;}

    void init();
    void writeMacAddress(const uint8_t *mac, int count);
    void deleteAddress();
};
#endif