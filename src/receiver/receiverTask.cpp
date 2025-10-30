#include "receiverTask.h"
#include "config.h"

i2s_pin_config_t i2sPins = {
    .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
    .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_SPEAKER_SERIAL_DATA,
    .data_in_num = I2S_SPEAKER_SD_PIN
};
i2s_port_t i2sPort = I2S_NUM_0;

i2s_config_t i2s_sirine_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    .sample_rate = SAMPLE_RATE,
#else
    .sample_rate = (int)sample_rate,
#endif
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
#endif
  };

i2s_config_t i2s_speaker_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .sample_rate = SAMPLE_RATE,
#else
        .sample_rate = (int)sample_rate,
#endif
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
#endif
    };

uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};

receiverTask::receiverTask()
{
    _taskMutex = xSemaphoreCreateMutex(); 
    mOutput = new speaker(i2sPort,i2sPins,i2s_speaker_config,256);
    mSirine = new sirine(i2sPort,i2sPins,i2s_sirine_config);
    outBuffer = new Buffer(300 * 16);
    mMemory = new storage();
    mCommunication = new commEspNow(outBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);
    mButton = new button(BINDING_BUTTON); 
    
    samples = (int16_t *)malloc(sizeof(int16_t) * BYTE_RATE);
    if (!samples) {
        Serial.println("Error: Failed to allocate memory for samples");
    }

    siren = 0;
    mode = 0;
    isLoop = false;
    clickTimeout = 300;
    clickTime = 0;
    clickCount = 0;
    SirenModeClick = 0;
    volumeAudio = 0;
}

receiverTask::~receiverTask()
{
    if (mCommunication) delete mCommunication;
    if (mMemory) delete mMemory;
    if (mOutput) delete mOutput;
    if (mButton) delete mButton;
}

void receiverTask::begin(){
    if (I2S_SPEAKER_SD_PIN != -1)
    {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }
    
    delay(200);
    
    mMemory->init(); 
    volumeAudio = mMemory->getVolume();
    delay(100);

    mSirine->generateWaveTable();
    delay(50);
    
    delay(100);

    mButton->begin(); 
    outBuffer->flush();
}

void receiverTask::communication(){
    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
        return;
    }
    int _mode = 0;
    bool _isLoop = false;
    int _siren = 0;
    int _currentSiren = 0;
    int lastSiren = 0;
    int onPress = 0;
    int clickCount = 0;

    for (;;)
    {
        mButton->tick();

        if (mButton->getMode() == true){
            mCommunication->binding();
        }

        if (mButton->getRemove() == true) 
        {
            mMemory->deleteAddress(); 
        }

        if (mCommunication->getMode() != _mode)
        {
            _mode = mCommunication->getMode();
        }

        if (mCommunication->parsingData()) {
            _siren = mCommunication->getButtonValue();
            _isLoop = mCommunication->getBool();
        }

        if (_isLoop == true)
        {
            if (_siren != lastSiren && _siren < 63)
            {
                lastSiren = _siren;

                if (lastSiren >= 1)
                {
                    onPress = lastSiren;
                }else
                {
                    clickCount = (clickCount + 1) % 2;
                    if (clickCount == 1)
                    {
                        _currentSiren = onPress;
                    }else
                    {
                        _currentSiren = 0;
                    }
                } 
            }
        }else if (_isLoop == false)
        {
            _currentSiren = _siren;
            clickCount = 0;
            onPress = 0;
        }

        xSemaphoreTake(_taskMutex, portMAX_DELAY);
        mode = _mode;
        isLoop = _isLoop;
        siren = _currentSiren;
        xSemaphoreGive(_taskMutex);

        vTaskDelay(5);
    }
}

void receiverTask::clearSample(){
    if (samples) {
        free(samples);
    }
}

void receiverTask::processData(){
    const size_t SINE_BUFFER_SAMPLES = 256;
    sine_buffer = (int16_t *)malloc(sizeof(int16_t) * SINE_BUFFER_SAMPLES);
    if (!sine_buffer) {
        Serial.println("Error: Failed to allocate memory for sine_buffer");
    }
    int lastSiren = 0;
    int onPress = 0;
    int currentSiren = 0;
    int speakerMode = 0;
    bool currentLoop = false;
    int loopSiren = 0;
    while (true)
    {
        xSemaphoreTake(_taskMutex, portMAX_DELAY);
        speakerMode= mode;
        currentLoop = isLoop;
        currentSiren = siren;
        xSemaphoreGive(_taskMutex);

        unsigned long start_time = 0;
        
        switch (speakerMode) {
            case 1: {
                mSirine->startSirine();
                while (currentSiren >= 1 && currentSiren < 63) {
                    xSemaphoreTake(_taskMutex, portMAX_DELAY);
                    currentSiren = siren;
                    xSemaphoreGive(_taskMutex);
                    
                    mSirine->generateI2sTone(currentSiren);
                    mSirine->generateSineWave(volumeAudio);
                    if (currentSiren == 0) {
                        mSirine->generateI2sTone(0);
                        mSirine->cleanBuffer();
                        break;
                    }
                }
                mSirine->stopSirine();
                break;
            }case 2: {
                volumeAudio = mMemory->getVolume();
                Serial.printf("[DEBUG] Volume audio: %d\n", volumeAudio);
                break;
            }
            default: {
                if (!samples) {
                    Serial.println("Error: samples buffer is null");
                    return;
                }
                mOutput->startSpeaker(); 
                start_time = millis();
                while (millis() - start_time < 1000 || !outBuffer->getBuffer())
                {
                    if (I2S_SPEAKER_SD_PIN != -1)
                    {
                        digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
                    }
                    int volSpeaker = map(volumeAudio, 0, 90, 0, 8);
                    outBuffer->removeBuffer(samples, BYTE_RATE, volSpeaker);
                    mOutput->write(samples, BYTE_RATE);
                    if (I2S_SPEAKER_SD_PIN != -1)
                    {
                        digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
                    }
                    yield();
                }
                mOutput->stopAudio();
                break;
            }
        }
        vTaskDelay(5);
    }
    clearSample();
}
