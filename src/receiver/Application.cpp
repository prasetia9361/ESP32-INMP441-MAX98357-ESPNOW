#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "I2SOutput.h"
#include "OutputBuffer.h"
#include "spiffsHandler.h" 
#include "config.h"
#include "OneButton.h"

OneButton bindingButton(BINDING_BUTTON, true);
bool mode = false;
bool removeData = false;

static void application_task(void *param)
{
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application()
{
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new I2SOutput(I2S_NUM_0, i2s_speaker_pins);
    spiffs = new spiffsHandler();
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);

    if (I2S_SPEAKER_SD_PIN != -1)
    {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }
}

void doubleClick(){
    Serial.println("diklik dua kali");
    mode = true;
    // m_transport->statusBinding();
}

void longPress(){
    removeData = true;
}

void Application::begin()
{
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    
    m_transport->begin();
    spiffs->init();
    m_output->start(SAMPLE_RATE);

    bindingButton.attachDoubleClick(doubleClick);
    bindingButton.attachLongPressStop(longPress);
    pinMode(BINDING_BUTTON,INPUT_PULLUP);

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, this, 1, &task_handle);
}

void Application::loop()
{
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    unsigned long lastPress = 0; // Ensure this is initialized
    int pressCount = 0;
    while (true)
    {
        if (mode)
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            stateBinding = true;
            m_transport->setBinding(stateBinding);
            mode = false;
        }
        stateBinding = m_transport->getBinding();

        if (removeData)
        {
            spiffs->deleteAddress();
            removeData = false;
        }
        
        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
        }

        unsigned long start_time = millis();
        if(millis() - start_time < 500){
        m_output_buffer->remove_samples(samples, 128);
        m_output->write(samples, 128);
        }

        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
        }
     
        bindingButton.tick();
        
    }
}

