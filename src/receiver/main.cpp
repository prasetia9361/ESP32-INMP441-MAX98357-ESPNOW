// #include "Application.h"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "audio.h"
#include "OutputBuffer.h"
#include "spiffsHandler.h" 
#include "config.h"
#include "OneButton.h"
#include "button/button.h"

Transport *m_transport;
spiffsHandler *spiffs;
OutputBuffer *m_output_buffer;
audio *m_output; // Diubah dari _output menjadi m_output
button *m_button; // Diubah dari myButton menjadi m_button

unsigned long currentTime;
volatile bool stateBinding = false;

void application_task(void *param);

void setup()
{
    Serial.begin(115200);
    
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new audio(I2S_NUM_0,i2s_speaker_pins,256); // Diubah dari _output menjadi m_output
    spiffs = new spiffsHandler();
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(BINDING_BUTTON); // Diubah dari myButton menjadi m_button

    if (I2S_SPEAKER_SD_PIN != -1)
    {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    
    m_transport->begin();
    spiffs->init();
    m_output->startSpeaker(SAMPLE_RATE); // Diubah dari _output menjadi m_output

    m_button->begin(); // Diubah dari myButton menjadi m_button
    Serial.println("Application started");

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, NULL, 1, &task_handle);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param){
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    
    while (true)
    {
        if (m_button->getMode()) // Diubah dari myButton menjadi m_button
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            m_transport->setBinding(true);
            m_button->setMode(false); // Diubah dari myButton menjadi m_button
        }
        stateBinding = m_transport->getBinding();

        if (m_button->getRemove()) // Diubah dari myButton menjadi m_button
        {
            spiffs->deleteAddress();
            m_button->setRemove(false); // Diubah dari myButton menjadi m_button
        }
        
        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
        }

        unsigned long start_time = millis();
        if(millis() - start_time < 500){
        m_output_buffer->remove_samples(samples, 128);
        m_output->write(samples, 128); // Diubah dari _output menjadi m_output
        }

        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
        }
     
        m_button->tick(); // Diubah dari myButton menjadi m_button
        
    }
}
