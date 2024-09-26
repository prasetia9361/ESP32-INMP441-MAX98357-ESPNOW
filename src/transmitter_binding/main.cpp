#include <Arduino.h>
#include "EspNowHandler.h"
#include "spiffs_handler.h"

EspNowHandler * espNow;
spiffs_handler * spiffs;

SemaphoreHandle_t bindingSemaphore;
const int buttonPin = 0;
int pressCount;
unsigned long currentTime;
unsigned long lastPress = 0;
uint8_t receiverMAC[6];

void setup(){
  Serial.begin(115200);
  espNow = new EspNowHandler();
  spiffs = new spiffs_handler();
  pinMode(buttonPin, INPUT_PULLUP);
  espNow->init();
  spiffs->init();
  espNow->callBack();
  bindingSemaphore = xSemaphoreCreateMutex();
  Serial.println("Sistem transmitter binding strated");
}

void loop(){
  if (xSemaphoreTake(bindingSemaphore, portMAX_DELAY) == pdTRUE)
  {
      int buttonState = digitalRead(buttonPin);
      if (buttonState == LOW){
          currentTime = millis();
          if (currentTime - lastPress > 300)
          {
              pressCount++;
              lastPress = currentTime;
              Serial.print("Tombol ditekan: ");
              Serial.println(pressCount);
              if (pressCount == 2)
              {
                  Serial.println("Proses Binding Dimulai...");
                  espNow->bindingMode();
                  pressCount = 0;
              }
          }
      }
      xSemaphoreGive(bindingSemaphore);
  }
  spiffs->write(espNow->getMacAddr(),espNow->getIncomingData());
  // Sending Loop
  spiffs->readClose(receiverMAC);
  Serial.print("Data diterima dari: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
  receiverMAC[0], receiverMAC[1], receiverMAC[2], receiverMAC[3], receiverMAC[4],  receiverMAC[5]);
  Serial.println(macStr);
  // Writing Loop
  // spiffs->write();

  vTaskDelay(200 / portTICK_PERIOD_MS);
}
