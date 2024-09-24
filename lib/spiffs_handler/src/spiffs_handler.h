#ifndef SPIFFS_HANDLER_H
#define SPIFFS_HANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include "FS.h"
#include "SD.h"

class EspNowHandler;
class spiffs_handler{
private:
    EspNowHandler* espNow;
    String receivedData;
    unsigned long currentMillis;
    unsigned long lastSend = 0;
    bool exists(){return SPIFFS.exists("/receiverMAC.bin");}
    File readData(){return SPIFFS.open("/receiverMAC.bin", FILE_READ);}
public:
    spiffs_handler();
    void init(EspNowHandler* _espNow);
    void write(const uint8_t * mac, const uint8_t* incomingData);
    void readClose(uint8_t * mac);
};

#endif