#include "displayTask.h"
extern bool gBinding;
extern bool gSending;
extern bool gDelete;
extern bool gMain;
extern bool gSetting;
extern bool gSirine;
extern bool loadSetting;
extern bool saveTone;
extern int32_t toneSelected[8];
extern int32_t vol;
extern const char *address;
extern const char *address2;
extern const char *device1;
extern const char *device2;
extern const char* message_notification;
extern bool notif_hidden;
extern char *nameDevice;
// extern void set_var_address(const char *value);
// extern void set_var_load(int32_t value);
extern int32_t load_persentage;
int persent = 10;
// int32_t get_var_volume();
uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};

lv_obj_t *obj; // untuk msg box

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

uint8_t* displayTask::convertTouint8t(int32_t* dataInt, size_t size){
    uint8_t* result = new uint8_t[size];
    for (size_t i = 0; i < size; i++)
    {
        result[i] = static_cast<uint8_t>(dataInt[i]);
    }
    return result;
}

void displayTask::begin(){
    Serial.println("Application started");

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    mMemory->init();
    // Inisialisasi komunikasi dan komponen lain
    if (!mCommunication->begin()) {
        Serial.println("Komunikasi gagal dimulai!");
    }
    
    
    for (size_t i = 0; i < 8; i++)
    {
        toneSelected[i] = mMemory->readModeTones()[i];
    }
    
    
    mScreen->begin();

    vol = mMemory->getVolume();

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
    
    // const char *lastMessage = "";
    
    if (mMemory->getMac() == nullptr || mMemory->getMac()[0] == 0) {
        address = "not device connect";
        device1 = "";
    } else {
        // address = mMemory->device1();
        uint8_t* mac = mMemory->getMac();
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        address = macStr;
        device1 = mMemory->device1();
    }

    if (mMemory->getMac1() == nullptr || mMemory->getMac1()[0] == 0) {
        address2 = "not device connect";
        device2 = "";
    } else {
        uint8_t* mac = mMemory->getMac1();
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        address2 = macStr;
        device2 = mMemory->device2();
    }
    // Serial.println(address);
    // if(address != lastMessage) {
    //     lv_label_set_text(objects.data_from_receiver, address);
    //     lastMessage = address;
    // }
    
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
    if (gBinding) {
        bool bindingResult = mCommunication->binding();
        
        if (nameDevice == "address0") {
            macAddress = mMemory->getMac();
        } else if (nameDevice == "address1") {
            macAddress = mMemory->getMac1();
        }
        Serial.println(nameDevice);
        
        message_notification = bindingResult ? "approved" : "failed";
        notif_hidden = true;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        notif_hidden = false;
        
        gBinding = false;
        Serial.println("binding completed");
    }
}

void displayTask::deleteAddress(){
    // Proses penghapusan alamat jika diperlukan
    if (gDelete) {
        // mMemory->deleteAddress(); 
        
        bool deleteResult = mMemory->hapusAlamat(nameDevice);

        // if (nameDevice == "address0") {
        //     macAddress = mMemory->getMac();
        // } else if (nameDevice == "address1") {
        //     macAddress = mMemory->getMac1();
        // }
        // Serial.println(nameDevice);

        message_notification = deleteResult ? "deleted" : "failed";
        // address = "not device connect";
        notif_hidden = true;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        notif_hidden = false;

        gDelete = false;
    }
}

void displayTask::sendVolume(){
    // Proses pengiriman jika diperlukan
    if (gSending) {
        // mCommunication->addPeer();
        // mCommunication->sendButton(vol);
        mCommunication->addPeer(macAddress);
        mCommunication->sendVolume(vol);
        mMemory->saveVolume(vol);
        String presentase = String(vol) + "%";
        // lv_label_set_text(objects.perentase, presentase.c_str());
        Serial.println(presentase);
        gSending = false;
    }
}

void displayTask::sendTones(){
    if (saveTone)
    {
        mCommunication->addPeer(macAddress);
        uint8_t* dataTones = convertTouint8t(toneSelected,8);
        mCommunication->sendSirineSetting(dataTones);
        mMemory->writeMode(toneSelected, 8);
        

        Serial.print("Nilai toneSelected: ");
        for (int i = 0; i < 8; i++) {
            Serial.print(dataTones[i]);
            if (i < 7) Serial.print(", ");
        }
        Serial.println();
        
        // Serial.println(toneSelected);
        // mMemory->writeMode()
        saveTone = false;
        delete[] dataTones;
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
        // Serial.println("setting");
        // lv_scr_load(objects.setting);
        gSetting = false;
    } else if(gSirine)
    {
        loadScreen(SCREEN_ID_SETTING_SIREN_PAGE);
        gSirine = false;
    }
}
