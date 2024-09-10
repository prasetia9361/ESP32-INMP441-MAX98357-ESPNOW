#include <driver/gpio.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

// sample rate for the system
// #define SAMPLE_RATE 22050
#define SAMPLE_RATE 44100 // Microfone
// are you using an I2S microphone - comment this if you want to use an analog
// mic and ADC input #define USE_I2S_MIC_INPUT

// I2S Microphone Settings

// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or
// I2S_CHANNEL_FMT_ONLY_RIGHT Generally they will default to LEFT - but you may
// need to attach the L/R pin to GND
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7

// transmit button
#define GPIO_TRANSMIT_BUTTON 23


// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other
// applications or walkie talkie sets), specify TRANSPORT_HEADER_SIZE (the
// length in bytes of the header) in the next line, and define the transport
// header in config.cpp
#define TRANSPORT_HEADER_SIZE 10
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];

// i2s config for reading from of I2S
extern i2s_config_t i2s_mic_Config;
// i2s microphone pins
extern i2s_pin_config_t i2s_mic_pins;
