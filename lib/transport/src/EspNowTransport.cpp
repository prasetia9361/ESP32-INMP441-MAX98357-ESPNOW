#include "EspNowTransport.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "OutputBuffer.h"

const int MAX_ESP_NOW_PACKET_SIZE = 250;
const char* message = "hello binding started!";

static EspNowTransport *instance = NULL;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
    #ifdef TRANSMITTER
    // Serial.print("Receiver Address: ");
    instance->spiffs->writeMacAddress(macAddr);
    #else
    bool binding = instance->stateBinding;
    if (binding)
    {  
        instance->spiffs->writeMacAddress(macAddr);
        Serial.println("binding transport true");
        binding = false;
    }else
    {
        int header_size = instance->m_header_size;

        if ((dataLen > header_size) && (dataLen <= MAX_ESP_NOW_PACKET_SIZE) && (memcmp(data, instance->m_buffer, header_size) == 0)) {
            instance->m_output_buffer->add_samples(data + header_size, dataLen - header_size);
        }
    }
    #endif

}

EspNowTransport::EspNowTransport(OutputBuffer *output_buffer,spiffsHandler *_spiffs, uint8_t wifi_channel): Transport(output_buffer, MAX_ESP_NOW_PACKET_SIZE) {
    instance = this;
    spiffs = _spiffs;
    m_wifi_channel = wifi_channel;
}

bool EspNowTransport::begin() {
    if (spiffs == nullptr) {
        Serial.println("Error: spiffs tidak terinisialisasi");
        return false;
    }
    
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(m_wifi_channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    esp_err_t result = esp_now_init();
    if (result == ESP_OK) {
        Serial.println("ESPNow Init Success");
        esp_now_register_recv_cb(receiveCallback);
    }else {
        Serial.printf("ESPNow Init failed: %s\n", esp_err_to_name(result));
        return false;
    }
    return true;
}

void EspNowTransport::addPeer(){
    if (spiffs->getMac()[0] == 0)
    {
        return;
    }
    
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, spiffs->getMac(), 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA; 
    
    if (!esp_now_is_peer_exist(spiffs->getMac())) {
        esp_err_t result = esp_now_add_peer(&peerInfo);
        if (result == ESP_OK) {
        Serial.println("Receiver ditambahkan sebagai peer");

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
            spiffs->getMac()[0], 
            spiffs->getMac()[1], 
            spiffs->getMac()[2], 
            spiffs->getMac()[3], 
            spiffs->getMac()[4], 
            spiffs->getMac()[5]);
        Serial.printf("mac address: %s\n", macStr);
        } else {
        Serial.print("Gagal menambahkan Receiver sebagai peer. Error code: ");
        Serial.println(result);
        }
    }
}
void EspNowTransport::send() {
    if (spiffs->getMac()[0] == 0)
    {
        return;
    }
    
    
    esp_err_t send = esp_now_send(spiffs->getMac(), m_buffer, m_index + m_header_size);
    if (send != ESP_OK) {
        Serial.printf("Gagal mengirim: %s\n", esp_err_to_name(send));
    }
}

void EspNowTransport::bindingMode(){
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    for (int i = 0; i < 6; i++) {
        peerInfo.peer_addr[i] = 0xFF;
    }

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        esp_now_send(peerInfo.peer_addr, ( uint8_t *)message, 6);
        esp_now_del_peer(peerInfo.peer_addr);
    }
}
