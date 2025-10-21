#include "storage.h"

storage::storage() {
    _semaphore = xSemaphoreCreateMutex(); // Buat mutex
    if (_semaphore == NULL) {
        Serial.println("[ERROR] Gagal membuat mutex storage!");
    }
}

storage::~storage() {
    if (_semaphore != NULL) {
        vSemaphoreDelete(_semaphore); // Hapus mutex saat objek dihancurkan
    }
}

void storage::init() {
    // Melindungi akses ke configData dengan mutex
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        Serial.println("[DEBUG] Memulai inisialisasi storage");

        if (!SPIFFS.begin(true)) {
            Serial.println("[ERROR] SPIFFS init failed");
            xSemaphoreGive(_semaphore); // Lepaskan mutex sebelum return
            return;
        }

        Serial.println("[DEBUG] SPIFFS berhasil diinisialisasi");

        // Handle config.json
        if (SPIFFS.exists("/config.json")) {
            Serial.println("[DEBUG] File config.json ditemukan");
            File file = SPIFFS.open("/config.json", FILE_READ);
            if (file) {
                size_t size = file.size();
                Serial.printf("[DEBUG] Ukuran file config.json: %d bytes\n", size);
                
                if (size > 0 && size < 512) {
                    String content = file.readString();
                    file.close();
                    Serial.println("[DEBUG] Konten config.json berhasil dibaca");
    
                    JsonDocument doc;
                    if (deserializeJson(doc, content) == DeserializationError::Ok) {
                        Serial.println("[DEBUG] JSON parsing berhasil");
                        // Process MAC addresses
                        for (int i = 0; i < 2; i++) {
                            char key[10];
                            sprintf(key, "address%d", i);
                            Serial.printf("[DEBUG] Memproses key: %s\n", key);
                            
                            if (doc[key].is<JsonArray>()) {
                                JsonArray arr = doc[key];
                                Serial.printf("[DEBUG] Ukuran array untuk %s: %d\n", key, arr.size());
                                
                                if (arr.size() >= 2) {
                                    const char* macStr = arr[0];
                                    const char* devName = arr[1];
                                    Serial.printf("[DEBUG] MAC: %s, Device: %s\n", macStr, devName);
                                    
                                    if (strlen(macStr) == 17) {
                                        uint8_t* targetMac = (i == 0) ? configData.macAddress : configData.macAddress1;
                                        char* targetName = (i == 0) ? configData.nameDevice1 : configData.nameDevice2;
                                        
                                        for (int j = 0; j < 6; j++) {
                                            targetMac[j] = strtoul(macStr + j*3, NULL, 16);
                                        }
                                        Serial.printf("[DEBUG] MAC address yang disimpan: %02X:%02X:%02X:%02X:%02X:%02X\n",
                                            targetMac[0], targetMac[1], targetMac[2], targetMac[3], targetMac[4], targetMac[5]);
                                        
                                        strncpy(targetName, devName, 11);
                                        Serial.printf("[DEBUG] Nama device yang disimpan: %s\n", targetName);
                                    }
                                }
                            }
                        }
                    } else {
                        Serial.println("[ERROR] Gagal parsing JSON config.json");
                    }
                }
            }
        } else {
            Serial.println("[DEBUG] File config.json tidak ditemukan");
        }

        // Handle mode.json
        if (SPIFFS.exists("/mode.json")) {
            Serial.println("[DEBUG] File mode.json ditemukan");
            File file = SPIFFS.open("/mode.json", FILE_READ);
            if (file) {
                JsonDocument doc;
                DeserializationError error = deserializeJson(doc, file);
                if (error == DeserializationError::Ok) {
                    Serial.println("[DEBUG] JSON parsing mode.json berhasil");
                    configData.dataInt = doc["volume"] | 0; // Default ke 0 jika tidak ada
                    Serial.printf("[DEBUG] Volume yang dibaca: %d\n", configData.dataInt);
                    
                    JsonArray modes = doc["mode"];
                    Serial.printf("[DEBUG] Jumlah mode yang ditemukan: %d\n", modes.size());
                    
                    for (int i = 0; i < 9 && i < modes.size(); i++) {
                        configData.modeArray[i] = modes[i] | 0; // Default ke 0 jika tidak ada
                        Serial.printf("[DEBUG] Mode[%d] = %d\n", i, configData.modeArray[i]);
                    }
                } else {
                    Serial.printf("[ERROR] Gagal parsing JSON mode.json: %s\n", error.c_str());
                }
                file.close();
            } else {
                 Serial.println("[ERROR] Gagal membuka file mode.json");
            }
        } else {
            Serial.println("[DEBUG] File mode.json tidak ditemukan");
        }

        Serial.println("[DEBUG] Inisialisasi storage selesai");
        xSemaphoreGive(_semaphore); // Lepaskan mutex
    } else {
        Serial.println("[ERROR] storage::init: Gagal mendapatkan mutex.");
    }
}

