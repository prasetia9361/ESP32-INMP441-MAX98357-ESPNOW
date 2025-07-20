#include <Arduino.h>
#include "./ui/ui.h"

#include "./screen/screen.h"
#include "commEspNow.h"
#include "buffer.h"
#include "storage.h"

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other
// applications or walkie talkie sets), specify TRANSPORT_HEADER_SIZE (the
// length in bytes of the header) in the next line, and define the transport
// header in config.cpp
#define TRANSPORT_HEADER_SIZE 10
extern uint8_t transportHeader[TRANSPORT_HEADER_SIZE];


class displayTask
{
private:
    commEspNow *mCommunication;
    Buffer *mOutputBuffer;
    storage *mMemory;
    Screen *mScreen;

    int tones[8];
    char macStr[18];
    uint8_t *macAddress = nullptr;
    uint8_t* convertTouint8t(int32_t* dataInt, size_t size);

    lv_event_t gEventSending;
    lv_event_t gEventBinding;
    lv_event_t gEventDelete;

public:
    displayTask();
    ~displayTask();
    void begin();
    void showDataReceive();
    void loadPage();
    void binding();
    void deleteAddress();
    void sendVolume();
    void sendTones();
    void testing();
    void changeScreen();
};