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
    // Pengecekan null pointer untuk mencegah crash
    if (!instance || !macAddr || !data) {
        Serial.println("[ERROR] Null pointer in receiverCallback");
        return;
    }
    
    if (dataLen <= 0) {
        Serial.println("[ERROR] Invalid data length in receiverCallback");
        return;
    }
    
    if (strcmp((char*)data, "remot") == 0 || strcmp((char*)data, "displ") == 0) {
        Serial.println("[DEBUG] Menerima esp32remote/displ, menyimpan MAC address...");
        if (instance->memoryStorage) {
            instance->memoryStorage->writeMacAddress(macAddr,(char*)data, 2);
        }
    } else {
        // Serial.println("[DEBUG] Memproses data normal");
        if (!instance->memoryStorage) {
            Serial.println("[ERROR] memoryStorage is null");
            return;
        }
        
        int headerSize = instance->headerSize;
        if (dataLen >= sizeof(instance->messageData)) {
            memcpy(&instance->messageData, data, sizeof(instance->messageData));
        } else {
            Serial.println("[ERROR] Data length too small for messageData");
            return;
        }

        if (memcmp(macAddr, instance->memoryStorage->getMac(), 6) == 0 || 
            memcmp(macAddr, instance->memoryStorage->getMac1(), 6) == 0) {
            if (sizeof(instance->messageData.buffer) > headerSize && (sizeof(instance->messageData.buffer) <= maxEspNowPacketSize) && strlen(instance->messageData.data) == 0) {
                if (instance->messageData.dataLen == 0)
                {
                    if (instance->audioBuffer) {
                        instance->audioBuffer->addBuffer(instance->messageData.buffer + headerSize, sizeof(instance->messageData.buffer) - headerSize);
                    }
                }else
                {// masih sampe disini
                    for (int i = 0; i < 9; i++) {
                        Serial.print(instance->messageData.buffer[i]);
                        if (i < 8) Serial.print(", ");
                    }
                    Serial.println();
                    instance->memoryStorage->writeMode(instance->messageData.buffer, 9);
                }
                // Audio data
            } else {
                char jsonBuffer[256];
                memcpy(jsonBuffer, instance->messageData.data, sizeof(jsonBuffer) - 1);
                jsonBuffer[sizeof(jsonBuffer) - 1] = '\0';
                JsonDocument jsonDoc;
                DeserializationError error = deserializeJson(jsonDoc, jsonBuffer);
                if (!error) {
                    const char *header = jsonDoc["h"];
                    if (strcmp(header, "vol") == 0) {
                        Serial.print("[DEBUG] Volume speaker: ");
                        int vol = jsonDoc["d"];
                        instance->memoryStorage->saveVolume(vol);
                        Serial.println(vol);
                    } else if (strcmp(header, "test") == 0) {
                        Serial.print("[DEBUG] Button value: ");
                        instance->buttonValue = jsonDoc["d"];
                        Serial.println(instance->buttonValue);
                    }else if (strcmp(header, "remot") == 0)
                    {
                        Serial.print("[DEBUG] Button value: ");
                        int value = jsonDoc["d"];
                        int32_t* modeTones = instance->memoryStorage->readModeTones();
                        instance->buttonValue = modeTones[value];
                        Serial.println(instance->buttonValue);
                    }
                } else {
                    Serial.println("[DEBUG] Error parsing JSON");
                    Serial.print("[DEBUG] Error parsing JSON: ");
                    Serial.println(error.c_str());
                }
            }
        } else {
            Serial.println("[DEBUG] MAC address tidak cocok dengan yang tersimpan di storage");
        }
    }
}

// Callback untuk transmitter
void transmitterCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    // Pengecekan null pointer untuk mencegah crash
    if (!instance || !macAddr || !data) {
        Serial.println("[ERROR] Null pointer in transmitterCallback");
        return;
    }
    
    if (dataLen <= 0) {
        Serial.println("[ERROR] Invalid data length in transmitterCallback");
        return;
    }
    
    // Serial.println("[DEBUG] Transmitter callback dipanggil");
    // Serial.print("[DEBUG] Data diterima dari MAC: ");
    // for (int i = 0; i < 6; i++) {
    //     Serial.printf("%02X", macAddr[i]);
    //     if (i < 5) Serial.print(":");
    // }
    // Serial.println();

    if (strcmp((char*)data, "sound") == 0) {
        Serial.println("[DEBUG] Menerima sound, menyimpan MAC address...");
        if (instance->memoryStorage) {
            instance->memoryStorage->writeMacAddress(macAddr,(char*)data, 2);
        }
    }
}

