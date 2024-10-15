#include <Arduino.h>
#include <driver/i2s.h>

// Pin configuration for INMP441
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_19
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

// I2S configuration
void setupI2SMicrophone() {
  // Configuration of I2S peripheral
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),  // Receive data
    .sample_rate = 16000,                               // Sampling rate
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,       // Bit depth
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,        // Use only left channel
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,           // Interrupt level
    .dma_buf_count = 8,                                 // Number of DMA buffers
    .dma_buf_len = 64                                   // Size of each buffer
  };

  // Pin configuration for I2S
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,    // Serial Clock (BCK)
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK, // Word Select (LRCLK)
    .data_out_num = I2S_PIN_NO_CHANGE, 
    .data_in_num = I2S_MIC_SERIAL_DATA    // Serial Data Input (DIN)
  };

  // Install and start the I2S driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_clk(I2S_NUM_0, 16000, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_MONO);
}

void setup() {
  Serial.begin(115200);
  Serial.println("testing");
  setupI2SMicrophone();
}

void loop() {
  int32_t buffer[64];  // Buffer to store incoming data
  size_t bytes_read;

  // Read data from I2S peripheral
  i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

  // Process the audio data here
  Serial.println("Audio data received!");
int sample = bytes_read/sizeof(uint32_t);
  // Example: print raw data for testing
  for (int i = 0; i < 64; i++) {
    Serial.println(buffer[i]);
  }

  delay(1000);  // Delay between reads for easier serial monitoring
}
