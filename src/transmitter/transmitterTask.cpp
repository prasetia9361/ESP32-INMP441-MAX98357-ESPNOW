#include "transmitterTask.h"

// Definisi variabel global
uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};

transmitterTask::transmitterTask()
{
    mInput = new mic(128);
    outBuffer = new Buffer(300 * 16);
    mMemory = new storage();
    mCommunication = new commEspNow(outBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);
    mButton = new button();
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
    
    // Inisialisasi komunikasi
    mMemory->init(); 

    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
    
    mButton->begin();
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);
    Serial.println("Application started");
    mCommunication->addPeer();
    // mCommunication->sendButton(0);
}

void transmitterTask::processBinding(){
    if (digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // Mode tombol
        mButton->checkKey(mMemory->readModeTones());
        dataByte = mButton->getButton();

        // Proses tombol binding (10)
        if (dataByte == -1 && dataByte != lastByte) {
            mCommunication->statusBinding();
            lastByte = dataByte;
        } 
        // Proses tombol hapus (11)
        else if (dataByte == -2 && dataByte != lastByte) {
            mMemory->deleteAddress();
            lastByte = dataByte;
        } 
        // Proses tombol normal
        else if (dataByte != -1 && dataByte != -2) {
            // mCommunication->addPeer();
            mCommunication->sendButton(dataByte);
            mButton->setButton();
        }
    }
}

void transmitterTask::trasnmitData(){
    mCommunication->addPeer();
    if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
        Serial.println("Started transmitting");
        mInput->startMic(SAMPLE_RATE);
        unsigned long start_time = millis();
        while (millis() - start_time < 100 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
            int samples_read = mInput->read(samples, 128);

            // Kirim sampel yang dibaca
            for (int i = 0; i < samples_read; i++) {
                // Serial.println(samples[i] >> 8);
                mCommunication->addSample(samples[i]);
            }
            
            // Tambahkan delay untuk beri waktu proses
            vTaskDelay(1);
        }

        mCommunication->flush();
        Serial.println("Finished transmitting");
        mInput->stopAudio();
    }
}

void transmitterTask::clearSample(){
    if (samples) {
        free(samples);
    }
} 