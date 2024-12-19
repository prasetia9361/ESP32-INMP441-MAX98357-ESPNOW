#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "audio.h"
#include "OutputBuffer.h"
#include "memory.h" 
#include "config.h"
#include "OneButton.h"
#include "button/button.h"

Transport *m_transport;
memory *m_memory; 
OutputBuffer *m_output_buffer;
audio *m_output;
button *m_button; 

unsigned long currentTime;
volatile bool stateBinding = false;

void application_task(void *param);

void setup()
{
    Serial.begin(115200);
    
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new audio(I2S_NUM_0,i2s_speaker_pins,256);
    m_memory = new memory();
    m_transport = new EspNowTransport(m_output_buffer, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(BINDING_BUTTON); 

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
    m_memory->init(); 
    m_output->startSpeaker(SAMPLE_RATE); 

    m_button->begin(); 
    Serial.println("Application started");

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 10192, NULL, 1, &task_handle);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param){
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    
    while (true)
    {
        if (m_button->getMode()) 
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            m_transport->setBinding(true);
            m_button->setMode(false); 
        }
        stateBinding = m_transport->getBinding();

        if (m_button->getRemove()) 
        {
            m_memory->deleteAddress(); 
            m_button->setRemove(false); 
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
     
        m_button->tick();
    }
}
