// #include "Application.h"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "audio.h"
#include "OutputBuffer.h"
#include "config.h"
#include "spiffsHandler.h"
#include "button/button.h"

// our application
Transport *m_transport;
OutputBuffer *m_output_buffer;
spiffsHandler *spiffs;
audio *m_input; // Diubah dari _input menjadi m_input
button *m_button; // Diubah dari myButton menjadi m_button
unsigned long currentTime;

void application_task(void *param);

void setup()
{
    Serial.begin(115200);
    // start up the application
    m_output_buffer = new OutputBuffer(300 * 16);
    m_input = new audio(I2S_NUM_0, i2s_mic_pins, 128); // Diubah dari _input menjadi m_input
    spiffs = new spiffsHandler();  
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(BINDING_BUTTON); // Diubah dari myButton menjadi m_button
    
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    spiffs->init();
    m_transport->begin();
    m_button->begin(); // Diubah dari myButton menjadi m_button

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
        if (m_button->getMode()) // Diubah dari myButton menjadi m_button
        {
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            m_button->setMode(false); // Diubah dari myButton menjadi m_button
        }

        if (m_button->getRemove()) // Diubah dari myButton menjadi m_button
        {
            spiffs->deleteAddress();
            m_button->setRemove(false); // Diubah dari myButton menjadi m_button
        }
   
        m_transport->peerReady();

        if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
            Serial.println("Started transmitting");
            m_input->startMic(44100); // Diubah dari _input menjadi m_input

            unsigned long start_time = millis();
            while (millis() - start_time < 1000 ||
                   !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                int samples_read = m_input->read(samples, 128); // Diubah dari _input menjadi m_input

                for (int i = 0; i < samples_read; i++) {
                    m_transport->add_sample(samples[i]);
                }
            }

            m_transport->flush();
            Serial.println("Finished transmitting");
            m_input->stopAudio(); // Diubah dari _input menjadi m_input
        }
        m_button->tick(); // Diubah dari myButton menjadi m_button
    }
}