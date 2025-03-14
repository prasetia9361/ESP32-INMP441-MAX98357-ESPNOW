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

byte lastByte = 0;
bool lastButton = false;
unsigned long currentTime;

void application_task(void *param);



void setup()
{
    Serial.begin(115200);
    // start up the application
    m_output_buffer = new OutputBuffer(300 * 16);
    m_input = new audio(I2S_NUM_0, i2s_mic_pins, 128);
    m_memory = new memory();
    m_transport = new EspNowTransport(m_output_buffer, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7);
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    m_memory->init(); 
    // m_input->startMic(SAMPLE_RATE);
    m_transport->begin();
    m_button->begin();
    
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);

    Serial.println("Application started");
    m_transport->peerReady();

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 10192, NULL, 2,&task_handle);
    
    
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param){
    int16_t *samples =
        reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));

    int dataByte;
    const TickType_t xDelay = pdMS_TO_TICKS(5); // Delay kecil untuk memberi kesempatan task lain

    while (true) {
        // Beri kesempatan untuk task lain
        vTaskDelay(xDelay);
        
        if (digitalRead(GPIO_TRANSMIT_BUTTON)) {
            m_button->checkKey();
            dataByte = m_button->getButton();

            if (dataByte == 10 && dataByte != lastByte) {
                m_transport->statusBinding();
                lastByte = dataByte;
            } else if (dataByte == 11 && dataByte != lastByte) {
                m_memory->deleteAddress();
                lastByte = dataByte;
            } else if (dataByte != 10 && dataByte != 11) {
                m_transport->sendButton(dataByte);
                m_button->setButton();
            }
        } else {
            Serial.println("Started transmitting");
            m_input->startMic(SAMPLE_RATE);
  

            unsigned long start_time = millis();
            while (millis() - start_time < 100 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                // Tambahkan yield setiap beberapa sampel
                int samples_read = m_input->read(samples, 128);

                for (int i = 0; i < samples_read; i++) {
                    // Serial.println(samples[i]);
                    m_transport->add_sample(samples[i]);
                    
                    // // Setiap 32 sampel, beri kesempatan task lain
                    // if (i % 127 == 0) {
                    //     vTaskDelay(1);
                    // }
                }
                
                // Beri kesempatan watchdog
                vTaskDelay(1);
            }

            m_transport->flush();
            Serial.println("Finished transmitting");
            m_input->stopAudio();
        }
    }

    // Kode di bawah ini sebaiknya tidak dijalankan dalam loop normal
    if (samples != nullptr) {
        free(samples);
    }
}