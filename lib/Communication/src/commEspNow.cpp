#include "commEspNow.h"

#ifdef RECEIVER
const char messaging[12] = "bindingMode";
#else
const char messaging[12] = "bindingSend";
#endif

static commEspNow* instance = NULL;

// Callback untuk receiver
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    if (instance->stateBinding) {
        if (strcmp((char*)data, "bindingSend") == 0) {
            instance->memoryStorage->writeMacAddress(macAddr, 2);
            instance->stateBinding = false;
        }
    } else {
        int headerSize = instance->headerSize;
        memcpy(&instance->messageData, data, sizeof(instance->messageData));
        
        if (memcmp(macAddr, instance->memoryStorage->getMac(), 6) == 0 || 
            memcmp(macAddr, instance->memoryStorage->getMac1(), 6) == 0) {
            
            if (instance->messageData.dataLen > instance->headerSize && 
                (instance->messageData.dataLen <= maxEspNowPacketSize) && 
                strlen(instance->messageData.data) == 0) {
                // Audio data
                instance->audioBuffer->addBuffer(instance->messageData.buffer + headerSize, 
                                           instance->messageData.dataLen - headerSize);
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
        instance->memoryStorage->writeMacAddress(macAddr, 1);
        memcpy(instance->dataFromReceiver, data, dataLen);
    }
}

commEspNow::commEspNow(Buffer* audioBuffer, storage* memoryStorage, uint8_t wifiChannel) {
    instance = this;
    this->audioBuffer = audioBuffer;
    this->memoryStorage = memoryStorage;
    this->wifiChannel = wifiChannel;
    this->bufferSize = maxEspNowPacketSize;
    this->index = 0;
    this->headerSize = 0;
    this->lastData = 0;
}

bool commEspNow::begin() {
    if (memoryStorage == nullptr) {
        Serial.println("Error: memoryStorage tidak terinisialisasi");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    // Setup WiFi channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    // Initialize ESP-NOW
    esp_err_t result = esp_now_init();
    if (result == ESP_OK) {
        #ifdef RECEIVER
            // m_audio->startBuffering(300 * 16);
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

void commEspNow::addPeer() {
    if (memoryStorage->getMac()[0] == 0) {
        return;
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, memoryStorage->getMac(), 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    if (!esp_now_is_peer_exist(memoryStorage->getMac())) {
        esp_now_add_peer(&peerInfo);
    }
}

void commEspNow::sendData() {
    if (memoryStorage->getMac()[0] == 0) {
        return;
    }

    messageData.dataLen = index + headerSize;
    esp_now_send(memoryStorage->getMac(), (uint8_t*)&messageData, sizeof(messageData));
}

void commEspNow::addSample(int16_t sample) {
    messageData.buffer[index + headerSize] = (sample + 2048) * 256 / 4096;
    index++;

    if ((index + headerSize) == bufferSize) {
        sendData();
        index = 0;
    }
}

void commEspNow::sendButton(int data) {
    if (data != lastData) {
        JsonDocument doc;
        doc["d"] = data;
        serializeJson(doc, messageData.data);
        sendData();
        messageData.data[0] = '\0'; // Reset data
        lastData = data;
    }
}

void commEspNow::flush() {
    if (index > 0) {
        sendData();
        index = 0;
    }
}

void commEspNow::statusBinding() {
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

int commEspNow::setHeader(const int headerSize, const uint8_t* header) {
    if ((headerSize < bufferSize) && (header)) {
        this->headerSize = headerSize;
        memcpy(messageData.buffer, header, headerSize);
        return 0;
    } else {
        return -1;
    }
}

bool commEspNow::setBinding(bool bindingState) {
    stateBinding = bindingState;
    return stateBinding;
}