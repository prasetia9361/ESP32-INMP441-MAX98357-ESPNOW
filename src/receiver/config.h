#include <driver/gpio.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

// sample rate for the system
// #define SAMPLE_RATE 22050
#define SAMPLE_RATE 44100 // Microfone
// are you using an I2S microphone - comment this if you want to use an analog
// mic and ADC input #define USE_I2S_MIC_INPUT

// speaker settings
#define USE_I2S_SPEAKER_OUTPUT
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_18
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_5
// Shutdown line if you have this wired up or -1 if you don't
#define I2S_SPEAKER_SD_PIN GPIO_NUM_22

#define BINDING_BUTTON 0

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other
// applications or walkie talkie sets), specify TRANSPORT_HEADER_SIZE (the
// length in bytes of the header) in the next line, and define the transport
// header in config.cpp
#define TRANSPORT_HEADER_SIZE 0
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];

// i2s speaker pins
extern i2s_pin_config_t i2s_speaker_pins;
