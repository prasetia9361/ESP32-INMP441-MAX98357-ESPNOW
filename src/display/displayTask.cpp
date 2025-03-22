#ifdef DISP
#include "displayTask.h"

uint8_t transport_header[TRANSPORT_HEADER_SIZE] = {};

displayTask::displayTask()
{
    m_screen = new Screen();
    // tft = new LGFX();
    m_memory = new memory(); 
    m_output_buffer = new audio(I2S_NUM_0,i2s_speaker_pins,128);
    m_communication = new Communication(m_output_buffer, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_communication->setHeader(TRANSPORT_HEADER_SIZE, transport_header);
}

displayTask::~displayTask()
{
}

void displayTask::begin(){
    Serial.println("Application started");

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    // Inisialisasi komunikasi dan komponen lain
    if (!m_communication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
    
    m_memory->init();
    m_screen->begin();

    Serial.println("Setup done");

    ui_init();
}

void displayTask::showDataReceive(){
    m_screen->lvHandler();
    ui_tick();
    
    // Tampilkan pesan yang diterima
    lv_label_set_text(objects.data_from_receiver, m_communication->getReceivedMessage());
}
void displayTask::binding(){
    // Proses binding jika diperlukan
    if (g_binding) {
        m_communication->statusBinding();
        g_binding = false;
    }
}

void displayTask::deleteAddress(){
    // Proses penghapusan alamat jika diperlukan
    if (g_delete) {
        m_memory->deleteAddress(); 
        g_delete = false;
    }
}

void displayTask::sendMassage(){
    // Proses pengiriman jika diperlukan
    if (g_sending) {
        m_communication->addPeer();
        m_communication->sendButton(1);
        g_sending = false;
    }
}
#endif