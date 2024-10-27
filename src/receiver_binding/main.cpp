#include <WiFi.h>
#include <driver/i2s.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <stdlib.h>
#include <stdint.h>
// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 14

#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_18
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_5

typedef struct message
{
    uint8_t m_buffer[64];
    char pesan[12];
}message;
message incomingMessage;

int16_t *audioBuffer = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
int m_header = 0;

void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

    // Tampilkan pesan teks
    // Serial.print("Received message: ");
    // Serial.println(incomingMessage.text);

    // Konversi audio data kembali ke uint16_t dan kirim ke I2S
    for (int i = 0; i < 64; i++) {
        audioBuffer[i] = (uint16_t)((incomingMessage.m_buffer[i] - 128) << 8); // Konversi kembali ke uint16_t
        Serial.println(audioBuffer[i]);
        // Serial.println(audioBuffer[i]);
    }
    // Serial.println(incomingMessage.m_buffer);

    // for (int i = 0; i < 64; i++){
    //     audioBuffer[i] = (int16_t)(incomingData[i] - 128) << 8;
    //     // Serial.println(audioBuffer[i]);
    // }
    

    // size_t bytesWritten;
    // i2s_write(I2S_NUM_0, audioBuffer, len * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
}

void setup() {
    Serial.begin(115200);

    // Setup WiFi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Setup I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .sample_rate = 44100,
#else
        .sample_rate = (int)44100,
#endif
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,  // Unused
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT   // Use bits per sample
#endif
    };



    // i2s_pin_config_t pin_config = {
    //     .bck_io_num = I2S_SCK,
    //     .ws_io_num = I2S_WS,
    //     .data_out_num = I2S_SD,
    //     .data_in_num = I2S_PIN_NO_CHANGE
    // };

    i2s_pin_config_t pin_config = { .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
                                    .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
                                    .data_out_num = I2S_SPEAKER_SERIAL_DATA,
                                    .data_in_num = I2S_PIN_NO_CHANGE};

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);

    i2s_start(I2S_NUM_0);
    // incomingMessage.m_buffer = (uint8_t *)malloc(250);
    // audioBuffer = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    // Setup ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(onDataReceived);
}

void loop() {
    // Kode loop kosong karena penerimaan dilakukan melalui callback
}
