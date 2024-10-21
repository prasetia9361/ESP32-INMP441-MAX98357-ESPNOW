#include <esp_now.h>
#include <WiFi.h>
#include <driver/i2s.h>

// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 14

#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_18
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_5

// Struktur data yang diterima
typedef struct struct_message {
    char text[50];
    uint8_t audioData[256];
    size_t audioSize;
} struct_message;

struct_message incomingMessage;
uint16_t audioBuffer[256];

void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

    // Tampilkan pesan teks
    Serial.print("Received message: ");
    Serial.println(incomingMessage.text);

    // Konversi audio data kembali ke uint16_t dan kirim ke I2S
    for (int i = 0; i < incomingMessage.audioSize; i++) {
        audioBuffer[i] = (uint16_t)(incomingMessage.audioData[i] << 8); // Konversi kembali ke uint16_t
    }

    size_t bytesWritten;
    i2s_write(I2S_NUM_0, audioBuffer, incomingMessage.audioSize * sizeof(uint16_t), &bytesWritten, portMAX_DELAY);
}

void setup() {
    Serial.begin(115200);

    // Setup WiFi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Setup I2S
    i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
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
