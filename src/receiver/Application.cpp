#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "I2SOutput.h"
#include "OutputBuffer.h"
#include "config.h"

static void application_task(void *param) {
    // delegate onto the application
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application() {
    m_output_buffer = new OutputBuffer(300 * 16);
    m_output = new I2SOutput(I2S_NUM_0, i2s_speaker_pins);

    m_transport = new EspNowTransport(m_output_buffer, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);

    if (I2S_SPEAKER_SD_PIN != -1) {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }
}

void Application::begin() {
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    // bring up WiFi
    WiFi.mode(WIFI_STA);

    // but don't connect if we're using ESP NOW
    WiFi.disconnect();

    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    // do any setup of the transport
    m_transport->begin();
    // start off with i2S output running
    m_output->start(SAMPLE_RATE);
    // start the main task for the application
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, this, 1,
                &task_handle);
}

// application task - coordinates everything
void Application::loop() {
    int16_t *samples =
        reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    // continue forever
    while (true) {
        Serial.println("Started Receiving");
        if (I2S_SPEAKER_SD_PIN != -1) {
            digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
        }
        unsigned long start_time = millis();
        while (millis() - start_time < 1000) {
            // read from the output buffer (which should be getting filled
            // by the transport)
            m_output_buffer->remove_samples(samples, 128);
            // and send the samples to the speaker
            m_output->write(samples, 128);
        }
        if (I2S_SPEAKER_SD_PIN != -1) {
            digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
        }
        Serial.println("Finished Receiving");
    }
}
