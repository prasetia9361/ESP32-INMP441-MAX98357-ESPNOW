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
    .dma_buf_count = 5,
    .dma_buf_len = 128,
    .use_apll = true,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

    i2s_driver_install(i2sPort, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(i2sPort, &i2s_mic_pins);
    i2s_zero_dma_buffer(i2sPort);

    i2s_start(i2sPort);
}

int mic::read(int16_t *samples, int count) {
    size_t bytes_read = 0;
    if (count > rawSamplesSize) {
        count = rawSamplesSize;
    }

    i2s_read(i2sPort, rawSamples, count, &bytes_read, portMAX_DELAY);
    int samplesRead = bytes_read / sizeof(int16_t);
    
    for (int i = 0; i < samplesRead; i++) {
        int16_t temp = (abs(rawSamples[i]) < noiseThreshold) ? 0 : rawSamples[i] << 8;
        samples[i] = constrain(temp, -INT16_MAX, INT16_MAX);
    }
    
    return samplesRead;
}

void mic::stopAudio(){
    i2s_stop(i2sPort);
    i2s_driver_uninstall(i2sPort);
}
