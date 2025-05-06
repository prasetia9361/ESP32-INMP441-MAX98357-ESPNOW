#include "displayTask.h"
extern bool gBinding;
extern bool gSending;
extern bool gDelete;
extern bool gMain;
extern bool gSetting;
extern bool loadSetting;
extern int32_t vol;
// extern void set_var_load(int32_t value);
extern int32_t load_persentage;
int persent = 10;
// int32_t get_var_volume();
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
    static uint32_t lastUpdate = 0;
    while (load_persentage < 100)
    {
        mScreen->lvHandler();
        ui_tick();
        Serial.println(load_persentage);
        load_persentage += 10;
        delay(500);
    }
    
    // if(millis() - lastUpdate >= 1000 && load_persentage < 100) {
    //     mScreen->lvHandler();
    //     ui_tick();
    //     Serial.println(load_persentage);
    //     load_persentage += 10;
    //     lastUpdate = millis();
    // }
    // for (int i = 0; i <= 100; i += 10)
    // {
    //     load_persentage = i;
    //     Serial.print(load_persentage);
    //     delay(1000);
    // }
    // gMain = true;

    // lv_scr_load(objects.main);
    
    
    
}

void displayTask::showDataReceive(){
    // mScreen->lvHandler();
    // ui_tick();
    
    // // Tampilkan pesan yang diterima
    // lv_label_set_text(objects.data_from_receiver, mCommunication->getReceivedMessage());

    static String lastMessage = "";
    String currentMessage = mCommunication->getReceivedMessage();
    
    if(currentMessage != lastMessage) {
        lv_label_set_text(objects.data_from_receiver, currentMessage.c_str());
        lastMessage = currentMessage;
    }
    
    mScreen->lvHandler();
    ui_tick();

}

void displayTask::loadPage(){
    // static uint32_t lastUpdate = 0;
    // if(millis() - lastUpdate >= 1000 && load_persentage < 100) {
    //     Serial.println(load_persentage);
    //     load_persentage += 10;
    //     lastUpdate = millis();
    // }
    // else
    // {
    //     gMain = true;
    // }
    
    
    if(load_persentage >= 100 && !gMain) {
        // mScreen->lvHandler();
        // ui_tick();
        loadScreen(SCREEN_ID_HOME);
        // lv_scr_load(objects.home);
        load_persentage = 0;
    }
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
        mCommunication->sendButton(vol);
        String presentase = String(vol) + "%";
        // lv_label_set_text(objects.perentase, presentase.c_str());
        Serial.println(presentase);
        gSending = false;
    }
}

void displayTask::changeScreen(){
    if (gMain)
    {
        loadScreen(SCREEN_ID_HOME);
        // lv_scr_load(objects.home);
        gMain = false;
        Serial.println("home");
    }else if (loadSetting)
    {
        
        for (size_t i = 5; i > 0; i--)
        {
            loadScreen(SCREEN_ID_LOAD_SETTING);
        }
        Serial.println("load setting");
        
        loadSetting = false;
    } else if (gSetting)
    {
        loadScreen(SCREEN_ID_SETTING);
        Serial.println("setting");
        // lv_scr_load(objects.setting);
        gSetting = false;
    }
}
