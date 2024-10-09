#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "I2SOutput.h"
#include "OutputBuffer.h"
#include "spiffs_handler.h"
#include "config.h"

SemaphoreHandle_t bindingSemaphore;

static void application_task(void *param)
{
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application()
{
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new I2SOutput(I2S_NUM_0, i2s_speaker_pins);
    spiffs = new spiffs_handler();
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);

    if (I2S_SPEAKER_SD_PIN != -1)
    {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }
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

    // bindingSemaphore = xSemaphoreCreateMutex();
    pinMode(BINDING_BUTTON,INPUT_PULLUP);

    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, this, 1,
                &task_handle);
}

void Application::loop()
{
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    unsigned long lastPress = 0; // Ensure this is initialized
    int pressCount = 0;
    while (true)
    {
        m_transport->binding(stateBinding);
        Serial.println("Started Receiving");

        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
        }

        unsigned long start_time = millis();
        while (millis() - start_time < 1000)
        {
            int buttonState = digitalRead(BINDING_BUTTON);
            if (buttonState == LOW) {
                unsigned long currentTime = millis();
                if (currentTime - lastPress > 300) {
                    pressCount++;
                    lastPress = currentTime;
                    Serial.print("tombol ditekan: ");
                    Serial.println(pressCount);
                    if (pressCount == 2) {
                        Serial.println("Proses binding dimulai");
                        m_transport->statusBinding();
                        stateBinding = true;
                        pressCount = 0;
                    }
                }
            }

            m_output_buffer->remove_samples(samples, 128);
            m_output->write(samples, 128);
        }

        if (I2S_SPEAKER_SD_PIN != -1)
        {
            digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
        }
        Serial.println("Finished Receiving");
    }
}
