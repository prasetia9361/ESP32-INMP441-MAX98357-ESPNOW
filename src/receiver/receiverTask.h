#include <Arduino.h>
#include <driver/gpio.h>

#include "Communication.h"
#include "audio.h"
#include "storage.h"
#include "button/button.h"



// sample rate for the system
// #define SAMPLE_RATE 22050
#define SAMPLE_RATE 44100 

#define BINDING_BUTTON GPIO_NUM_0

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

#define TRANSPORT_HEADER_SIZE 0
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];

class receiverTask{
private:
    Communication *m_communication;
    storage *m_memory; 
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