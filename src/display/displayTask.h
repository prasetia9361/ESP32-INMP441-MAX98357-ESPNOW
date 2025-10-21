#ifndef DISPLAYTASK_H
#define DISPLAYTASK_H

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// #include "eez-flow.h"
#include "ui.h"
#include "vars.h"
#include "screen/screen.h"
#include "storage.h"
#include "Buffer.h"
#include "commEspNow.h"
#include "structs.h" 
class Screen;
class storage;
class Buffer;
class commEspNow;

enum DeviceType {
    device1 = 0,
    device2 = 1
};



class displayTask {
public:
    displayTask();
    ~displayTask();

    bool begin();
    bool initializeProcessData();
    bool lvglInit();
    void tick();
    void testSiren();
    void updateData();
    void loop();
    void processData();
    int latesButton;

private:
    uint8_t* convertTouint8t(int32_t* dataInt, size_t size);
    Screen* mScreen;
    storage* mMemory;
    Buffer* mOutputBuffer;
    commEspNow* mCommunication;
    
    int32_t* toneSelected;        
    uint8_t* macAddress;
    uint8_t* addr1;
    uint8_t* addr2;
    char *m_device1;
    char *m_device2;

    int state = 0;
    int lastState = 0;
    bool isBinding = false;
    bool isSaveSiren = false;
    bool isSendVol = false;
    int latsVol;
    char macStr[32];
    const char *deviceSelected = nullptr;
    int32_t * _getMode = nullptr;
    int vol = 0;
    bool _switch = false;
    int _button = 0;
    int sirenTone = 0;
    SemaphoreHandle_t _smphr;
    SemaphoreHandle_t _initSemaphore; // Tambahkan ini
    SemaphoreHandle_t _sTest;
};
#endif // DISPLAYTASK_H
