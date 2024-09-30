#ifndef ESPNOWHANDLER_H
#define ESPNOWHANDLER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>


class EspNowHandler {

private:
   
    
    typedef struct struct_message {
        uint8_t* macAddr;
        uint8_t* incomingData; 
        int size;
    } struct_message;
    struct_message receivedData;

    typedef struct fromSpiffs {
        uint8_t receiverMAC;
        uint8_t incomingData; 
        size_t len;
    } fromSpiffs;
    fromSpiffs myData;

    // void receiver();
    // 
    
    uint8_t transmitterMAC[6];
    bool bindingModeActive;
    esp_now_peer_info_t peerInfo;

    static void OnDataRecvWrapper(const uint8_t* mac, const uint8_t* incomingData, int len);
    static EspNowHandler* instance;
    


public:
    EspNowHandler();
    void init();
    void callBack() ;
    void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
    esp_err_t addPeer(const uint8_t* mac);
    esp_err_t sendingData(const uint8_t* mac, const uint8_t* data, size_t len);
    void bindingMode();
    // Fungsi untuk mengembalikan 3 variabel dari struct_message
    uint8_t* getMacAddr() { return receivedData.macAddr; }
    uint8_t* getIncomingData() { return receivedData.incomingData; }
    int getSize() { return receivedData.size; }

 

    
};

#endif