void storage::writeMacAddress(const uint8_t *mac, const char *device, int count) {
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        // Validasi parameter input, ini sudah benar.
        if (mac == nullptr || device == nullptr || count <= 0) {
            Serial.println("[ERROR] Parameter tidak valid di writeMacAddress.");
            xSemaphoreGive(_semaphore);
            return;
        }

        JsonDocument doc;
        File fileRead = SPIFFS.open("/config.json", FILE_READ);
        if (fileRead) {
            DeserializationError error = deserializeJson(doc, fileRead);
            if (error) {
                Serial.printf("[ERROR] Gagal deserialize config.json: %s\n", error.c_str());
                // Tetap lanjutkan dengan doc kosong jika deserialisasi gagal
            }
            fileRead.close();
        }

        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        bool macAlreadyExists = false;
        int firstEmptySlot = -1;

        for (int i = 0; i < count; i++) {
            char key[10];
            sprintf(key, "address%d", i);

            if (doc[key].is<JsonArray>()) {
                JsonArray arr = doc[key].as<JsonArray>();
                if (arr.size() > 0) {
                    const char* existingMac = arr[0];
                    
                    if (existingMac != nullptr && strcmp(existingMac, macStr) == 0) {
                        macAlreadyExists = true;
                        Serial.printf("[DEBUG] MAC %s sudah ada di key %s.\n", macStr, key);
                        break;
                    }
                } else {
                    if (firstEmptySlot == -1) {
                        firstEmptySlot = i;
                    }
                }
            } else {
                if (firstEmptySlot == -1) {
                    firstEmptySlot = i;
                }
            }
        }

        if (macAlreadyExists) {
            Serial.println("[DEBUG] Tidak ada perubahan, MAC sudah terdaftar.");
        } else if (firstEmptySlot != -1) {
            Serial.printf("[DEBUG] Menambahkan MAC %s ke slot %d.\n", macStr, firstEmptySlot);
            
            char key[10];
            sprintf(key, "address%d", firstEmptySlot);

            JsonArray arr = doc[key].to<JsonArray>();
            arr.clear(); 
            arr.add(macStr);
            arr.add(device);

            if (firstEmptySlot == 0) {
                memcpy(configData.macAddress, mac, 6);
                strncpy(configData.nameDevice1, device, 11);
            } else {
                memcpy(configData.macAddress1, mac, 6);
                strncpy(configData.nameDevice2, device, 11);
            }

            // Tulis kembali ke file
            File fileWrite = SPIFFS.open("/config.json", FILE_WRITE);
            if (fileWrite) {
                if (serializeJson(doc, fileWrite) == 0) {
                    Serial.println("[ERROR] Gagal menulis JSON ke file config.json");
                } else {
                    Serial.println("[DEBUG] config.json berhasil diperbarui.");
                }
                fileWrite.close();
            } else {
                Serial.println("[ERROR] Gagal membuka file config.json untuk ditulis.");
            }
        } else {
            Serial.println("[DEBUG] Tidak ada slot kosong yang tersedia.");
        }

        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] storage::writeMacAddress: Gagal mendapatkan mutex.");
    }
}

