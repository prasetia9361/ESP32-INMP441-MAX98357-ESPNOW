#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include "storage.h"
#include "buffer.h"

const int maxEspNowPacketSize = 127;

class commEspNow {
private:
    typedef struct message {
        uint8_t buffer[maxEspNowPacketSize];
        char data[20] = "";
        int dataLen;
    } message;
    
    message messageData;
    Buffer* audioBuffer;
    storage* memoryStorage;
    uint8_t wifiChannel;
    int bufferSize;
    int index;
    int headerSize;
    int lastData;
    int buttonValue = 0;
    bool stateBinding = false;
    char dataFromReceiver[12] = "";

public:
    commEspNow(Buffer* audioBuffer, storage* memoryStorage, uint8_t wifiChannel);
    bool begin();
    void addPeer();
    void sendData();
    
    // Fungsi audio
    void addSample(int16_t sample);
    void flush();
    
    // Fungsi untuk button
    void sendButton(int data);
    int getButtonValue() { return buttonValue; }
    
    // Fungsi binding
    void statusBinding();
    bool setBinding(bool bindingState);
    bool getBinding() { return stateBinding; }
    
    // Getter
    const char* getReceivedMessage() { return dataFromReceiver; }
    
    // Header settings
    int setHeader(const int headerSize, const uint8_t* header);
    
    // Friend fungsi untuk callback
    friend void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
    friend void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
};

// Callback function declarations
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);