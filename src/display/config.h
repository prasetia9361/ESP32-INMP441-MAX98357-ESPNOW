#include <driver/gpio.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>


// transmit button
#define GPIO_TRANSMIT_BUTTON 4
#define BINDING_BUTTON 0

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now
// channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other
// applications or walkie talkie sets), specify TRANSPORT_HEADER_SIZE (the
// length in bytes of the header) in the next line, and define the transport
// header in config.cpp
#define TRANSPORT_HEADER_SIZE 10
extern uint8_t transport_header[TRANSPORT_HEADER_SIZE];