void storage::writeMode(const uint8_t *bufferMode, int count){
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        if (bufferMode == nullptr || count <= 0) {
            Serial.println("[ERROR] bufferMode null atau count tidak valid di writeMode(uint8_t*).");
            xSemaphoreGive(_semaphore);
            return;
        }

        JsonDocument doc;
        File fileRead = SPIFFS.open("/mode.json", FILE_READ);
        if (fileRead) {
            DeserializationError error = deserializeJson(doc, fileRead);
            if (error) {
                Serial.printf("[ERROR] Gagal deserialize mode.json: %s\n", error.c_str());
            }
            fileRead.close();
        }

        JsonArray modes = doc["mode"].to<JsonArray>(); // Pastikan array mode ada dan bersih
        modes.clear(); // Hapus entri lama

        for (int i = 0; i < count && i < sizeof(configData.modeArray)/sizeof(configData.modeArray[0]); i++) {
            modes.add(bufferMode[i]);
            configData.modeArray[i] = bufferMode[i];
        }

        File fileWrite = SPIFFS.open("/mode.json", FILE_WRITE);
        if (!fileWrite) {
            Serial.println("[ERROR] Gagal membuka file untuk menulis mode.");
            xSemaphoreGive(_semaphore);
            return;
        }

        if (serializeJson(doc, fileWrite) == 0) {
            Serial.println("[ERROR] Gagal menulis JSON ke file mode.json");
        } else {
            Serial.println("[DEBUG] Mode berhasil disimpan ke SPIFFS");
        }
        fileWrite.close();
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] storage::writeMode(uint8_t*): Gagal mendapatkan mutex.");
    }
}

void storage::writeMode(const int32_t* bufferMode, int count){
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        if (bufferMode == nullptr || count <= 0) {
            Serial.println("[ERROR] bufferMode null atau count tidak valid di writeMode(int32_t*).");
            xSemaphoreGive(_semaphore);
            return;
        }

        JsonDocument doc;
        File fileRead = SPIFFS.open("/mode.json", FILE_READ);
        if (fileRead) {
            DeserializationError error = deserializeJson(doc, fileRead);
            if (error) {
                Serial.printf("[ERROR] Gagal deserialize mode.json: %s\n", error.c_str());
            }
            fileRead.close();
        }

        JsonArray modes = doc["mode"].to<JsonArray>(); // Pastikan array mode ada dan bersih
        modes.clear(); // Hapus entri lama

        for (int i = 0; i < count && i < sizeof(configData.modeArray)/sizeof(configData.modeArray[0]); i++) {
            modes.add(bufferMode[i]);
            configData.modeArray[i] = bufferMode[i];
        }

        File fileWrite = SPIFFS.open("/mode.json", FILE_WRITE);
        if (!fileWrite) {
            Serial.println("[ERROR] Gagal membuka file untuk menulis mode.");
            xSemaphoreGive(_semaphore);
            return;
        }

        if (serializeJson(doc, fileWrite) == 0) {
            Serial.println("[ERROR] Gagal menulis JSON ke file mode.json");
        } else {
            Serial.println("[DEBUG] Mode berhasil disimpan ke SPIFFS");
        }
        fileWrite.close();
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] storage::writeMode(int32_t*): Gagal mendapatkan mutex.");
    }
}

bool storage::saveVolume(int data){
        JsonDocument doc;
        // File fileRead = SPIFFS.open("/mode.json", FILE_READ);
        // if (fileRead) {
        //     DeserializationError error = deserializeJson(doc, fileRead);
        //     if (error) {
        //         // Serial.printf("[ERROR] Gagal deserialize mode.json: %s\n", error.c_str());
        //         return false;
        //     }
        //     fileRead.close();
        // }

        File fileWrite = SPIFFS.open("/mode.json", FILE_WRITE);
        if (!fileWrite) {
            // ESP_LOGE(TAG,"[ERROR] Gagal membuka file untuk menulis volume");
            return false;
        }

        doc["volume"] = data;
        configData.dataInt = data;
        serializeJson(doc, fileWrite);
        
        fileWrite.close();
        return true;
}

