#include "Application.h"
#include <Arduino.h>

// our application
Application *application;

void setup()
{
    Serial.begin(115200);
    // start up the application
    application = new Application();
    application->begin();
    Serial.println("Application started");
}

void loop()
{
    // application->tick();
    vTaskDelay(pdMS_TO_TICKS(1000));
}