void displayCallback(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    // Serial.println("[DEBUG] Display callback dipanggil");
    // Serial.print("[DEBUG] Data diterima dari MAC: ");
    // for (int i = 0; i < 6; i++) {
    //     Serial.printf("%02X", macAddr[i]);
    //     if (i < 5) Serial.print(":");
    // }
    // Serial.println();

    if (strcmp((char*)data, "sound") == 0) {
        Serial.println("[DEBUG] Menerima sound/, menyimpan MAC address...");
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

    // Delay untuk memastikan sistem stabil
    delay(100);
    
    WiFi.mode(WIFI_STA);
    delay(50);
    WiFi.disconnect();
    delay(100);
    
    Serial.print("[DEBUG] MAC Address: ");
    Serial.println(WiFi.macAddress());

    Serial.println("[DEBUG] Setup WiFi channel");
    esp_wifi_set_promiscuous(true);
    delay(10);
    esp_wifi_set_channel(wifiChannel, WIFI_SECOND_CHAN_NONE);
    delay(10);
    esp_wifi_set_promiscuous(false);
    delay(50);

    Serial.println("[DEBUG] Inisialisasi ESP-NOW");
    esp_err_t result = esp_now_init();
    if (result == ESP_OK) {
        delay(50); // Delay setelah ESP-NOW init
        
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
        
        delay(50); // Delay setelah register callback
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
    } else if(strcmp(memoryStorage->device2(), "sound") == 0){
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
        // messageData.dataLen = index + headerSize;
        messageData.dataLen = 0;
        // memset(messageData.data, 0, sizeof(messageData.data));
        sendData();
        index = 0;
    }
}
// bakal ganti ke sendDataInt
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
// bakal ganti ke sendDataInt
void commEspNow::sendVolume(int vol) {
    JsonDocument doc;
    doc["h"] = "vol";
    doc["d"] = vol;
    serializeJson(doc, messageData.data);
    sendData();
    memset(messageData.data, 0, sizeof(messageData.data));
}

void commEspNow::sendDataInt(int data, const char *header) {
    bool isVolHeader = (strcmp(header, "vol") == 0);
    if (!isVolHeader && data == lastData) {
        return;
    }

    Serial.print("[DEBUG] Mengirim data integer. Header: ");
    Serial.print(header);
    Serial.print(", Data: ");
    Serial.println(data);

    JsonDocument doc;
    doc["h"] = header;
    doc["d"] = data;

    serializeJson(doc, messageData.data);

    sendData();
    memset(messageData.data, 0, sizeof(messageData.data));
    if (!isVolHeader) {
        lastData = data;
        Serial.print("[DEBUG] lastData diupdate menjadi: ");
        Serial.println(lastData);
    }
}

void commEspNow::sendSirineSetting(const uint8_t *modelBuffer){
    memcpy(messageData.buffer, modelBuffer, sizeof(messageData.buffer));
    memset(messageData.data, 0, sizeof(messageData.data));
    messageData.dataLen = sizeof(modelBuffer);
    sendData();
}

void commEspNow::sendMode(const int32_t *modelBuffer, int count){
    Serial.println("[DEBUG] Memulai fungsi sendMode");
    JsonDocument doc;
    doc["h"] = "mode";
    Serial.print("[DEBUG] Jumlah mode yang akan dikirim: ");
    Serial.println(count);
    for (int i = 0; i < count; i++) {
        doc["siren"][i] = modelBuffer[i];
        Serial.print("[DEBUG] Mode[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(modelBuffer[i]);
    }
    serializeJson(doc, messageData.data);
    Serial.print("[DEBUG] Payload JSON mode: ");
    Serial.println(messageData.data);

    memset(messageData.buffer, 0, sizeof(messageData.buffer));
    index = 0;
    messageData.dataLen = 0;
    sendData();
    memset(messageData.data, 0, sizeof(messageData.data));
    Serial.println("[DEBUG] Selesai mengirim mode dan membersihkan buffer data");
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

    // uint8_t custom_addr[6] = {0x34, 0x85, 0x18, 0x8F, 0xBD, 0x04};
    // memcpy(peerInfo.peer_addr, custom_addr, 6);

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        Serial.println(messaging);
        esp_err_t result = esp_now_send(peerInfo.peer_addr, (uint8_t*)messaging, 12);
        if (result != ESP_OK) {
            Serial.printf("Send Error: %s\n", esp_err_to_name(result));
        }else
        {
            Serial.printf("Send Success");
        }
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