void storage::deleteAddress() {
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        File file = SPIFFS.open("/config.json", FILE_WRITE);
        if (!file) {
            Serial.println("[ERROR] failed to open file for writing");
            xSemaphoreGive(_semaphore);
            return;
        }
        
        JsonDocument doc; 

        // Menulis dokumen kosong akan menghapus semua konten
        if (serializeJson(doc, file) == 0) {
             Serial.println("[ERROR] Gagal menulis JSON kosong ke config.json");
        }
        file.close();
        memset(configData.macAddress, 0, 6);
        memset(configData.macAddress1, 0, 6);
        memset(configData.nameDevice1, 0, sizeof(configData.nameDevice1));
        memset(configData.nameDevice2, 0, sizeof(configData.nameDevice2));
        Serial.println("Addresses cleared successfully");
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] storage::deleteAddress: Gagal mendapatkan mutex.");
    }
}

bool storage::hapusAlamat(const char *deviceName) {
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        File fileRead = SPIFFS.open("/config.json", FILE_READ);
        if (!fileRead) {
            Serial.println("[ERROR] Gagal membuka config.json untuk dibaca.");
            xSemaphoreGive(_semaphore);
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, fileRead);
        fileRead.close();

        if (error) {
            Serial.printf("[ERROR] deserializeJson() gagal: %s\n", error.c_str());
            xSemaphoreGive(_semaphore);
            return false;
        }

        JsonObject root = doc.as<JsonObject>();
        const char* keyToRemove = nullptr;

        for (JsonPair kv : root) {
            JsonArray arr = kv.value().as<JsonArray>();
            
            if (!arr.isNull() && arr.size() >= 2) {
                const char* currentDeviceName = arr[1];

                if (currentDeviceName && strcmp(currentDeviceName, deviceName) == 0) {
                    
                    keyToRemove = kv.key().c_str();
                    Serial.printf("Perangkat '%s' ditemukan pada kunci '%s'. Akan dihapus.\n", deviceName, keyToRemove);
                    break;
                }
            }
        }

        if (keyToRemove != nullptr) {
            if (strcmp(keyToRemove, "address0") == 0) {
                memset(configData.macAddress, 0, sizeof(configData.macAddress));
                memset(configData.nameDevice1,0,sizeof(configData.nameDevice1));
            } else if (strcmp(keyToRemove, "address1") == 0) {
                memset(configData.macAddress1, 0, sizeof(configData.macAddress1));
                memset(configData.nameDevice2,0,sizeof(configData.nameDevice2));
            }
            
            root.remove(keyToRemove); // Hapus entry dari JSON

            File fileWrite = SPIFFS.open("/config.json", FILE_WRITE);
            if (!fileWrite) {
                Serial.println("[ERROR] Gagal membuka config.json untuk ditulis.");
                xSemaphoreGive(_semaphore);
                return false;
            }

            if (serializeJson(doc, fileWrite) == 0) {
                Serial.println("[ERROR] Gagal menulis ke file.");
            } else {
                Serial.println("[DEBUG] File config.json berhasil diperbarui.");
            }
            fileWrite.close();
            xSemaphoreGive(_semaphore);
            return true;
        } else {
            Serial.printf("[DEBUG] Perangkat dengan nama '%s' tidak ditemukan.\n", deviceName);
            xSemaphoreGive(_semaphore);
            return false;
        }
    } else {
        Serial.println("[ERROR] storage::hapusAlamat: Gagal mendapatkan mutex.");
        return false;
    }
}

