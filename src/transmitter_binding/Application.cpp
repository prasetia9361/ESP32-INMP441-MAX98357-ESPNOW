#include "Application.h"

#include <Arduino.h>
static void binding_task(void *param)
{
    // delegate onto the application
    Application *application = reinterpret_cast<Application *>(param);
    application->bindingLoop();
}

static void sending_task(void *param){
    Application *application = reinterpret_cast<Application *>(param);
    application->sendingLoop();
}

Application::Application(){
    espNow = new EspNowHandler();
    spiffs = new spiffs_handler();
    pinMode(buttonPin, INPUT_PULLUP);
    // 
}

void Application::init(){
    espNow->init();
    spiffs->init(espNow);
    espNow->callBack();
    bindingSemaphore = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
    binding_task,   /* Task function. */
    "BindingTask", /* name of task. */
    10000,         /* Stack size of task */
    this,          /* parameter of the task */
    1,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    0);            /* pin task to core 0 */

    xTaskCreatePinnedToCore(
    sending_task,   /* Task function. */
    "SendingTask", /* name of task. */
    15000,         /* Stack size of task */
    this,          /* parameter of the task */
    1,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
}

void Application::bindingLoop(){
    for (;;){
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
                        pressCount = 0;
                        Serial.println("Proses Binding Dimulai...");
                        espNow->bindingMode();
                    }
                    
                }
                
            }
            xSemaphoreGive(bindingSemaphore);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
}

void Application::sendingLoop(){
    for (;;)
    {
       spiffs->readClose(receiverMAC);
       vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
}
