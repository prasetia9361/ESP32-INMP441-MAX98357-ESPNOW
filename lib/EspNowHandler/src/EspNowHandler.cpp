#include "EspNowHandler.h"
 // Tambahkan ini

EspNowHandler::EspNowHandler() {}

void EspNowHandler::init() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    memcpy(transmitterMAC, WiFi.macAddress().c_str(), 6);
    
}

void EspNowHandler::callBack(){
    // esp_now_register_recv_cb([](const uint8_t* mac, const uint8_t* incomingData, int len) {
    //     EspNowHandler::OnDataRecv(mac, incomingData, len);
        
    // });
    // memset(&receivedData, 0, sizeof(receivedData));  // Mengosongkan receivedData terlebih dahulu
    esp_now_register_recv_cb(OnDataRecv);
}

esp_err_t EspNowHandler::addPeer(const uint8_t* mac){
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA; 
    esp_err_t result;
    if (!esp_now_is_peer_exist(mac)) {
        result = esp_now_add_peer(&peerInfo);
        // return result;
    }
    return result;
}

esp_err_t EspNowHandler::sendingData(const uint8_t* mac, const uint8_t* data, size_t len){
    esp_err_t result = esp_now_send(mac, data, len);
    return result;
}

// void EspNowHandler::transmitter(const uint8_t* mac, const uint8_t* data, size_t len) {
//     memcpy(myData.receiverMAC, mac, 6);
//     memcpy(myData.incomingData, data, len);
//     myData.len = len;
// }

void EspNowHandler::bindingMode() {
    memset(&peerInfo, 0, sizeof(peerInfo));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    for (int i = 0; i < 6; i++) {
        peerInfo.peer_addr[i] = 0xFF;
    }
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        esp_now_send(peerInfo.peer_addr, transmitterMAC, 6);
        esp_now_del_peer(peerInfo.peer_addr);
    }
}


void EspNowHandler::OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    struct_message receivedData;
    memcpy(receivedData.macAddr, mac, 6);
    memcpy(receivedData.incomingData, incomingData, sizeof(incomingData));
    receivedData.size = len;

    // Panggil fungsi write dari spiffs_handler
    // instance->spiffsHandler->write(mac, incomingData);
}