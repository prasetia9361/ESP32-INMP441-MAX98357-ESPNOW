#include "spiffs_handler.h"
// #include "EspNowHandler.h"
spiffs_handler::spiffs_handler(){}
void spiffs_handler::init(){
    // espNow = _espNow;
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS");
    return;
  }
  // memset(macAddr, 0,sizeof(macAddr));
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
    receivedData = String((char*)incomingData);
    Serial.println(receivedData);
}

void spiffs_handler::readClose(uint8_t * mac) {
    if (!exists()) {
        Serial.println("File tidak ditemukan");
        memset(mac, 0, 6);
        return;
    }

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

