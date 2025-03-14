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
void tone_task(void *param);
void generateWave(void *param);

void setup()
{
    Serial.begin(115200);
    
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new audio(I2S_NUM_0,i2s_speaker_pins,128);// 256
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
    // m_output->i2sStartTone();

    m_button->begin(); 
    Serial.println("Application started");

    TaskHandle_t task_handle;
    // TaskHandle_t tone_handle;

    xTaskCreate(application_task, "application_task", 10192, NULL, 2, &task_handle);
    // xTaskCreate(tone_task,"tone_task", 1024 * 2, NULL, 3, &tone_handle );
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param){
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    int mode = 0;
    while (true)
    {
        //doubleclick
        if (m_button->getMode()) 
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            m_transport->setBinding(true);
            m_button->setMode(false); 
        }
        stateBinding = m_transport->getBinding();


        //longpress
        if (m_button->getRemove()) 
        {
            m_memory->deleteAddress(); 
            m_button->setRemove(false); 
        }
        

        mode = m_transport->getMode();
        if (mode > 0)
        {
            m_output->i2sTone(mode);
            m_output->generateSineWave();
        }else
        {
            unsigned long start_time = millis();
            while (millis() - start_time < 100 || !m_output_buffer->getBuffering()  )
            {
                if (I2S_SPEAKER_SD_PIN != -1)
                {
                    digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
                }

                m_output_buffer->remove_samples(samples, 128);
                // for (int i = 0; i < 128; i++)
                // {
                //     Serial.println(samples[i]);
                // }
                m_output->write(samples, 128);

                if (I2S_SPEAKER_SD_PIN != -1)
                {
                    digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
                }

            }
        }
        
        

        // Serial.println(mode);
        // if(mode == 0){


        // }
        // else
        // {
        // m_output->i2sTone(mode);
        // m_output->generateSineWave();
        // }
        
        

        // unsigned long start_time = millis();
        // if(millis() - start_time < 500){
        
        // // for (int i = 0; i < 128; i++) {
        // //     Serial.println(samples[i]);
        // // }
        
        
        // }

        m_button->tick();
    }
}

// void tone_task(void *param){
//     int mode = 0;
//     while (true)
//     {
//         // mode = m_transport->getMode();
//         // m_output->i2sTone(mode);
//         // m_output->generateSineWave();
//         // vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }
