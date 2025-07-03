#include "commEspNow.h"

#ifdef RECEIVER
const char messaging[6] = "sound";
#endif
#ifdef TRANSMITTER
const char messaging[6] = "remot";
#endif
#ifdef DISP
const char messaging[6] = "displ";
#endif

static commEspNow* instance = NULL;

// Callback untuk receiver
void receiverCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) { 
    if (strcmp((char*)data, "remot") == 0 || strcmp((char*)data, "displ") == 0) {
        Serial.println("[DEBUG] Menerima esp32remote/displ, menyimpan MAC address...");
        instance->memoryStorage->writeMacAddress(macAddr,(char*)data, 2);
    } else {
        // Serial.println("[DEBUG] Memproses data normal");
        int headerSize = instance->headerSize;
        memcpy(&instance->messageData, data, sizeof(instance->messageData));

        if (memcmp(macAddr, instance->memoryStorage->getMac(), 6) == 0 || 
            memcmp(macAddr, instance->memoryStorage->getMac1(), 6) == 0) {
            if (instance->messageData.dataLen > headerSize && 
                (instance->messageData.dataLen <= maxEspNowPacketSize) && 
                strlen(instance->messageData.data) == 0) {
            // if (strlen(instance->messageData.data) == 0){
            
                // Audio data
                instance->audioBuffer->addBuffer(instance->messageData.buffer + headerSize, 
                                           instance->messageData.dataLen - headerSize);
            } else {
                Serial.println("[DEBUG] Memproses button data");
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, instance->messageData.data);
                if (!error) {
                    const char *header = jsonDoc["h"];
                    if (strcmp(header, "displ") == 0) {
                        Serial.print("[DEBUG] Volume speaker: ");
                        int vol = jsonDoc["v"];
                        instance->memoryStorage->saveVolume(vol);
                        Serial.println(vol);
                    } else if (strcmp(header, "remot") == 0) {
                        Serial.print("[DEBUG] Button value: ");
                        instance->buttonValue = jsonDoc["d"];
                        Serial.println(instance->buttonValue);
                    }
                } else {
                    Serial.println("[DEBUG] Error parsing JSON");
                }
            }
        } else {
            Serial.println("[DEBUG] MAC address tidak cocok dengan yang tersimpan di storage");
        }
    }
}

// Callback untuk transmitter
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    Serial.println("[DEBUG] Transmitter callback dipanggil");
    Serial.print("[DEBUG] Data diterima dari MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", macAddr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    if (strcmp((char*)data, "sound") == 0 || strcmp((char*)data, "displ") == 0) {
        Serial.println("[DEBUG] Menerima sound/displ, menyimpan MAC address...");
        instance->memoryStorage->writeMacAddress(macAddr,(char*)data, 2);
    } else {
        Serial.println("[DEBUG] Memproses data normal");
        int headerSize = instance->headerSize;
        memcpy(&instance->messageData, data, sizeof(instance->messageData));
        
        if (memcmp(macAddr, instance->memoryStorage->getMac(), 6) == 0 || 
            memcmp(macAddr, instance->memoryStorage->getMac1(), 6) == 0) {
            Serial.println("[DEBUG] MAC address cocok");
            
            if (strlen(instance->messageData.data) == 0) {
                Serial.println("[DEBUG] Memproses tone selected data");
                Serial.print("[DEBUG] Nilai toneSelected: ");
                for (int i = 0; i < 8; i++) {
                    Serial.print(instance->messageData.buffer[i]);
                    if (i < 7) Serial.print(", ");
                }
                Serial.println();
                instance->memoryStorage->writeMode(instance->messageData.buffer, 8);
            } else {
                Serial.println("[DEBUG] Memproses JSON data");
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, instance->messageData.data);
                if (!error) {
                    const char *header = jsonDoc["h"];
                    if (strcmp(header, "displ") == 0) {
                        Serial.print("[DEBUG] Volume speaker: ");
                        int volume = jsonDoc["v"];
                        Serial.println(volume);
                    }
                } else {
                    Serial.println("[DEBUG] Error parsing JSON");
                }
            }
        } else {
            Serial.println("[DEBUG] MAC address tidak cocok dengan yang tersimpan di storage");
        }
    }
}

void displayCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    Serial.println("[DEBUG] Display callback dipanggil");
    Serial.print("[DEBUG] Data diterima dari MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", macAddr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    if (strcmp((char*)data, "sound") == 0 || strcmp((char*)data, "remot") == 0) {
        Serial.println("[DEBUG] Menerima esp32-sound/esp32remote, menyimpan MAC address...");
        instance->memoryStorage->writeMacAddress(macAddr,(char*)data, 2);
    }
}

commEspNow::~commEspNow() {
}

commEspNow::commEspNow(Buffer* audioBuffer, storage* memoryStorage, uint8_t wifiChannel) {
    Serial.println("[DEBUG] Inisialisasi commEspNow");
    instance = this;
    this->audioBuffer = audioBuffer;
    this->memoryStorage = memoryStorage;
    this->wifiChannel = wifiChannel;
    this->bufferSize = maxEspNowPacketSize;
    this->index = 0;
    this->headerSize = 0;
    this->lastData = 0;
    this->mac = nullptr;
}

