#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
// #include "I2SMEMSSampler.h"
#include "OutputBuffer.h"
#include "config.h"
#include "spiffsHandler.h"
#include "OneButton.h"

OneButton bindingButton(BINDING_BUTTON, true);
bool mode = false;
bool removeData = false;

static void application_task(void *param) {
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application() {
    m_output_buffer = new OutputBuffer(300 * 16);
    // m_input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config, 128);
    spiffs = new spiffsHandler();  
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    
}

void doubleClick(){
    Serial.println("diklik dua kali");
    mode = true;
    // m_transport->statusBinding();
}

void longPress(){
    removeData = true;
}

void Application::begin() {
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    m_transport->begin();
    spiffs->init();
    
    

    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    bindingButton.attachClick(doubleClick);
    bindingButton.attachLongPressStop(longPress);
    pinMode(BINDING_BUTTON, INPUT_PULLUP);
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 16384, this, 5,
                &task_handle);
}

void Application::loop() {
    unsigned long lastPress = 0;  // Ensure this is initialized
    int pressCount = 0;
    unsigned long lastDebounceTime = 0;  // last time the button state changed
    int lastButtonState = HIGH;          // previous reading from the input pin
    int buttonState;                     // current reading from the input pin
    

    while (true) {
        if (mode)
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            mode = false;
        }

        if (removeData)
        {
            // Serial.println("mengkosongkan file spiffs");
            spiffs->deleteAddress();
            removeData = false;
        }
        

       m_transport->peerReady();

        // if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
            // Serial.println("Started transmitting");
            // m_input->start();

        // unsigned long start_time = millis();
        // if (millis() - start_time >= 1000) {
        m_transport->sendChar("esp32remote");
        delay(1000);
        //     start_time = millis(); // Reset timer
        // }

            // m_transport->flush();
            // Serial.println("Finished transmitting");
            // m_input->stop();
        // }
        bindingButton.tick();
    }
    // free(samples);
    delay(50);
}
