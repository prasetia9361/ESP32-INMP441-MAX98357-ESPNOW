#include "EspNowTransport.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "OutputBuffer.h"

const int MAX_ESP_NOW_PACKET_SIZE = 127;

const char *messaging = "hello binding started!";

static EspNowTransport *instance = NULL;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
#ifdef RECEIVER
    message messageReceiver;
    // bool binding = instance->stateBinding;
    if (instance->stateBinding) {
        instance->spiffs->writeMacAddress(macAddr, 2);
        // Serial.println("binding transport true");
        instance->stateBinding = false;
    } else {
        memcpy(&messageReceiver, data, sizeof(messageReceiver));
        int header_size = instance->m_header_size;
        if (memcmp(macAddr, instance->spiffs->getMac(), 6) == 0) {
            messageReceiver.data[12] = '\0';
            Serial.print("data from: ");
            Serial.println(messageReceiver.data);
            
        }else if (memcmp(macAddr, instance->spiffs->getMac1(), 6) == 0)
        {
            
            if (messageReceiver.dataLen > header_size && messageReceiver.dataLen <= MAX_ESP_NOW_PACKET_SIZE /*&& (memcmp(messageReceiver.m_buffer, instance->bufferValue, header_size) == 0)*/) {
                instance->m_output_buffer->add_samples(messageReceiver.m_buffer + header_size, messageReceiver.dataLen - header_size);
            } else {
                Serial.println("Ukuran buffer atau pointer tidak valid.");
            }
        }
        
    }
#else
    // Serial.print("Receiver Address: ");
    instance->spiffs->writeMacAddress(macAddr, 1);
#endif
}

EspNowTransport::EspNowTransport(OutputBuffer *output_buffer, spiffsHandler *_spiffs, uint8_t wifi_channel) : Transport(output_buffer, MAX_ESP_NOW_PACKET_SIZE) {
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
    } else {
        Serial.printf("ESPNow Init failed: %s\n", esp_err_to_name(result));
        return false;
    }

    return true;
}

void EspNowTransport::addPeer() {
    if (spiffs->getMac()[0] == 0) {
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
    }
}

void EspNowTransport::send() {
    if (spiffs->getMac()[0] == 0) {
        return;
    }

    messageData.dataLen = m_index + m_header_size;

    esp_err_t send = esp_now_send(spiffs->getMac(), (uint8_t *)&messageData, sizeof(messageData));
}

void EspNowTransport::bindingMode() {
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    for (int i = 0; i < 6; i++) {
        peerInfo.peer_addr[i] = 0xFF;
    }

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        esp_now_send(peerInfo.peer_addr, (uint8_t *)messaging, 6);
        esp_now_del_peer(peerInfo.peer_addr);
    }
}
