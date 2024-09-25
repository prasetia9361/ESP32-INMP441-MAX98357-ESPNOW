#ifndef ESPNOWHANDLER_H
#define ESPNOWHANDLER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>


class EspNowHandler {

private:
   
    
    typedef struct struct_message {
        uint8_t macAddr[6];
        uint8_t incomingData[250]; 
        int size;
    } struct_message;
    struct_message receivedData;

    typedef struct fromSpiffs {
        uint8_t receiverMAC[6];
        uint8_t incomingData[250]; 
        size_t len;
    } fromSpiffs;
    fromSpiffs myData;

    // void receiver();
    static void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
    
    uint8_t transmitterMAC[6];
    bool bindingModeActive;
    esp_now_peer_info_t peerInfo;
    


public:
    EspNowHandler();
    void init();
    void callBack();
    // void transmitter(const uint8_t* mac, const uint8_t* data, size_t len);
    esp_err_t addPeer(const uint8_t* mac);
    esp_err_t sendingData(const uint8_t* mac, const uint8_t* data, size_t len);
    void bindingMode();
    // Fungsi untuk mengembalikan 3 variabel dari struct_message
    uint8_t* getMacAddr() { return receivedData.macAddr; }
    uint8_t* getIncomingData() { return receivedData.incomingData; }
    int getSize() { return receivedData.size; }

 

    
};

#endif