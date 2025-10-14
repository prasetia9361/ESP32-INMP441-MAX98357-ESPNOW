#pragma once

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

    void begin();
    bool commBegin();
    void tick();
    void testSiren();
    void updateData();
    void updateMAc();
    int latesButton;

private:
    uint8_t* convertTouint8t(int32_t* dataInt, size_t size);
    Screen* mScreen;
    storage* mMemory;
    Buffer* mOutputBuffer;
    commEspNow* mCommunication;
    
    int32_t* toneSelected;        
    uint8_t* macAddress;

    int state = 0;
    bool isBinding = false;
    bool isSaveSiren = false;
    bool isSendVol = false;
    int latsVol;
    char macStr[32];
    char deviceSelected[12];
    int32_t * _getMode = nullptr;
    int vol = 0;
    bool _swtich = false;
    int _button = 0;
    int sirenTone = 0;
    SemaphoreHandle_t _smphr;
    SemaphoreHandle_t _sTest;
};
