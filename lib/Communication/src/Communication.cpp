#include "Communication.h"

#ifdef RECEIVER
const char messaging[12] = "bindingMode";
#else
const char messaging[12] = "bindingSend";
#endif

static Communication* instance = NULL;

// Callback untuk receiver
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    if (instance->stateBinding) {
        if (strcmp((char*)data, "bindingSend") == 0) {
            instance->m_memory->writeMacAddress(macAddr, 2);
            instance->stateBinding = false;
        }
    } else {
        int header_size = instance->m_header_size;
        memcpy(&instance->messageData, data, sizeof(instance->messageData));
        
        if (memcmp(macAddr, instance->m_memory->getMac(), 6) == 0 || 
            memcmp(macAddr, instance->m_memory->getMac1(), 6) == 0) {
            
            if (instance->messageData.dataLen > instance->m_header_size && 
                (instance->messageData.dataLen <= MAX_ESP_NOW_PACKET_SIZE) && 
                strlen(instance->messageData.data) == 0) {
                // Audio data
                instance->m_audio->addBuffer(instance->messageData.buffer + header_size, 
                                           instance->messageData.dataLen - header_size);
            } else {
                // Button data
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, instance->messageData.data);
                if (!error) {
                    instance->buttonValue = jsonDoc["d"];
                    Serial.println(instance->buttonValue);
                }
            }
        }
    }
}

// Callback untuk transmitter
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    if (strcmp((char*)data, "bindingMode") == 0) {
        instance->m_memory->writeMacAddress(macAddr, 1);
        memcpy(instance->dataFromReceiver, data, dataLen);
    }
}

Communication::Communication(audio* audio_buffer, memory* memory_storage, uint8_t wifi_channel) {
    instance = this;
    m_audio = audio_buffer;
    m_memory = memory_storage;
    m_wifi_channel = wifi_channel;
    m_buffer_size = MAX_ESP_NOW_PACKET_SIZE;
    m_index = 0;
    m_header_size = 0;
    lastData = 0;
}

bool Communication::begin() {
    if (m_memory == nullptr) {
        Serial.println("Error: m_memory tidak terinisialisasi");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    // Setup WiFi channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(m_wifi_channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    // Initialize ESP-NOW
    esp_err_t result = esp_now_init();
    if (result == ESP_OK) {
        #ifdef RECEIVER
            m_audio->startBuffering(300 * 16);
            Serial.println("ESPNow Init in Receiver Success");
            esp_now_register_recv_cb(receiverCallback);
        #endif
        
        #ifdef TRANSMITTER
            Serial.println("ESPNow Init in Transmitter Success");
            esp_now_register_recv_cb(transmitterCallback);
        #endif
        
        #ifdef DISP
            Serial.println("ESPNow Init in Display Success");
            esp_now_register_recv_cb(transmitterCallback);
        #endif
    } else {
        Serial.printf("ESPNow Init failed: %s\n", esp_err_to_name(result));
        return false;
    }

    return true;
}

void Communication::addPeer() {
    if (m_memory->getMac()[0] == 0) {
        return;
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, m_memory->getMac(), 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    if (!esp_now_is_peer_exist(m_memory->getMac())) {
        esp_now_add_peer(&peerInfo);
    }
}

void Communication::sendData() {
    if (m_memory->getMac()[0] == 0) {
        return;
    }

    messageData.dataLen = m_index + m_header_size;
    esp_now_send(m_memory->getMac(), (uint8_t*)&messageData, sizeof(messageData));
}

// void Communication::bindingMode() {
//     esp_now_peer_info_t peerInfo;
//     memset(&peerInfo, 0, sizeof(peerInfo));
//     peerInfo.channel = 0;
//     peerInfo.encrypt = false;
//     peerInfo.ifidx = WIFI_IF_STA;

//     // Set broadcast address
//     for (int i = 0; i < 6; i++) {
//         peerInfo.peer_addr[i] = 0xFF;
//     }

//     if (esp_now_add_peer(&peerInfo) == ESP_OK) {
//         Serial.println(messaging);
//         esp_now_send(peerInfo.peer_addr, (uint8_t*)messaging, 12);
//         esp_now_del_peer(peerInfo.peer_addr);
//     }
// }

void Communication::addSample(int16_t sample) {
    messageData.buffer[m_index + m_header_size] = (sample + 2048) * 256 / 4096;
    m_index++;

    if ((m_index + m_header_size) == m_buffer_size) {
        sendData();
        m_index = 0;
    }
}

void Communication::sendButton(int data) {
    if (data != lastData) {
        JsonDocument doc;
        doc["d"] = data;
        serializeJson(doc, messageData.data);
        sendData();
        messageData.data[0] = '\0'; // Reset data
        lastData = data;
    }
}

void Communication::flush() {
    if (m_index > 0) {
        sendData();
        m_index = 0;
    }
}

void Communication::statusBinding() {
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    // Set broadcast address
    for (int i = 0; i < 6; i++) {
        peerInfo.peer_addr[i] = 0xFF;
    }

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        Serial.println(messaging);
        esp_now_send(peerInfo.peer_addr, (uint8_t*)messaging, 12);
        esp_now_del_peer(peerInfo.peer_addr);
    }
}

int Communication::setHeader(const int header_size, const uint8_t* header) {
    if ((header_size < m_buffer_size) && (header)) {
        m_header_size = header_size;
        memcpy(messageData.buffer, header, header_size);
        return 0;
    } else {
        return -1;
    }
}

bool Communication::setBinding(bool bindingState) {
    stateBinding = bindingState;
    return stateBinding;
}