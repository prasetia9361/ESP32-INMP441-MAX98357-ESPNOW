#include "receiverTask.h"

uint8_t transport_header[TRANSPORT_HEADER_SIZE] = {};


receiverTask::receiverTask()
{
    m_output = new audio(128);// 256
    m_memory = new storage();
    m_communication = new Communication(m_output, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_communication->setHeader(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(BINDING_BUTTON); 
}

receiverTask::~receiverTask()
{
        if (m_communication) delete m_communication;
        if (m_memory) delete m_memory;
        if (m_output) delete m_output;
        if (m_button) delete m_button;
}

void receiverTask::begin(){
    if (I2S_SPEAKER_SD_PIN != -1)
    {
        pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    }

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    // Inisialisasi komunikasi dan komponen lain
    m_memory->init(); 

    if (!m_communication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
   
    m_output->startSpeaker(SAMPLE_RATE); 

    m_button->begin(); 
    Serial.println("Application started");
}

void receiverTask::processBinding(){

    // Proses binding jika tombol double-click ditekan
    if (m_button->getMode()) 
    {
        Serial.println("Proses binding dimulai");
        m_communication->statusBinding();
        m_communication->setBinding(true);
        m_button->setMode(false); 
    }
    stateBinding = m_communication->getBinding();

    //Proses penghapusan alamat jika tombol long-press ditekan
    if (m_button->getRemove()) 
    {
        m_memory->deleteAddress(); 
        m_button->setRemove(false); 
    }
}

// Dapatkan nilai tombol yang ditekan
void receiverTask::receiveData(){
    mode = m_communication->getButtonValue();
    if (mode > 0)
    {
        // Mainkan nada berdasarkan nilai tombol
        m_output->i2sTone(mode);
        m_output->generateSineWave();
    }
    else
    {
        // Proses audio normal
        unsigned long start_time = millis();
        while (millis() - start_time < 100 || !m_output->getBuffer())
        {
            if (I2S_SPEAKER_SD_PIN != -1)
            {
                digitalWrite(I2S_SPEAKER_SD_PIN, HIGH);
            }

            m_output->removeBuffer(samples, 128);
            m_output->write(samples, 128);

            if (I2S_SPEAKER_SD_PIN != -1)
            {
                digitalWrite(I2S_SPEAKER_SD_PIN, LOW);
            }
        }
    }

    m_button->tick();
}

// memberishkan data yang tersisia pada sample setelah nilai sample tidak digunakan
void receiverTask::clearSample(){
    if (samples) {
        free(samples);
    }
}