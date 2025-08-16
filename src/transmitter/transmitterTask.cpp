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

// Definisikan konstanta untuk "magic numbers" agar kode lebih mudah dibaca

// Helper function untuk memeriksa apakah sebuah tombol adalah tone yang valid
// (Ini opsional, tapi membuat kode utama lebih bersih)
bool isToneValid(int key, const int* tones, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (key == tones[i]) {
            return true;
        }
    }
    return false;
}

void transmitterTask::processBinding() {
    // 1. Periksa kondisi utama (tombol transmit aktif)
    if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
        return; // Keluar lebih awal jika tombol tidak aktif
    }

    // 2. Baca nilai tombol
    mButton->checkKey(mMemory->readModeTones());
    int dataByte = mButton->getButton();

    // 3. Hanya proses jika nilai tombol BERUBAH
    if (dataByte == lastByte) {
        return; // Tidak ada perubahan, tidak perlu melakukan apa-apa
    }

    // 4. Proses nilai tombol yang baru menggunakan logika yang jelas
    bool shouldSend = false;

    switch (dataByte) {
        case KEY_STATUS_BINDING:
            mCommunication->statusBinding();
            shouldSend = true; // Anggap ini sebagai aksi yang valid
            break;

        case KEY_DELETE_ADDRESS:
            mMemory->deleteAddress();
            shouldSend = true; // Aksi yang valid
            break;

        case KEY_RELEASED:
            mCommunication->sendButton(dataByte); // Kirim status tombol dilepas (0)
            shouldSend = true;
            break;

        default:
            // Untuk semua nilai lainnya, periksa apakah itu adalah tone yang valid
            const int* modeTones = mMemory->readModeTones();
            if (isToneValid(dataByte, modeTones, 8)) {
                mCommunication->sendButton(dataByte);
                shouldSend = true;
            }
            break;
    }

    // 5. Perbarui lastByte HANYA JIKA ada aksi yang valid dilakukan
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