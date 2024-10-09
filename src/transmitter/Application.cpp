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
    // delegate onto the application
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application()
{
    // TODO: Fix this mmake selft buffer
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

    // bring up WiFi
    WiFi.mode(WIFI_STA);

    // but don't connect if we're using ESP NOW
    WiFi.disconnect();

    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    // do any setup of the transport
    spiffs->init();
    spiffs->readMacAddress();
    m_transport->begin();
    
    // setup the transmit button
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    //setup button for binding mode
    pinMode(BINDING_BUTTON,INPUT_PULLUP);
    // start the main task for the application
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 10192, this, 1,
                &task_handle);
}
// application task - coordinates everything
void Application::loop()
{
    int16_t *samples =
        reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    // continue forever
    unsigned long lastPress = 0; // Ensure this is initialized
    int pressCount = 0; 
    unsigned long lastDebounceTime = 0;  // last time the button state changed
    int lastButtonState = HIGH;           // previous reading from the input pin
    int buttonState;                      // current reading from the input pin

    while (true) {
        // Read the state of the button
        int reading = digitalRead(BINDING_BUTTON);

        // Check if the button state has changed
        if (reading != lastButtonState) {
            lastDebounceTime = millis(); // reset the debouncing timer
        }

        // If the button state has been stable for longer than the debounce time
        if ((millis() - lastDebounceTime) > 50) {
            // If the button state has changed
            if (reading != buttonState) {
                buttonState = reading;
                // Only count the button press if it was pressed
                if (buttonState == LOW) {
                    currentTime = millis();
                    if (currentTime - lastPress > 300) {
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

        lastButtonState = reading; // save the reading for next time
        // char macStr[18];
        // snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", spiffs->getMac()[0], spiffs->getMac()[1], spiffs->getMac()[2], spiffs->getMac()[3], spiffs->getMac()[4], spiffs->getMac()[5]);
        // Serial.printf("mac address: %s\n", macStr);
        m_transport->peerReady();
        // do we need to start transmitting?
        if (!digitalRead(GPIO_TRANSMIT_BUTTON)){
            Serial.println("Started transmitting");
            
            // start the input to get samples from the microphone
            m_input->start();
            // transmit for at least 1 second or while the button is pushed
            unsigned long start_time = millis();
            while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON)){
                // read samples from the microphone
                int samples_read = m_input->read(samples, 128);
                // and send them over the transport
                // Serial.println(samples_read);
                for (int i = 0; i < samples_read; i++){
                    // Serial.println(samples_read);
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
    // free(samples);
}
