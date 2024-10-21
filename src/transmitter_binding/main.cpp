#include <esp_now.h>
#include <WiFi.h>
#include <driver/i2s.h>

// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 14

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

// Struktur data yang dikirim
typedef struct struct_message {
    char text[50];
    uint8_t audioData[256];
    size_t audioSize;
} struct_message;

struct_message message;
uint16_t audioBuffer[256]; // Buffer untuk data audio
uint8_t receiverMAC[] = {0xFC, 0xE8, 0xC0, 0x75, 0x02, 0x5C};


void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

    i2s_pin_config_t pin_config = {.bck_io_num = I2S_MIC_SERIAL_CLOCK,
                                 .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
                                 .data_out_num = I2S_PIN_NO_CHANGE,
                                 .data_in_num = I2S_MIC_SERIAL_DATA};

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);

    // Setup ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(onDataSent);

    // Add peer (receiver MAC address)
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6); // Masukkan alamat MAC receiver di sini
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Isi pesan teks
    strcpy(message.text, "hello world");

    // Baca data audio dan konversi ke uint8_t
    size_t bytesRead;
    i2s_read(I2S_NUM_0, audioBuffer, sizeof(audioBuffer), &bytesRead, portMAX_DELAY);
    for (int i = 0; i < bytesRead / 2; i++) {
        message.audioData[i] = (uint8_t)(audioBuffer[i] >> 8); // Konversi uint16_t ke uint8_t
    }
    message.audioSize = bytesRead / 2;

    // Kirim data menggunakan ESP-NOW
    esp_now_send(receiverMAC, (uint8_t *)&message, sizeof(message));
}

void loop() {
    // Kode loop kosong karena pengiriman dilakukan di setup()
}
