#pragma once

#include "EspNowHandler.h"
#include "spiffs_handler.h"

class EspNowHandler;
class spiffs_handler;

class Application{
private:
    EspNowHandler *espNow;
    spiffs_handler *spiffs;
    const int buttonPin = 0;
    unsigned long currentTime;
    unsigned long lastPress = 0;
    int pressCount;
    uint8_t receiverMAC[6];
    SemaphoreHandle_t bindingSemaphore;
public:
    Application(/* args */);
    void init();
    void sendingLoop();
    void bindingLoop();

};
