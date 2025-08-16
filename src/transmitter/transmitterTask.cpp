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

bool isToneValid(int key, const int* tones, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (key == tones[i]) {
            return true;
        }
    }
    return false;
}

void transmitterTask::processBinding() {
    if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
        return;
    }

    mButton->checkKey(mMemory->readModeTones());
    int dataByte = mButton->getButton();

    if (dataByte == lastByte) {
        return; 
    }

    bool shouldSend = false;

    switch (dataByte) {
        case KEY_STATUS_BINDING:
            mCommunication->statusBinding();
            shouldSend = true;
            break;

        case KEY_DELETE_ADDRESS:
            mMemory->deleteAddress();
            shouldSend = true;
            break;

        case KEY_RELEASED:
            mCommunication->sendButton(dataByte);
            shouldSend = true;
            break;

        default:
            const int* modeTones = mMemory->readModeTones();
            if (isToneValid(dataByte, modeTones, 8)) {
                mCommunication->sendButton(dataByte);
                shouldSend = true;
            }
            break;
    }

    if (shouldSend) {
        lastByte = dataByte;
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

            for (int i = 0; i < samples_read; i++) {
                // Serial.println(samples[i] >> 8);
                mCommunication->addSample(samples[i]);
            }
            
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