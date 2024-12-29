// #include "Application.h"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "audio.h"
#include "OutputBuffer.h"
#include "config.h"
#include "memory.h"
#include "button/button.h"
// our application
Transport *m_transport;
OutputBuffer *m_output_buffer;
memory *m_memory; 
audio *m_input; 
button *m_button;
// myButton 
// oneButton *myButton;
unsigned long currentTime;

void application_task(void *param);

byte lastByte = 0;
bool lastButton = false;

void setup()
{
    Serial.begin(115200);
    // start up the application
    m_output_buffer = new OutputBuffer(300 * 16);
    m_input = new audio(I2S_NUM_0, i2s_mic_pins, 128); 
    m_memory = new memory();  
    m_transport = new EspNowTransport(m_output_buffer, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(PIN_1, PIN_2, PIN_3, 15, 4, 22, 23);
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    m_memory->init(); 
    m_transport->begin();
    m_button->begin(); 
    
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);

    Serial.println("Application started");

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 10192, NULL, 1,
                &task_handle);
    
    
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param){
    int16_t *samples =
        reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    

    while (true) {
        m_button->checkKey();
        // Serial.println(m_button->getButton());

        byte dataByte = static_cast<byte>(m_button->getButton());
        
        if (dataByte == 66 && dataByte != lastByte) 
        {
            // Serial.println(dataByte);
            m_transport->statusBinding();
            lastByte = dataByte;
            // m_button->setButton();
            // m_button->setMode(false); 
        }else if (dataByte == 52 && dataByte != lastByte ) 
        {
            m_memory->deleteAddress();
            lastByte = dataByte;
            // m_button->setButton();
            // m_button->setRemove(false); 
        }else if (dataByte != lastByte && dataByte != 66 && dataByte != 52)
        {
            // Serial.println(m_button->getButton());
            m_transport->sendChar(m_button->getButton());
            lastByte = dataByte;
            // m_button->setButton();
        }

        // Serial.println(m_button->getAudio());
        // m_transport->peerReady();

        // if (m_button->getAudio() != lastButton)
        // {
        //     Serial.println("Started transmitting");
        //     m_input->startMic(44100); 

        //     unsigned long start_time = millis();
        //     while (millis() - start_time < 1000 || m_button->getAudio() == true) {
        //         int samples_read = m_input->read(samples, 128); 

        //         for (int i = 0; i < samples_read; i++) {
        //             // Serial.println(samples[i]);
        //             m_transport->add_sample(samples[i]);
        //             // m_transport->sendChar("esp32display");
        //         }
        //     }

        //     m_transport->flush();
        //     Serial.println("Finished transmitting");
        //     m_input->stopAudio(); 
        // }
        
//    Serial.println(!digitalRead(GPIO_TRANSMIT_BUTTON));
        m_transport->peerReady();

        if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
            Serial.println("Started transmitting");
            m_input->startMic(44100); 

            unsigned long start_time = millis();
            while (millis() - start_time < 1000 ||
                   !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                int samples_read = m_input->read(samples, 128); 

                for (int i = 0; i < samples_read; i++) {
                    Serial.println(samples[i]);
                    m_transport->add_sample(samples[i]);
                    // m_transport->sendChar("esp32display");
                }
            }

            m_transport->flush();
            Serial.println("Finished transmitting");
            m_input->stopAudio(); 
        }
        // m_button->ticks();
    }
}