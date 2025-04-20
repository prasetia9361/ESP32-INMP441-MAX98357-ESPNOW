#include "displayTask.h"
extern bool gBinding;
extern bool gSending;
extern bool gDelete;
extern bool gMain;
extern bool gSetting;
uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};

displayTask::displayTask(){
    mScreen = new Screen();
    // tft = new LGFX();
    mMemory = new storage(); 
    mOutputBuffer = new Buffer(300 * 16);
    mCommunication = new commEspNow(mOutputBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);
}

displayTask::~displayTask()
{
}

void displayTask::begin(){
    Serial.println("Application started");

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    // Inisialisasi komunikasi dan komponen lain
    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
    
    mMemory->init();
    mScreen->begin();

    Serial.println("Setup done");

    ui_init();
}

void displayTask::showDataReceive(){
    mScreen->lvHandler();
    ui_tick();
    
    // Tampilkan pesan yang diterima
    lv_label_set_text(objects.data_from_receiver, mCommunication->getReceivedMessage());
}

void displayTask::binding(){
    // Proses binding jika diperlukan
    if (gBinding) {
        mCommunication->statusBinding();
        gBinding = false;
        Serial.println("binding");
    }
}

void displayTask::deleteAddress(){
    // Proses penghapusan alamat jika diperlukan
    if (gDelete) {
        mMemory->deleteAddress(); 
        gDelete = false;
    }
}

void displayTask::sendMassage(){
    // Proses pengiriman jika diperlukan
    if (gSending) {
        mCommunication->addPeer();
        mCommunication->sendButton(1);
        gSending = false;
    }
}

void displayTask::changeScreen(){
    if (gMain)
    {
        lv_scr_load(objects.main);
        gMain = false;
    }else if (gSetting)
    {
        lv_scr_load(objects.setting);
        gSetting = false;
    }
}
