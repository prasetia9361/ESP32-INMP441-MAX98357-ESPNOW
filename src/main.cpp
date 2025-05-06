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
#endif
    TaskHandle_t taskHandler;
    xTaskCreate(applicationTask, "applicationTask", 10192, NULL, 2, &taskHandler);

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
    }
    receiver->clearSample();
#endif

#ifdef TRANSMITTER
    while (true)
    {
        transmitter->processBinding();
        transmitter->trasnmitData();
    }
    transmitter->clearSample();
#endif
#ifdef DISP
    while (true)
    {
        lcd->showDataReceive();
        lcd->loadPage();
        lcd->binding();
        lcd->deleteAddress();
        lcd->sendMassage();
        lcd->changeScreen();
        vTaskDelay(10);
    }
    
#endif
}
