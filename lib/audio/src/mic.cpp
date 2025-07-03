#include "mic.h"
#include "soc/i2s_reg.h"
#include <esp_log.h>

mic::mic(int size){
    rawSamplesSize = size;
    micReadHead = 0;
    rawSamples = (int16_t *)malloc(sizeof(int16_t)* rawSamplesSize);
}

mic::~mic(){
    free(rawSamples);
}

void mic::startMic(uint16_t sampleRate){
i2s_config_t i2s_mic_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

    i2s_driver_install(i2sPort, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(i2sPort, &i2s_mic_pins);
    i2s_zero_dma_buffer(i2sPort);

    i2s_start(i2sPort);
}

int mic::read(int16_t *samples, int count){
    size_t bytes_read = 0;
    if (count > rawSamplesSize)
    {
        count = rawSamplesSize;
    }
    i2s_read(i2sPort, rawSamples, sizeof(int16_t) * count, &bytes_read, portMAX_DELAY);
    int samplesRead = bytes_read / sizeof(int16_t);
    
    for (int i = 0; i < samplesRead; i++)
    {
        // samples[i] = constrain(rawSamples[i], -INT16_MAX, INT16_MAX);
        int16_t temp = rawSamples[i] >> 5;
        samples[i] = (temp > INT16_MAX) ? INT16_MAX : (temp < -INT16_MAX) ? -INT16_MAX : (int16_t)temp;
    }
    return samplesRead;
}

// int mic::read(int16_t *samples, int count) {
//     size_t bytes_read = 0;
//     if (count > rawSampl\sSize) {
//         count = rawSamplesSize;
//     }
//     i2s_read(i2sPort, rawSamples, count, &bytes_read, portMAX_DELAY);
//     int samplesRead = bytes_read / sizeof(int16_t);

//     const int16_t lowThreshold = 5;   // Threshold untuk masuk ke zona 0
//     const int16_t highThreshold = 8;  // Threshold untuk keluar zona 0
//     static int16_t lastSample = 0;    // State tracking

//     for (int i = 0; i < samplesRead; i++) {
//         // Terapkan hysteresis noise gate
//         if (abs(rawSamples[i]) < lowThreshold) {
//             samples[i] = 0;
//         } else if (abs(rawSamples[i]) > highThreshold) {
//             samples[i] = constrain(rawSamples[i], -INT16_MAX, INT16_MAX);
//             lastSample = samples[i];
//         } else {
//             // Zona transisi: pertahankan state sebelumnya
//             samples[i] = lastSample;
//         }
//     }
//     return samplesRead;
// }

// size_t mic::read(int16_t *samples, int count) {
//     size_t bytes_read = 0;
//     if (count > rawSamplesSize) {
//         count = rawSamplesSize;
//     }
//     i2s_read(i2sPort, rawSamples, count, &bytes_read, portMAX_DELAY);
//     int samplesRead = bytes_read / sizeof(int16_t);
    
//     const int16_t noiseThreshold = 6;  // Threshold untuk noise suppression
    
//     for (int i = 0; i < samplesRead; i++) {
//         // Terapkan noise gate: nilai antara -threshold dan +threshold di-set ke 0
//         if (rawSamples[i] > -noiseThreshold && rawSamples[i] < noiseThreshold) {
//             samples[i] = 0;
//         } else {
//             samples[i] = constrain(rawSamples[i], -INT16_MAX, INT16_MAX);
//         }
//     }
//     return samplesRead;
// }

// int mic::read(int16_t *samples, int count) {
//     if (count > rawSamplesSize) {
//         count = rawSamplesSize;
//     }
    
//     size_t bytes_read = 0;
//     i2s_read(i2sPort, rawSamples, count, &bytes_read, portMAX_DELAY);
//     const int samplesRead = bytes_read / sizeof(int16_t);
    
//     const int16_t noiseThreshold = 6;
    
//     // Proses samples dalam batch untuk efisiensi cache
//     for (int i = 0; i < samplesRead; i++) {
//         const int16_t sample = rawSamples[i];
//         samples[i] = (abs(sample) < noiseThreshold) ? 0 : constrain(sample, -INT16_MAX, INT16_MAX);
//     }
    
//     return samplesRead;
// }

void mic::stopAudio(){
    i2s_stop(i2sPort);
    i2s_driver_uninstall(i2sPort);
}
