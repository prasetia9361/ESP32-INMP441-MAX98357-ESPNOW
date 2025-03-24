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
}

void transmitterTask::processBinding(){
    if (digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // Mode tombol
        mButton->checkKey();
        dataByte = mButton->getButton();

        // Proses tombol binding (10)
        if (dataByte == 10 && dataByte != lastByte) {
            mCommunication->statusBinding();
            lastByte = dataByte;
        } 
        // Proses tombol hapus (11)
        else if (dataByte == 11 && dataByte != lastByte) {
            mMemory->deleteAddress();
            lastByte = dataByte;
        } 
        // Proses tombol normal
        else if (dataByte != 10 && dataByte != 11) {
            mCommunication->sendButton(dataByte);
            mButton->setButton();
        }
    }
}

void transmitterTask::trasnmitData(){
    if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // Mode transmisi audio
        Serial.println("Started transmitting");
        mInput->startMic(SAMPLE_RATE);

        // Transmisi audio selama tombol ditekan atau timeout
        unsigned long start_time = millis();
        while (millis() - start_time < 100 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
            int samples_read = mInput->read(samples, 128);

            // Kirim sampel yang dibaca
            for (int i = 0; i < samples_read; i++) {
                mCommunication->addSample(samples[i]);
            }
            
            // Beri kesempatan watchdog
            vTaskDelay(1);
        }

        // Kirim sisa sampel dalam buffer
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