#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>

class OutputBuffer {
   private:
    uint8_t *m_buffer;

    SemaphoreHandle_t m_semaphore;

    int m_number_samples_to_buffer;
    int m_read_head;
    int m_write_head;
    int m_available_samples;
    int m_buffer_size;
    bool m_buffering;

   public:
    OutputBuffer(int number_samples_to_buffer) : m_number_samples_to_buffer(number_samples_to_buffer) {
        m_semaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(m_semaphore);

        m_read_head = 0;
        m_write_head = 0;
        m_available_samples = 0;
        m_buffering = true;
        m_buffer_size = 3 * number_samples_to_buffer;
        m_buffer = (uint8_t *)malloc(m_buffer_size);

        memset(m_buffer, 0, m_buffer_size);
        if (!m_buffer) {
            Serial.println("Failed to allocate buffer");
        }
    }

    void add_samples(const uint8_t *samples, int count) {
        xSemaphoreTake(m_semaphore, portMAX_DELAY);
        for (int i = 0; i < count; i++) {
            m_buffer[m_write_head] = samples[i];
            // Serial.println(m_buffer[m_write_head]);
            m_write_head = (m_write_head + 1) % m_buffer_size;
        }
        m_available_samples += count;
        xSemaphoreGive(m_semaphore);
    }

    void remove_samples(int16_t *samples, int count) {
        xSemaphoreTake(m_semaphore, portMAX_DELAY);
        for (int i = 0; i < count; i++) {
            samples[i] = 0;
            if (m_available_samples == 0 && !m_buffering) {
                m_buffering = true;
                samples[i] = 0;
                // Serial.println(m_buffering);
            }
            if (m_buffering && m_available_samples < m_number_samples_to_buffer) {
                samples[i] = 0;
                // m_buffering = true;
                // Serial.println(m_buffering);
            } else{
                m_buffering = false;
                uint8_t sample = m_buffer[m_read_head];
                
                // samples[i] = map(sample, 0, 255, -32768, 32767);
                samples[i] = ((int16_t)(sample - 127)) << 10;
                // Serial.println(m_buffering);
                // samples[i] = (int16_t)(sample - 127.0f) * 255.0f;
                // samples[i] = ((int16_t)(sample) << 8) | ((int16_t)(sample));
                
                m_read_head = (m_read_head + 1) % m_buffer_size;
                m_available_samples--;
            }
        }
        xSemaphoreGive(m_semaphore);
    }

    bool getBuffering(){return m_buffering;}
};
