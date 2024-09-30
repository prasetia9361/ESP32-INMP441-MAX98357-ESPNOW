#include "EspNowHandler.h"
 // Tambahkan ini


EspNowHandler *EspNowHandler::instance = NULL;

EspNowHandler::EspNowHandler() {
    instance = this;
}

void EspNowHandler::OnDataRecvWrapper(const uint8_t *mac, const uint8_t* incomingData, int len) {
    if (instance) {
        instance->OnDataRecv(mac, incomingData, len);
    }
}

void EspNowHandler::OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    // Serial.print("Data diterima dari: ");
    // char macStr[18];
    // snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    // Serial.println(macStr);
    if (mac == nullptr || incomingData == nullptr) {
        Serial.println("Error: Null pointer atau MAC address tidak valid diterima di OnDataRecv");
        return;
    }
// memcmp(reciveMac, "\0\0\0\0\0\0", 6) == 0
    // Alokasikan memori untuk receivedData.macAddr dan receivedData.incomingData
    receivedData.macAddr = new uint8_t[6];
    receivedData.incomingData = new uint8_t[len];
    
    // Salin nilai dari mac ke receivedData.macAddr
    memcpy(receivedData.macAddr, mac, 6);
    
    // Salin nilai dari incomingData ke receivedData.incomingData
    memcpy(receivedData.incomingData, incomingData, len);
    
    // Simpan panjang data
    receivedData.size = len;
    
    // Serial.println("Data berhasil disimpan ke dalam receivedData");

}

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
    memset(&receivedData, 0, sizeof(receivedData));  // Mengosongkan receivedData terlebih dahulu
    esp_now_register_recv_cb(OnDataRecvWrapper);
}

esp_err_t EspNowHandler::addPeer(const uint8_t* mac) {
    if (esp_now_is_peer_exist(mac)) {
        return ESP_OK;  // Peer sudah ada, tidak perlu menambahkan lagi
    }

    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    memcpy(peerInfo.peer_addr, mac, 6);

    return esp_now_add_peer(&peerInfo);
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

// Static callback wrapper calls the instance method





