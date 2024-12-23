#include "EspNowTransport.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "OutputBuffer.h"

const int MAX_ESP_NOW_PACKET_SIZE = 127;
#ifdef RECEIVER
const char messaging[12] = "bindingMode";
#else
const char messaging[12] = "bindingSend";
#endif

static EspNowTransport *instance = NULL;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen) {
#ifdef RECEIVER
    message messageReceiver;
    // bool binding = instance->stateBinding;
    if (instance->stateBinding) {
        // char dataStr[12];
        // memcpy(dataStr, data, dataLen);
        // dataStr[dataLen] = '\0';
        if (strcmp((char *)data, "bindingSend") == 0) {
            instance->m_memory->writeMacAddress(macAddr, 2);
            instance->stateBinding = false;
        }
        // instance->m_memory->writeMacAddress(macAddr, 2);
        // Serial.println("binding transport true");
        
    } else {

        
        int header_size = instance->m_header_size;
        const char* sender = jsonDoc["s"];
        const char* messageButton = jsonDoc["b"];
        int len = jsonDoc["d"];
        JsonArray audio = jsonDoc["a"].as<JsonArray>();
        for (size_t i = 0; i < (len - header_size); i++)
        {
            instance->bufferAudio[i] = audio[i].as<uint8_t>();
        }
        
        
        if (memcmp(macAddr, instance->m_memory->getMac(), 6) == 0) {
            memcpy(&messageReceiver, data, sizeof(messageReceiver));
            // Serial.println(messageReceiver.data);
            


                // const char* messageButton = jsonDoc["data"];
            if (messageReceiver.data == "" || strlen(messageReceiver.data) == 0) {
                instance->m_output_buffer->add_samples(messageReceiver.m_buffer + header_size, messageReceiver.dataLen - header_size);
            } else {
                // messageReceiver.data[12] = '\0';
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, messageReceiver.data);
                if (error)
                {
                    Serial.println("deserialization failed!");
                    Serial.println(error.c_str());
                    return;
                }
                const char* messageButton = jsonDoc["d"];
                Serial.print("data: ");
                Serial.println(messageButton);
            }
            
        }
        
        if (memcmp(macAddr, instance->m_memory->getMac1(), 6) == 0){
            memcpy(&messageReceiver, data, sizeof(messageReceiver));
            // Serial.println(messageReceiver.data);
            if (messageReceiver.data == "" || strlen(messageReceiver.data) == 0) {
                instance->m_output_buffer->add_samples(messageReceiver.m_buffer + header_size, messageReceiver.dataLen - header_size);
            } else {
                // messageReceiver.data[12] = '\0';
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, messageReceiver.data);
                if (error)
                {
                    Serial.println("deserialization failed!");
                    Serial.println(error.c_str());
                    return;
                }
                const char* messageButton = jsonDoc["d"];
                Serial.print("data: ");
                Serial.println(messageButton);
            }
        }
    }
#else
    // Serial.print("Receiver Address: ");
    // char dataStr[MAX_ESP_NOW_PACKET_SIZE];
    // memcpy(dataStr, data, dataLen);
    // dataStr[dataLen] = '\0';
    if (strcmp((char *)data, "bindingMode") == 0) {
        instance->m_memory->writeMacAddress(macAddr, 1);
    }
#endif
}

EspNowTransport::EspNowTransport(OutputBuffer *output_buffer, memory *_memory, uint8_t wifi_channel) : Transport(output_buffer, MAX_ESP_NOW_PACKET_SIZE) {
    instance = this;
    m_memory = _memory;
    m_wifi_channel = wifi_channel;
}

bool EspNowTransport::begin() {
    if (m_memory == nullptr) {
        Serial.println("Error: m_memory tidak terinisialisasi");
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
    if (m_memory->getMac()[0] == 0) {
        // Serial.print("MAC Address0: ");
        // for(int i=0; i<6; i++) {
        //     Serial.print(instance->m_memory->getMac()[i], HEX);
        //     if(i<5) Serial.print(":");
        // }
        // Serial.println();
        return;
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, m_memory->getMac(), 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    if (!esp_now_is_peer_exist(m_memory->getMac())) {
        esp_err_t result = esp_now_add_peer(&peerInfo);
    }
}

void EspNowTransport::send() {
    JsonDocument doc;
    if (m_memory->getMac()[0] == 0) {
        return;
    }

    doc["s"] = "esp32";
    doc["b"] = "";
    doc["d"] = m_index + m_header_size;
    JsonArray audio = doc["a"].to<JsonArray>();
    for (int i = m_header_size; i < m_buffer_size; i++)
    {
        // Serial.println(i);
        audio.add(bufferValue[i]);
    }
    
    
    serializeJson(doc, jsonData);
    // messageData.dataLen = m_index + m_header_size;

    // esp_err_t send = esp_now_send(m_memory->getMac(), (uint8_t *)&messageData, sizeof(messageData));
    // esp_err_t send = esp_now_send(m_memory->getMac(), (const uint8_t *)_jsonData.c_str(), _jsonData.length());
    esp_err_t send = esp_now_send(m_memory->getMac(), (const uint8_t *)jsonData, sizeof(jsonData));
    
    // Serial.print("Ukuran char jsonData yang terpakai: ");
    // Serial.println(strlen(jsonData));
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
        Serial.println(messaging);
        esp_now_send(peerInfo.peer_addr, (uint8_t *)messaging, 12);
        esp_now_del_peer(peerInfo.peer_addr);
    }
}
