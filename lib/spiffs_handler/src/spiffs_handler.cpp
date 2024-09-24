#include "spiffs_handler.h"
#include "EspNowHandler.h"
spiffs_handler::spiffs_handler(){}
void spiffs_handler::init(EspNowHandler* _espNow){
    espNow = _espNow;
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS");
    return;
  }
}

void spiffs_handler::write(const uint8_t * mac, const uint8_t * incomingData){
    File file = SPIFFS.open("/receiverMAC.bin", FILE_WRITE);
    if (file) {
      file.write(mac, 6);
      file.close();
      Serial.println("Receiver MAC disimpan ke SPIFFS");
    } else {
      Serial.println("Gagal membuka file untuk menulis");
    }
      // Cetak nilai dari incomingData di serial monitor
  Serial.print("Nilai incomingData: ");
  receivedData = String((char*)incomingData);
  Serial.println(receivedData);
}

void spiffs_handler::readClose(uint8_t * mac){
    if (!exists){
    File file = readData();
        if (file){
            file.read(mac, 6);
            file.close();
            esp_err_t result = espNow->addPeer(mac);
            if (result == ESP_OK) {
            Serial.println("Receiver ditambahkan sebagai peer");
            } else {
                Serial.print("Gagal menambahkan Receiver sebagai peer. Error code: ");
                Serial.println(result);
            }
            currentMillis = millis();
            if (currentMillis - lastSend >= 2000) {
                const char* message = "data_dari_transmiter";
                esp_err_t result =espNow->sendingData(mac, (const uint8_t*)message, strlen(message));
                if (result == ESP_OK) {
                    Serial.println("Data berhasil dikirim");
                } else {
                    Serial.println("Gagal mengirim data");
                }
                lastSend = currentMillis;
            }
        } else {
            Serial.println("Gagal membuka file MAC address");
        }
    } else {
      Serial.println("Menunggu proses binding...");
    }
}
    
