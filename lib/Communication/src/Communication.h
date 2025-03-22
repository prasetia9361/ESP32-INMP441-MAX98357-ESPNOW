#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include "memory.h"
#include "audio.h"

const int MAX_ESP_NOW_PACKET_SIZE = 127;

class Communication {
private:
    typedef struct message {
        uint8_t buffer[MAX_ESP_NOW_PACKET_SIZE];
        char data[20] = "";
        int dataLen;
    } message;
    
    message messageData;
    audio* m_audio;
    memory* m_memory;
    uint8_t m_wifi_channel;
    int m_buffer_size;
    int m_index;
    int m_header_size;
    int lastData;
    int buttonValue = 0;
    bool stateBinding = false;
    char dataFromReceiver[12] = "";

public:
    Communication(audio* audio_buffer, memory* memory_storage, uint8_t wifi_channel);
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
    int setHeader(const int header_size, const uint8_t* header);
    
    // Friend fungsi untuk callback
    friend void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
    friend void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
};

// Callback function declarations
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen);