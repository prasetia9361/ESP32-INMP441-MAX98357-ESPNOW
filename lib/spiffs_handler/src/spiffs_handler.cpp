#include "spiffs_handler.h"
// #include "EspNowHandler.h"
spiffs_handler::spiffs_handler(){}
void spiffs_handler::init(){
    // espNow = _espNow;
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS");
    return;
  }
}

void spiffs_handler::write(const uint8_t* reciveMac, const uint8_t* incomingData){
//   reciveMac = espNow->getMacAddr();
//   incomingData = espNow->getIncomingData();
  // Serial.print("Data diterima dari: ");
  // char macStr[18];
  // snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
  //          reciveMac[0], reciveMac[1], reciveMac[2], reciveMac[3], reciveMac[4], reciveMac[5]);
  // Serial.println(macStr);
  if (!exists()) {
    File file = SPIFFS.open("/receiverMAC.bin", FILE_WRITE);
    if (file) {
      file.write(reciveMac, 6);
      file.close();
      Serial.println("Receiver MAC disimpan ke SPIFFS");
    } else {
      Serial.println("Gagal membuka file untuk menulis");
    }
      // Cetak nilai dari incomingData di serial monitor
    Serial.print("Nilai incomingData: ");
    receivedData = String((char*)incomingData);
    Serial.println(receivedData);
  } else {
    Serial.println("reciveMac kosong, tidak ada yang disimpan ke SPIFFS");
  }


}

void spiffs_handler::readClose(uint8_t * mac){
    uint8_t macAddr[6];
    if (SPIFFS.exists("/receiverMAC.bin")){
      File file = SPIFFS.open("/receiverMAC.bin", FILE_READ);
        if (file){
            file.read(macAddr, 6);
            file.close();
            
            // Copy the read MAC address to the provided mac variable
            memcpy(mac, macAddr, 6);
            
            // esp_err_t result = espNow->addPeer(mac);
            // if (result == ESP_OK) {
            // Serial.println("Receiver ditambahkan sebagai peer");
            // } else {
            //     Serial.print("Gagal menambahkan Receiver sebagai peer. Error code: ");
            //     Serial.println(result);
            // }
            // currentMillis = millis();
            // if (currentMillis - lastSend >= 2000) {
            //     const char* message = "data_dari_transmiter";
            //     esp_err_t result =espNow->sendingData(mac, (const uint8_t*)message, strlen(message));
            //     if (result == ESP_OK) {
            //         Serial.println("Data berhasil dikirim");
            //     } else {
            //         Serial.println("Gagal mengirim data");
            //     }
            //     lastSend = currentMillis;
            // }
        } else {
            Serial.println("Gagal membuka file MAC address");
        }
    } else {
      Serial.println("Menunggu proses binding...");
    }
}
    