bool commEspNow::begin() {
    Serial.println("[DEBUG] Memulai inisialisasi ESP-NOW");
    
    if (memoryStorage == nullptr) {
        Serial.println("[ERROR] memoryStorage tidak terinisialisasi");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("[DEBUG] MAC Address: ");
    Serial.println(WiFi.macAddress());

    Serial.println("[DEBUG] Setup WiFi channel");
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    Serial.println("[DEBUG] Inisialisasi ESP-NOW");
    esp_err_t result = esp_now_init();
    if (result == ESP_OK) {
        #ifdef RECEIVER
            Serial.println("[DEBUG] ESPNow Init in Receiver Success");
            esp_now_register_recv_cb(receiverCallback);
        #endif
        
        #ifdef TRANSMITTER
            Serial.println("[DEBUG] ESPNow Init in Transmitter Success");
            esp_now_register_recv_cb(transmitterCallback);
        #endif
        
        #ifdef DISP
            Serial.println("[DEBUG] ESPNow Init in Display Success");
            esp_now_register_recv_cb(displayCallback);
        #endif
    } else {
        Serial.printf("[ERROR] ESPNow Init failed: %s\n", esp_err_to_name(result));
        return false;
    }

    Serial.println("[DEBUG] Inisialisasi ESP-NOW berhasil");
    return true;
}

void commEspNow::addPeer() {
    
    if (strcmp(memoryStorage->device1(), "sound") == 0) {
        mac = memoryStorage->getMac();
    } else {
        mac = memoryStorage->getMac1();
    }

    if (!mac || mac[0] == 0) {
        delay(500);
        Serial.print(".");
        return;
    }

    if (!esp_now_is_peer_exist(mac)) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;          // Gunakan channel 0 untuk auto-select
        peerInfo.encrypt = false;      // Non-aktifkan enkripsi (ubah jika diperlukan)
        peerInfo.ifidx = WIFI_IF_STA;  // Gunakan interface Station

        esp_err_t result = esp_now_add_peer(&peerInfo);
        
        if (result == ESP_OK) {
            Serial.printf("Peer added successfully with MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        } else {
            Serial.printf("[ERROR] Failed to add peer: %s\n", esp_err_to_name(result));
        }
    }
}

void commEspNow::addPeer(const uint8_t* _mac){
    mac = _mac;
    if (mac == nullptr || mac[0] == 0) {
        Serial.println("MAC address null, tidak bisa menambah peer.");
        return;
    }

    if (!esp_now_is_peer_exist(mac)) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        peerInfo.ifidx = WIFI_IF_STA;
        esp_err_t res = esp_now_add_peer(&peerInfo);
        if (res == ESP_OK) {
            Serial.println("Peer berhasil ditambahkan.");
        } else {
            Serial.print("Gagal menambah peer: ");
            Serial.println(esp_err_to_name(res));
        }
    }
}

void commEspNow::sendData() {
    
    // if (mac == nullptr || mac[0] == 0) {
    //     Serial.println("MAC address null, tidak bisa menambah peer.");
    //     return;
    // }

    esp_err_t result = esp_now_send(mac, (uint8_t*)&messageData, sizeof(messageData));
    if (result != ESP_OK) {
        Serial.printf("Send Error: %s\n", esp_err_to_name(result));
        
        // Handle error spesifik
        if (result == ESP_ERR_ESPNOW_NO_MEM) {
            vTaskDelay(pdMS_TO_TICKS(10)); // Beri waktu pemrosesan
        }
    }
}

void commEspNow::addSample(int16_t sample) {
    
    // if (mac == nullptr || mac[0] == 0) {
    //     Serial.println("MAC address null, tidak bisa menambah peer.");
    //     return;
    // }
    
    // if (!esp_now_is_peer_exist(mac)) {
    //     Serial.println("Peer not registered, adding...");
    //     addPeer(mac);
    // }
    // messageData.buffer[index + headerSize] = (sample + 2048) * 256 / 4096;
    // messageData.buffer[index + headerSize] = (sample + 512) * 256 / 1024;
    messageData.buffer[index + headerSize] = (sample + 32768) >> 8;
    index++;

    if ((index + headerSize) == bufferSize) {
        messageData.dataLen = index + headerSize;
        // memset(messageData.data, 0, sizeof(messageData.data));
        sendData();
        index = 0;
    }
}

void commEspNow::sendButton(int data) {
    if (data != lastData) {
        JsonDocument doc;
        doc["h"] = "remot";
        doc["d"] = data;
        serializeJson(doc, messageData.data);
        sendData();
        // messageData.data[0] = '\0'; // Reset data
        memset(messageData.data, 0, sizeof(messageData.data));
        Serial.print("sending data mode ");
        Serial.println(data);
        lastData = data;
    }
}

void commEspNow::sendVolume(int vol) {
    JsonDocument doc;
    doc["h"] = "displ";
    doc["v"] = vol;
    serializeJson(doc, messageData.data);
    sendData();
    memset(messageData.data, 0, sizeof(messageData.data));
}

void commEspNow::sendSirineSetting(const uint8_t *modelBuffer){
    memcpy(messageData.buffer, modelBuffer, sizeof(messageData.buffer));
    memset(messageData.data, 0, sizeof(messageData.data));
    messageData.dataLen = sizeof(modelBuffer);
    sendData();
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

bool commEspNow::binding() {
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
        return true;
    }
    return false;
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