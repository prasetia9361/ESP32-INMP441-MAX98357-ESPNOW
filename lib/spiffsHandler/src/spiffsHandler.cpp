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
            memset(configData.macAddress1, 0, 6);
            return;
        }

        char macStr[128];
        file.readBytes(macStr,file.size());
        file.close();
        
        DeserializationError error = deserializeJson(doc, macStr);
        if (error) {
            Serial.print("deserializeJson() returned ");
        }

        const char* dataMac0 = doc["address0"];
        const char* dataMac1 = doc["address1"];
        // if (dataMac == nullptr) {
        //     Serial.println("Output is null, invalid JSON structure");
        //     // return;
        // }
       
        // if (strlen(dataMac) != 17) {// 3 * sizeof uint8_t - 1
        //     Serial.println("Invalid MAC address length");
        //     // return;
        // }

        for (int i = 0; i < 6; i++) {
            char byteStr[3];
            byteStr[0] = dataMac0[i * 3];
            byteStr[1] = dataMac0[i * 3 + 1];
            byteStr[2] = '\0';
            configData.macAddress[i] = (uint8_t)strtol(byteStr, nullptr, 16);
        }

        for (int i = 0; i < 6; i++) {
            char byteStr[3];
            byteStr[0] = dataMac1[i * 3];
            byteStr[1] = dataMac1[i * 3 + 1];
            byteStr[2] = '\0';
            configData.macAddress1[i] = (uint8_t)strtol(byteStr, nullptr, 16);
        }

        Serial.println("MAC address converted successfully");
    }
}
/*buat dua data json yaitu address dan address1
di dalam function writeMacAddress nanti di deserialisation dulu
kemudian cek apakah address kosong 
jika kosong maka data di simpan ke dalam address
jika addess terisi maka data disimpan ke address1
jika keduanya sudah terisi data tidak akan disimpan

buat function untuk menghapus data yang di address dan address1
nanti panggil di aplication saat kondisi tertentu misal saat button di tekan 3 kali
*/
void spiffsHandler::writeMacAddress(const uint8_t *mac, int count){
    JsonDocument doc;
    char macStr[18];
    char dataMac[128];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    File file = SPIFFS.open("/config.json",FILE_READ);
    if (!file) {
        Serial.println("Gagal membuka file untuk dibaca");
        memset(configData.macAddress, 0, 6);
        return;
    }
    file.readBytes(dataMac,file.size());
    file.close();
    DeserializationError error = deserializeJson(doc,dataMac);
    if (error) {
        Serial.print("deserializeJson() returned ");
    }

    for (int i = 0; i < count; i++)
    {
        char docName[9];
        sprintf(docName, "address%d", i);
        const char *address = doc[docName];
        if (strlen(address) != 17)
        {
            doc[docName] = macStr;
            File file = SPIFFS.open("/config.json", FILE_WRITE);
            if (!file) {
                Serial.println("- failed to open file for writing");
            }

            serializeJson(doc, file);
            file.close();
            switch (i){
            case 0:
                memcpy(configData.macAddress,mac,6);
                for (int i = 0; i < 6; i++) {
                    Serial.print(configData.macAddress[i], HEX);
                    if (i < 5) {
                        Serial.print(":");
                    }
                }
                Serial.println();
                break;
            case 1:
                memcpy(configData.macAddress1,mac,6);
                for (int i = 0; i < 6; i++) {
                    Serial.print(configData.macAddress1[i], HEX);
                    if (i < 5) {
                        Serial.print(":");
                    }
                }
                Serial.println();
                break;        
            default:
                break;
            }
            break;
        }else
        {
            Serial.print(docName);
            Serial.println(" is available");
            // continue;
        }
    }
    

    // const char *address = doc["address"];
    // if (strlen(address) != 17)
    // {
    //     doc["address"] = macStr;
    //     file = SPIFFS.open("/config.json", FILE_WRITE);
    //     if (!file) {
    //         Serial.println("- failed to open file for writing");
    //     }

    //     serializeJson(doc, file);
    //     file.close();
    //     memcpy(configData.macAddress,mac,6);
    //     for (int i = 0; i < 6; i++) {
    //         Serial.print(configData.macAddress[i], HEX);
    //         if (i < 5) {
    //             Serial.print(":");
    //         }
    //     }
    //     Serial.println();
    // }else{
    //     doc["address1"] = macStr;
    //     file = SPIFFS.open("/config.json", FILE_WRITE);
    // }
    
    
    
    // doc["address"] = macStr;
    // File file = SPIFFS.open("/config.json", FILE_WRITE);
    // if (!file) {
    //     Serial.println("- failed to open file for writing");
    // }

    // 
    // serializeJson(doc, file);
    // file.close();
    // memcpy(configData.macAddress,mac,6);


}