#include <Arduino.h>
#include <driver/gpio.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>

#include "Communication.h"
#include "audio.h"
#include "memory.h"
#include "button/button.h"



// sample rate for the system
// #define SAMPLE_RATE 22050
#define SAMPLE_RATE 44100 // Microfone
// are you using an I2S microphone - comment this if you want to use an analog
// mic and ADC input #define USE_I2S_MIC_INPUT

// speaker settings
#define USE_I2S_SPEAKER_OUTPUT
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_26
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_25
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_22
// Shutdown line if you have this wired up or -1 if you don't
#define I2S_SPEAKER_SD_PIN GPIO_NUM_5

#define BINDING_BUTTON 0

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

#define TRANSPORT_HEADER_SIZE 0
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];

// i2s speaker pins
extern i2s_pin_config_t i2s_speaker_pins;


class receiverTask
{
private:
    Communication *m_communication;
    memory *m_memory; 
    audio *m_output;
    button *m_button; 

    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    
    int mode = 0;

    unsigned long currentTime;
    volatile bool stateBinding = false;
public:
    receiverTask();
    ~receiverTask();
    void begin();
    void processBinding();
    void receiveData();
    void clearSample();
};