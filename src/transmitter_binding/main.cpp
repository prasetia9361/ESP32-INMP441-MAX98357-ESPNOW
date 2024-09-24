#include <Arduino.h>
#include "Application.h"

Application * application;

void setup(){
  Serial.begin(115200);
  application = new Application();
  application->init();
  Serial.println("Sistem transmitter binding strated");
}

void loop(){
  vTaskDelay(pdMS_TO_TICKS(1000));
}