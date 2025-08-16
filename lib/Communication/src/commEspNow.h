#pragma once

#include <Arduino.h>
#include <WiFi.h>
#ifdef DISP
#include "arduinojson_fix.h"
#else
#include <ArduinoJson.h>
#endif
#include <esp_now.h>
#include <esp_wifi.h>
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
    const uint8_t* mac = nullptr;
    bool stateBinding = false;
    char dataFromReceiver[12] = "";

public:
    commEspNow(Buffer* audioBuffer, storage* memoryStorage, uint8_t wifiChannel);
    ~commEspNow();
    bool begin();
    void addPeer();
    void addPeer(const uint8_t* _mac);
    void sendData();
    
    // Fungsi audio
    void addSample(int16_t sample);
    void flush();
    
    // fungsi untuk display
    void sendVolume(int vol);
    void sendSirineSetting(const uint8_t *modelBuffer);

    // Fungsi untuk button
    void sendButton(int data);
    int getButtonValue() { return buttonValue; }
    
    // Fungsi binding
    void statusBinding();
    bool binding();
    bool setBinding(bool bindingState);
    bool getBinding() { return stateBinding; }
    
    // Getter
    const char* getReceivedMessage() { return dataFromReceiver; }
    
    // Header settings
    int setHeader(const int headerSize, const uint8_t* header);
    
    // Friend fungsi untuk callback
    friend void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
    friend void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
    friend void displayCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
    
    // Static task function for sending
    static void sendTask(void* parameter);
};

// Callback function declarations
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
void displayCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);