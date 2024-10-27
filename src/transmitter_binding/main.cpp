#include <WiFi.h>
#include <driver/i2s.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <stdlib.h>
#include <stdint.h>

#define SAMPLE_RATE 44100
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
#define BINDING_BUTTON 0

typedef struct message
{
    uint8_t m_buffer[64];
    char pesan[12];
}message;
message myData;
// uint8_t *m_buffer = NULL;
int16_t *audioBuffer = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
unsigned long start_time = 0;
uint8_t macAddress[] = {0xFC, 0xE8, 0xC0, 0x75, 0x02, 0x5C};
int m_index = 0;
int m_header = 0;

void add_peer(){
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, macAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    if (!esp_now_is_peer_exist(macAddress))
    {
        esp_now_add_peer(&peerInfo);
    }
    
}

void setup(){
    Serial.begin(115200);
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    esp_err_t result = esp_now_init();
    if (result != ESP_OK)
    {
        Serial.println("initializing failed!!");
        return;
    }
    
    // i2s config for reading from I2S
    i2s_config_t i2s_mic_Config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_MIC_CHANNEL,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    // i2s microphone pins
    i2s_pin_config_t i2s_mic_pins = {.bck_io_num = I2S_MIC_SERIAL_CLOCK,
                                    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
                                    .data_out_num = I2S_PIN_NO_CHANGE,
                                    .data_in_num = I2S_MIC_SERIAL_DATA};

    i2s_driver_install(I2S_NUM_0, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
    // myData.m_buffer = (uint8_t *)malloc(250);
    // m_buffer = (uint8_t *)malloc(250);
}
void loop(){
    add_peer();
    size_t byte_size = 0;
    if (!digitalRead(GPIO_TRANSMIT_BUTTON))
    {
        start_time = millis();
        while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON))
        {
            i2s_read(I2S_NUM_0, audioBuffer, 128, &byte_size, portMAX_DELAY);
            int audioBuffer_size = byte_size / sizeof(int16_t);
            // Serial.println(audioBuffer_size);
            for (int i = 0; i < audioBuffer_size; i++)
            {
                // for (m_index = 0; m_index < 251; m_index++)
                // {
                // m_buffer[i] = (uint8_t)((audioBuffer[i] + 32768) >> 8);

                myData.m_buffer[i] = (uint8_t)((audioBuffer[i] + 32768) >> 8);
                Serial.println(myData.m_buffer[i]);
                // }
                
                // Serial.println(audioBuffer[audioBuffer_size]);
                // m_buffer[i] = (uint8_t)((audioBuffer[i] + 32768) >> 8);
                // Serial.println(m_buffer[i]);
                // m_index++;
                // if (m_index == 250)
                // {
                
                // Serial.println(sizeof(m_buffer));
                //     m_index = 0;
                // }
            }
            // myData.m_buffer = 128;
            esp_err_t result = esp_now_send(macAddress, (uint8_t*)&myData, sizeof(myData));
            // if (result == ESP_OK)
            // {
            //     Serial.println("success");
            // }else
            // {
            //     Serial.println("failed");
            // }
            
            
        }
    }
    

    // delay(50);
}