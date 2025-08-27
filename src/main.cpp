#include <Arduino.h>
#ifdef RECEIVER
#include "receiver/receiverTask.h"
receiverTask *receiver;
#endif
#ifdef TRANSMITTER 
#include "transmitter/transmitterTask.h"
transmitterTask * transmitter;
#endif
#ifdef DISP
#include "display/displayTask.h"
displayTask *lcd;
#endif
void applicationTask(void *param);



void setup(){
    Serial.begin(115200);
#ifdef RECEIVER
    receiver = new receiverTask();

    receiver->begin();
#endif
#ifdef TRANSMITTER
    transmitter = new transmitterTask();
    transmitter->begin();
#endif
#ifdef DISP
    lcd = new displayTask();
    lcd->begin();
    
    // Delay tambahan untuk display
    delay(500);
#endif
    TaskHandle_t taskHandler;
    BaseType_t result = xTaskCreate(
        applicationTask, 
        "applicationTask", 
        32768, 
        NULL, 
        2, 
        &taskHandler
    );
    
    if (result != pdPASS) {
        Serial.println("Failed to create task!");
    }
}

void loop(){
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void applicationTask(void *param){
#ifdef RECEIVER
    while (true)
    {
        receiver->processBinding();
        receiver->receiveData();
        // vTaskDelay(pdMS_TO_TICKS(5)); // Tambahkan delay untuk menghindari watchdog
        // vTaskDelay(5);
    }
    receiver->clearSample();
#endif

#ifdef TRANSMITTER
    while (true)
    {
        
        transmitter->trasnmitData();
        transmitter->processBinding();
        // vTaskDelay(10);
        // vTaskDelay(5);
    }
    transmitter->clearSample();
#endif

#ifdef DISP
    while (true) {
        if (lcd) {
            lcd->tick();
            lcd->updateData();
            lcd->testSiren();
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Increased delay for stability
    }
#endif
}