uint8_t targetMac[6];
uint8_t *storage::getMac(int i){
    // static uint8_t targetMac[6];
    uint8_t *mac = nullptr;
    // Handle config.json
    File file = SPIFFS.open("/config.json", FILE_READ);
    if (file) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        char key[10];
        sprintf(key, "address%d", i);
        
        if (doc[key].is<JsonArray>()) {
            JsonArray arr = doc[key];
            
            if (!arr.isNull() && arr.size() >= 2) {
                const char* macStr = arr[0];
                
                if (macStr && strlen(macStr) == 17) { // Memastikan macStr valid
                    
                    for (int j = 0; j < 6; j++) {
                        targetMac[j] = strtoul(macStr + j*3, NULL, 16);
                    }
                    mac = targetMac;
                    
                }
            }
        }
    }
    return mac;
}

uint8_t *storage::getMac(){
    uint8_t *mac_ptr = nullptr;
    xSemaphoreTake(_semaphore, portMAX_DELAY);
    if (configData.macAddress[0] != 0)
    {
        mac_ptr = configData.macAddress;
    }
    xSemaphoreGive(_semaphore);
    return mac_ptr;
}

uint8_t *storage::getMac1(){
    uint8_t *mac_ptr = nullptr;
    xSemaphoreTake(_semaphore, portMAX_DELAY);
    if (configData.macAddress1[0] != 0)
    {
        mac_ptr = configData.macAddress1;
    }
 
    xSemaphoreGive(_semaphore);
    return mac_ptr;
}

int32_t *storage::readModeTones(){
    int32_t *mode_ptr = nullptr;
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        mode_ptr = configData.modeArray;
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] readModeTones: Gagal mendapatkan mutex, mengembalikan nullptr.");
    }
    return mode_ptr;
}

const char *storage::getDevice(int i){
    char* dev = nullptr;

    File file = SPIFFS.open("/config.json", FILE_READ);
    if (!file) {
        Serial.println("[ERROR] Gagal membuka file config.json");
        return nullptr;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.printf("[ERROR] Gagal parsing JSON: %s\n", error.c_str());
        return nullptr;
    }

    char key[10];
    sprintf(key, "address%d", i);
    Serial.printf("[DEBUG] Mencari kunci: %s\n", key);
    
    if (doc[key].is<JsonArray>()) {
        JsonArray arr = doc[key];
        
        Serial.printf("[DEBUG] Ukuran array: %d\n", arr.size());
        
        if (!arr.isNull() && arr.size() >= 2) {
            const char *devName = arr[1];
            Serial.printf("[DEBUG] Nama device ditemukan: %s\n", devName);

            dev = (i == 0) ? configData.nameDevice1 : configData.nameDevice2;
            strncpy(dev, devName, sizeof(configData.nameDevice1) - 1);
            dev[sizeof(configData.nameDevice1) - 1] = '\0';
            
            Serial.printf("[DEBUG] Nama device disalin: %s\n", dev);
        } else {
            Serial.println("[DEBUG] Array kosong atau tidak memiliki cukup elemen");
        }
    } else {
        Serial.println("[DEBUG] Kunci tidak ditemukan atau bukan array");
    }
    
    return dev;
}

char *storage::device1(){
    char *name_ptr = nullptr;
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        name_ptr = configData.nameDevice1;
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] device1: Gagal mendapatkan mutex, mengembalikan nullptr.");
    }
    return name_ptr;
}

char *storage::device2(){
    char *name_ptr = nullptr;
    if (_semaphore != NULL && xSemaphoreTake(_semaphore, portMAX_DELAY) == pdTRUE) {
        name_ptr = configData.nameDevice2;
        xSemaphoreGive(_semaphore);
    } else {
        Serial.println("[ERROR] device2: Gagal mendapatkan mutex, mengembalikan nullptr.");
    }
    return name_ptr;
}

int storage::getVolume(){
    int volume = 0;
    volume = configData.dataInt;
    return volume;
}