#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "I2SMEMSSampler.h"
#include "OutputBuffer.h"
#include "spiffs_handler.h"
#include "config.h"

static void application_task(void *param)
{
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application()
{
    m_output_buffer = new OutputBuffer(0);
    m_input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config, 128);
    spiffs = new spiffs_handler();
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
}

void Application::begin()
{
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    
    spiffs->init();
    m_transport->begin();
    
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    pinMode(BINDING_BUTTON,INPUT_PULLUP);
    
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 10192, this, 1,
                &task_handle);
}

void Application::loop()
{
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    unsigned long lastPress = 0; // Ensure this is initialized
    int pressCount = 0; 
    unsigned long lastDebounceTime = 0;  // last time the button state changed
    int lastButtonState = HIGH;           // previous reading from the input pin
    int buttonState;                      // current reading from the input pin

    while (true) {
        int reading = digitalRead(BINDING_BUTTON);

        if (reading != lastButtonState) {
            lastDebounceTime = millis(); // reset the debouncing timer
        }

        if ((millis() - lastDebounceTime) > 50) {
            if (reading != buttonState) {
                buttonState = reading;
                if (buttonState == LOW) {
                    currentTime = millis();
                    if (currentTime - lastPress > 200) {
                        pressCount++;
                        lastPress = currentTime;
                        Serial.print("tombol ditekan: ");
                        Serial.println(pressCount);
                        if (pressCount == 2) {
                            Serial.println("Proses binding dimulai");
                            m_transport->statusBinding();
                            pressCount = 0;
                        }
                    }
                }
            }
        }

        lastButtonState = reading;
        m_transport->peerReady();
        
        if (!digitalRead(GPIO_TRANSMIT_BUTTON)){
            Serial.println("Started transmitting");
            m_input->start();
            
            unsigned long start_time = millis();
            while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON)){
                int samples_read = m_input->read(samples, 128);
                for (int i = 0; i < samples_read; i++){
                    m_transport->add_sample(samples[i]);
                }
            }

            m_transport->flush();
            Serial.println("Finished transmitting");
            m_input->stop();
        }
    }
    // free(samples);
    delay(50);
}
