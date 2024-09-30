#include <Arduino.h>
#include "EspNowHandler.h"
#include "spiffs_handler.h"


EspNowHandler * espNow;
spiffs_handler * spiffs;
// Pin untuk tombol
const int buttonPin = 0; // Sesuaikan dengan pin yang digunakan
unsigned long currentTime;
unsigned long lastPress = 0;
unsigned long currenMillis;
unsigned long sendingMillis = 0;
const unsigned long debounce = 300;
int pressCount = 0;
// volatile bool stateBinding = false;
bool stateBinding = false;

// Variabel untuk menyimpan MAC Address
uint8_t transmitterMAC[6];

uint8_t* macAddr;
uint8_t* incomingData;
// uint8_t receiverMAC[6];



void setup() {
  Serial.begin(115200);
  espNow = new EspNowHandler();
  spiffs = new spiffs_handler();
  pinMode(buttonPin, INPUT_PULLUP);
  espNow->init();
  spiffs->init();
  espNow->callBack();
  Serial.println("Sistem reciver binding strated");

  // spiffs->readClose(transmitterMAC);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) { // Asumsikan tombol terhubung ke GND
    currentTime = millis();
    if (currentTime - lastPress > debounce) {
      pressCount++;
      lastPress = currentTime;
      Serial.print("Tombol ditekan: ");
      Serial.println(pressCount);
      
      if (pressCount == 2) {
        // Reset count
        stateBinding = true;
        espNow->bindingMode();
        pressCount = 0;
        
        // Lakukan binding
        Serial.println("Proses Binding Dimulai...");
        // Mengirim broadcast untuk mencari transmitter
      }
    }
  }
  if (memcmp(transmitterMAC, "\0\0\0\0\0\0", 6) == 0) {
    spiffs->readClose(transmitterMAC);
  }
  macAddr = espNow->getMacAddr();
  incomingData = espNow->getIncomingData();
  if (macAddr != nullptr && incomingData != nullptr) {
    if (stateBinding){
      spiffs->write(macAddr, incomingData);
      stateBinding = false;
    }else{
      Serial.print("Nilai incomingData: ");
      String message = String((char*)incomingData,espNow->getSize());
      Serial.println(message);
      esp_err_t result = espNow->addPeer(transmitterMAC);
      if (result == ESP_OK){
        Serial.println("berhasil ditambahkan sebagai peer");
      }else{
        Serial.print("gagal menambahkan mac adrr dengan kode :");
        Serial.println(result);
      }
      esp_err_t sending = espNow->sendingData(transmitterMAC, (uint8_t*)message.c_str(), message.length());
      if (sending == ESP_OK) {
          Serial.println("Data berhasil dikirim");
      } else {
          Serial.println("Gagal mengirim data");
      }
    }
  }
    vTaskDelay(200 / portTICK_PERIOD_MS);
}
