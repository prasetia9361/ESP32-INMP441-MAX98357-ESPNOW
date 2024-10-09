#include "spiffs_handler.h"
// #include "EspNowHandler.h"
// uint8_t macAddr[6];
spiffs_handler::spiffs_handler(){}
void spiffs_handler::init(){
    // espNow = _espNow;
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS");
    return;
  }
//   File file = SPIFFS.open("config.json",FILE_READ);


  //read data dari spiff di kirim ke struct

  // memset(macAddr, 0,sizeof(macAddr));
}
void spiffs_handler::writeMacAddress(const uint8_t *mac){
    // untuk write mac ke spiffs 
    JsonDocument doc;
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    doc["äddress"] = macStr;
    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
    }
    serializeJson(doc, file);
    Serial.println(macStr);
    file.close();
    memcpy(configData.macAddress,mac,6);
}

void spiffs_handler::readMacAddress(){
    JsonDocument doc;
    File file = SPIFFS.open("/config.json",FILE_READ);
    if (!file) {
        Serial.println("Gagal membuka file untuk dibaca");
        memset(configData.macAddress, 0, 6);
        return;
    }
    // file.read(configData.macAddress, 6);
    char macStr[128];
    file.readBytes(macStr,file.size());
    file.close();
    DeserializationError error = deserializeJson(doc, macStr);
    if (error) {
        Serial.print("deserializeJson() returned ");
    }
    
    const char* output = doc["address"];
    // Serial.printf("mac address: %s\n", output);
    for (int i = 0; i < 6; i++)
    {
        // Serial.printf("mac address: %s\n", output);
        configData.macAddress[i] = (uint8_t) strtol(output + (i * 3),NULL,16);
        // Serial.printf("configData.macAddress[%d]: %02X:%02X:%02X:%02X:%02X:%02X\n", i, configData.macAddress[0], configData.macAddress[1], configData.macAddress[2], configData.macAddress[3], configData.macAddress[4], configData.macAddress[5]);
    }

    
    // sscanf(output, "%02X:%02X:%02X:%02X:%02X:%02X", &configData.macAddress[0], &configData.macAddress[1], &configData.macAddress[2], &configData.macAddress[3], &configData.macAddress[4], &configData.macAddress[5]);
    
}

void spiffs_handler::write(const uint8_t* reciveMac, const uint8_t* incomingData) {
    File file = SPIFFS.open("/receiverMAC.bin", FILE_WRITE);
    if (!file) {
        Serial.println("Gagal membuka file untuk ditulis");
        return;
    }
    file.write(reciveMac, 6);
    file.close();
    Serial.print("Nilai incomingData: ");
    receivedData = String((char*)incomingData, sizeof(incomingData));
    Serial.println(receivedData);
}

void spiffs_handler::readClose(uint8_t * mac) {
    if (!exists()) {
        Serial.println("File tidak ditemukan");
        memset(mac, 0, 6);
        return;
    }
//spiffs data spiffs diubah ke json dari .bin 
//file spiffs dinamai config.json

    File file = readData();
    if (!file) {
        Serial.println("Gagal membuka file untuk dibaca");
        memset(mac, 0, 6);
        return;
    }
    file.read(mac, 6);
    if (memcmp(mac, "\0\0\0\0\0\0", 6) == 0) {
        Serial.println("Data kosong");
    } else {
        Serial.println("Data MAC berhasil dibaca dari SPIFFS");
    }
    // if (file.size() >= 6) {
    //     file.read(mac, 6);
    //     Serial.println("Data MAC berhasil dibaca dari SPIFFS");
    // } else {
    //     Serial.println("File tidak berisi data MAC yang valid");
    //     memset(mac, 0, 6);
    // }
    file.close();
}

