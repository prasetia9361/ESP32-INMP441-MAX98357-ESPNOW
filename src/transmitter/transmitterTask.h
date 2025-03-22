#ifndef TRANSMITTER_TASK_H
#define TRANSMITTER_TASK_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>

#include "Communication.h"
#include "audio.h"
#include "storage.h"
#include "button/button.h"

// Konstanta dan konfigurasi yang dipindahkan dari config.h
#define SAMPLE_RATE 44100 // Microfone

// I2S Microphone Settings
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7

// transmit button
#define GPIO_TRANSMIT_BUTTON 5
//banjar (kabel kuning)
#define PIN_1 33
#define PIN_2 25
#define PIN_3 26
////baris (kabel biru)
#define PIN_4 15
#define PIN_5 4
#define PIN_6 22
#define PIN_7 23

// On which wifi channel (1-11) should ESP-Now transmit?
#define ESP_NOW_WIFI_CHANNEL 1

#define TRANSPORT_HEADER_SIZE 0

// Variabel yang dipindahkan dari config.cpp
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];

// i2s microphone pins
extern i2s_pin_config_t i2s_mic_pins;

class transmitterTask
{
private:
    Communication *m_communication;
    storage *m_memory;
    audio *m_input; 
    button *m_button;

    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));

    int dataByte;
    int lastByte = 0;
    const TickType_t xDelay = pdMS_TO_TICKS(5);
public:
    transmitterTask();
    ~transmitterTask();
    void begin();
    void processBinding();
    void trasnmitData();
    void clearSample();
};

#endif // TRANSMITTER_TASK_H
