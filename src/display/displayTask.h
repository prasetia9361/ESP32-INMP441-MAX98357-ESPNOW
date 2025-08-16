#pragma once

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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


class displayTask {
public:
    displayTask();
    ~displayTask();

    void begin();
    void tick();
    void updateData();
    void testSiren();

private:
    uint8_t* convertTouint8t(int32_t* dataInt, size_t size);
    Screen* mScreen;
    storage* mMemory;
    Buffer* mOutputBuffer;
    commEspNow* mCommunication;
    
    int32_t* toneSelected;        
    uint8_t* macAddress;

    int latsVol;
    char macStr[32];
};
