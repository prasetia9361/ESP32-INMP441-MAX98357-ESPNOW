#include "spiffsHandler.h"

spiffsHandler::spiffsHandler(){}

void spiffsHandler::init(){
    if (!SPIFFS.begin(true)) {
        Serial.println("Gagal menginisialisasi SPIFFS");
        return;
    }
    
    if (SPIFFS.exists("/config.json"))
    {
        JsonDocument doc;
        File file = SPIFFS.open("/config.json",FILE_READ);

        if (!file) {
            Serial.println("Gagal membuka file untuk dibaca");
            memset(configData.macAddress, 0, 6);
            return;
        }

        char macStr[128];
        file.readBytes(macStr,file.size());
        file.close();
        
        DeserializationError error = deserializeJson(doc, macStr);
        if (error) {
            Serial.print("deserializeJson() returned ");
        }

        const char* dataMac = doc["address"];
        if (dataMac == nullptr) {
            Serial.println("Output is null, invalid JSON structure");
            return;
        }
       
        if (strlen(dataMac) != 17) {
            Serial.println("Invalid MAC address length");
            return;
        }

        for (int i = 0; i < 6; i++) {
            char byteStr[3];
            byteStr[0] = dataMac[i * 3];
            byteStr[1] = dataMac[i * 3 + 1];
            byteStr[2] = '\0';
            configData.macAddress[i] = (uint8_t)strtol(byteStr, nullptr, 16);
        }

        Serial.println("MAC address converted successfully");
    }
}
void spiffsHandler::writeMacAddress(const uint8_t *mac){
    JsonDocument doc;
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    doc["address"] = macStr;

    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
    }

    serializeJson(doc, file);
    file.close();
    memcpy(configData.macAddress,mac,6);

    for (int i = 0; i < 6; i++) {
        Serial.print(configData.macAddress[i], HEX);
        if (i < 5) {
            Serial.print(":");
        }
    }
    Serial.println();
}