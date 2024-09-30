#include <Arduino.h>
#include "EspNowHandler.h"
#include "spiffs_handler.h"

EspNowHandler * espNow;
spiffs_handler * spiffs;

SemaphoreHandle_t bindingSemaphore;
String receivedData;
const int buttonPin = 0;
int pressCount;
unsigned long currentTime;
unsigned long lastPress = 0;
unsigned long currenMillis;
unsigned long sendingMillis = 0;
uint8_t receiverMAC[6];
uint8_t* macAddr;
uint8_t* incomingData;

void setup(){
  Serial.begin(115200);
  espNow = new EspNowHandler();
  spiffs = new spiffs_handler();
  pinMode(buttonPin, INPUT_PULLUP);
  espNow->init();
  spiffs->init();
  espNow->callBack();
  // memset(receiverMAC, 0, sizeof(receiverMAC)); 
  // bindingSemaphore = xSemaphoreCreateMutex();
  Serial.println("Sistem transmitter binding strated");
  // spiffs->readClose(receiverMAC);
}

void loop(){
  // if (xSemaphoreTake(bindingSemaphore, portMAX_DELAY) == pdTRUE)
  // {
  
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW){
    currentTime = millis();
    if (currentTime - lastPress > 300){
      pressCount++;
      lastPress = currentTime;
      Serial.print("Tombol ditekan: ");
      Serial.println(pressCount);
      if (pressCount == 2){
        Serial.println("Proses Binding Dimulai...");
        espNow->bindingMode();
        pressCount = 0;
      }
    }
  }
       // xSemaphoreGive(bindingSemaphore);
  // }

  macAddr = espNow->getMacAddr();
  incomingData = espNow->getIncomingData();
  if (macAddr != nullptr && incomingData != nullptr) {
    spiffs->write(macAddr, incomingData);
  }

  //  else {
  //   Serial.println("Error: Invalid MAC address in loop");
  // }

  // Hanya baca dari SPIFFS jika receiverMAC masih kosong
  if (memcmp(receiverMAC, "\0\0\0\0\0\0", 6) == 0) {
    spiffs->readClose(receiverMAC);
  }else{
    esp_err_t result = espNow->addPeer(receiverMAC);
    if (result == ESP_OK){
      Serial.println("berhasil ditambahkan sebagai peer");
    }else{
      Serial.print("gagal menambhkan mac adrr receiver dengan kode :");
      Serial.println(result);
    }
    currenMillis = millis();
    if (currenMillis - sendingMillis >= 2000){
      const char* message = "Data dari transmiter dikirim ke receiver";
      esp_err_t sending = espNow->sendingData(receiverMAC, (uint8_t*)message, strlen(message));
      // esp_err_t sending = espNow->sendingData(receiverMAC, reinterpret_cast<const uint8_t*>(message.c_str()), message.length());
      if (sending == ESP_OK) {
          Serial.println("Data berhasil dikirim");
      } else {
          Serial.println("Gagal mengirim data");
      }
      sendingMillis = currenMillis;
    }
    
  }
  
  // Serial.print("Data diterima dari SPIFFS: ");
  // char macStr[18];
  // snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
  //     receiverMAC[0], receiverMAC[1], receiverMAC[2], receiverMAC[3], receiverMAC[4], receiverMAC[5]);
  // Serial.println(macStr);


  vTaskDelay(200 / portTICK_PERIOD_MS);
}
