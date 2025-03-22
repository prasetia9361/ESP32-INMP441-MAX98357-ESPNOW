#include "transmitterTask.h"

// Definisi variabel global
uint8_t transport_header[TRANSPORT_HEADER_SIZE] = {};

// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
};

transmitterTask::transmitterTask()
{
    m_input = new audio(I2S_NUM_0, i2s_mic_pins, 128);
    m_memory = new storage();
    m_communication = new Communication(m_input, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_communication->setHeader(TRANSPORT_HEADER_SIZE, transport_header);
    m_button = new button(PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7);
}

transmitterTask::~transmitterTask()
{
    if (m_communication) delete m_communication;
    if (m_memory) delete m_memory;
    if (m_input) delete m_input;
    if (m_button) delete m_button;
}

void transmitterTask::begin(){
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    
    // Inisialisasi komunikasi
    m_memory->init(); 

    if (!m_communication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
    
    m_button->begin();
    
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);

    Serial.println("Application started");
    m_communication->addPeer();
}

void transmitterTask::processBinding(){
    if (digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // Mode tombol
        m_button->checkKey();
        dataByte = m_button->getButton();

        // Proses tombol binding (10)
        if (dataByte == 10 && dataByte != lastByte) {
            m_communication->statusBinding();
            lastByte = dataByte;
        } 
        // Proses tombol hapus (11)
        else if (dataByte == 11 && dataByte != lastByte) {
            m_memory->deleteAddress();
            lastByte = dataByte;
        } 
        // Proses tombol normal
        else if (dataByte != 10 && dataByte != 11) {
            m_communication->sendButton(dataByte);
            m_button->setButton();
        }
    }
}

void transmitterTask::trasnmitData(){
    if (!digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // Mode transmisi audio
        Serial.println("Started transmitting");
        m_input->startMic(SAMPLE_RATE);

        // Transmisi audio selama tombol ditekan atau timeout
        unsigned long start_time = millis();
        while (millis() - start_time < 100 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
            int samples_read = m_input->read(samples, 128);

            // Kirim sampel yang dibaca
            for (int i = 0; i < samples_read; i++) {
                m_communication->addSample(samples[i]);
            }
            
            // Beri kesempatan watchdog
            vTaskDelay(1);
        }

        // Kirim sisa sampel dalam buffer
        m_communication->flush();
        Serial.println("Finished transmitting");
        m_input->stopAudio();
    }
}

void transmitterTask::clearSample(){
    if (samples) {
        free(samples);
    }
} 