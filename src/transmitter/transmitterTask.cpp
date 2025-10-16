
#include "transmitterTask.h"
#include "config.h"
#include <esp_log.h>

static const char *TAG = "transmitterTask";

i2s_port_t i2sPort = I2S_NUM_0;
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
};
i2s_config_t i2s_mic_Config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 128,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};
// Definisi variabel global
uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};

transmitterTask::transmitterTask()
{
    mInput = new mic(i2sPort, i2s_mic_pins, i2s_mic_Config, BYTE_RATE);
    outBuffer = new Buffer(300 * 16);
    mMemory = new storage();
    mCommunication = new commEspNow(outBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);
    mButton = new button();
    samples = (int16_t *)malloc(sizeof(int16_t) * BYTE_RATE);
    memset(samples, 0, sizeof(int16_t) * BYTE_RATE);
    if (!samples) {
        ESP_LOGE(TAG, "Error: Failed to allocate memory for samples");
    }
}

transmitterTask::~transmitterTask()
{
    if (mCommunication) delete mCommunication;
    if (mMemory) delete mMemory;
    if (mInput) delete mInput;
    if (mButton) delete mButton;
}

void transmitterTask::begin(){
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    mMemory->init(); 
    
    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }

    mButton->begin();
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    Serial.println("Application started");
}

bool isToneValid(int key, const int* tones, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (key == tones[i]) {
            return true;
        }
    }
    return false;
}

void transmitterTask::trasnmitData(){
    mInput->configMic();
    // int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    for (;;)
    {
        if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
            mInput->startMic();
            unsigned long start_time = millis();
            while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                int samples_read = mInput->read(samples, BYTE_RATE);
    
                for (int i = 0; i < samples_read; i++) {
                    mCommunication->addSample(samples[i]);
                }
            }
            mCommunication->flush();
            mInput->stopAudio();
        }
    }
}

void transmitterTask::playButton() {
    if (digitalRead(GPIO_TRANSMIT_BUTTON)) {
        mButton->checkKey();
        int dataByte = mButton->getButton();

        if (dataByte != lastByte) {
            
            bool shouldSend = false;

            switch (dataByte) {
                case KEY_STATUS_BINDING:
                    mCommunication->binding();
                    shouldSend = true;
                    break;

                case KEY_DELETE_ADDRESS:
                    mMemory->deleteAddress();
                    shouldSend = true;
                    break;

                case KEY_RELEASED:
                    mCommunication->sendDataInt(dataByte, "remot");
                    shouldSend = true;
                    break;

                default:
                    const int modeTones[9] = {0,1,2,3,4,5,6,7,8};
                    if (isToneValid(dataByte, modeTones, 9)) {
                        mCommunication->sendDataInt(dataByte, "remot");
                        shouldSend = true;
                    }
                    break;
            }

            if (shouldSend) {
                lastByte = dataByte;
            }
            
        }
    }
}

void transmitterTask::communication(){
    for (;;)
    {
        if (mCommunication->addPeer()){
            playButton();
        }
        vTaskDelay(50);
    }
    
}