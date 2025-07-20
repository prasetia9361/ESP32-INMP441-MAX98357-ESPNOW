#include "receiverTask.h"

uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};


receiverTask::receiverTask()
{
    mOutput = new speaker(128);// 256
    mSirine = new sirine();
    outBuffer = new Buffer(300 * 16);
    mMemory = new storage();
    mCommunication = new commEspNow(outBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);
    mButton = new button(BINDING_BUTTON); 
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

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    // Inisialisasi komunikasi dan komponen lain
    mMemory->init(); 

    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }

    mSirine->generateWaveTable();
    mOutput->startSpeaker(SAMPLE_RATE); 

    mButton->begin(); 
    outBuffer->flush();
    Serial.println("Application started");
}

void receiverTask::processBinding(){

    // Proses binding jika tombol double-click ditekan
    if (mButton->getMode()) 
    {
        Serial.println("Proses binding dimulai");
        mCommunication->statusBinding();
        mCommunication->setBinding(true);
        mButton->setMode(false); 
    }
    stateBinding = mCommunication->getBinding();

    //Proses penghapusan alamat jika tombol long-press ditekan
    if (mButton->getRemove()) 
    {
        mMemory->deleteAddress(); 
        mButton->setRemove(false); 
    }
}

// Dapatkan nilai tombol yang ditekan
void receiverTask::receiveData(){
    mode = mCommunication->getButtonValue();
    if (mode > 0)
    {
        // Mainkan nada berdasarkan nilai tombol
        mSirine->generateI2sTone(mode);
        mSirine->generateSineWave(mMemory->getVolume());
    }
    else
    {
        // Proses audio normal
        unsigned long start_time = millis();
        while (millis() - start_time < 100 || !outBuffer->getBuffer())
        {
            if (I2S_SPEAKER_SD_PIN != -1)
            {
                digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
            }
            volSpeaker = map(mMemory->getVolume(), 0, 99, 0, 10);
            outBuffer->removeBuffer(samples, 128, volSpeaker);
            mOutput->write(samples, 128);
            if (I2S_SPEAKER_SD_PIN != -1)
            {
                digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
            }
        }
    }

    mButton->tick();
}

// memberishkan data yang tersisia pada sample setelah nilai sample tidak digunakan
void receiverTask::clearSample(){
    if (samples) {
        free(samples);
    }
}