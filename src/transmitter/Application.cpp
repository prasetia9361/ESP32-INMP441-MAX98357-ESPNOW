#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include "EspNowTransport.h"
#include "I2SMEMSSampler.h"
#include "config.h"

static void application_task(void *param) {
    // delegate onto the application
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application() {
    m_input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config, 128);

    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
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
    // connected so show a solid green light
    // m_indicator_led->set_default_color(0x00ff00);
    // m_indicator_led->set_is_flashing(false, 0x00ff00);
    // setup the transmit button
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
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
        // do we need to start transmitting?
        if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
            Serial.println("Started transmitting");
            // start the input to get samples from the microphone
            m_input->start();
            // transmit for at least 1 second or while the button is pushed
            unsigned long start_time = millis();
            while (millis() - start_time < 1000 ||
                   !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                // read samples from the microphone
                int samples_read = m_input->read(samples, 128);
                // and send them over the transport
                // Serial.println(samples_read);
                for (int i = 0; i < samples_read; i++) {
                    // Serial.println(samples_read);
                    // Serial.println(samples[i]);
                    m_transport->add_sample(samples[i]);
                }
            }
            m_transport->flush();
            // finished transmitting stop the input and start the output
            Serial.println("Finished transmitting");
            // m_indicator_led->set_is_flashing(false, 0xff0000);
            m_input->stop();
        }
        // while the transmit button is not pushed and 1 second has not elapsed
    }
}
