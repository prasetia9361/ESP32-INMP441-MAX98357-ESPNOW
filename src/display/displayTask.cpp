#include "displayTask.h"

bool whenLoop;

extern int32_t getVol();

extern const char *getAddress();

extern const char *getDevice1();

extern const char *getAddress2();

extern const char *getDevice2();

extern const char *getChoiseDevice();

extern bool switchLoop();

extern int32_t getSirenTone();

extern int32_t *getModes();

extern int getButton();

extern int currentState();

extern void setCurrentState(int state);

extern void setHidden(int button, bool hidden);

extern void setAddress(const char *data);

extern void setDevice1(const char *data);

extern void setAddress2(const char *data);

extern void setDevice2(const char *data);

extern void setVol(int32_t vol);   

extern void setModes(int32_t* arr, size_t len);

extern void setPlayButton(int button);

#ifndef TRANSPORT_HEADER_SIZE
#define TRANSPORT_HEADER_SIZE 8
#endif

uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};
int32_t clickCount = 0;

displayTask::displayTask(){
    mScreen = new Screen();
    mMemory = new storage();
    mOutputBuffer = new Buffer(300 * 16);
    mCommunication = new commEspNow(mOutputBuffer, mMemory, /*channel*/ 0);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);

    macAddress = nullptr;
    toneSelected = nullptr;
    latsVol = 0;
    memset(macStr, 0, sizeof(macStr));
}

displayTask::~displayTask()
{
    if (mCommunication) { delete mCommunication; mCommunication = nullptr; }
    if (mOutputBuffer) { delete mOutputBuffer; mOutputBuffer = nullptr; }
    if (mMemory) { delete mMemory; mMemory = nullptr; }
    if (mScreen) { delete mScreen; mScreen = nullptr; }
}

uint8_t* displayTask::convertTouint8t(int32_t* dataInt, size_t size){
    if (!dataInt || size == 0) return nullptr;
    uint8_t* result = new uint8_t[size];
    for (size_t i = 0; i < size; i++) {
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
        Serial.println("ERROR: Communication initialization failed!");
        return;
    }
    Serial.println("Communication initialized successfully");
    
    // Inisialisasi screen
    if (!mScreen->begin()) {
        Serial.println("ERROR: Screen initialization failed!");
        return;
    }
    Serial.println("Screen initialized successfully");
    
    // Baca konfigurasi tone dari storage
    toneSelected = mMemory->readModeTones();

    Serial.println("Setup done");

    // Inisialisasi UI (EEZ/LVGL)
    ui_init();
    Serial.println("UI initialized successfully");

    setVol( mMemory->getVolume());
    latsVol = getVol();

    if (toneSelected != nullptr) {
        setModes(toneSelected, 8);
    } else {
        int32_t defaultModes[8] = {0,0,0,0,0,0,0,0};
        setModes(defaultModes, 8);
    }

    if (mMemory->getMac() == nullptr || mMemory->getMac()[0] == 0) {
        setAddress("");
        setDevice1("");
        setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, true);
    } else {
        uint8_t* mac = mMemory->getMac();
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        setAddress(macStr);
        setDevice1(mMemory->device1());
        setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, false);
    }

    if (mMemory->getMac1() == nullptr || mMemory->getMac1()[0] == 0) {
        setAddress2("");
        setDevice2("");
        setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, true);
    } else {
        uint8_t* mac = mMemory->getMac1();
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        setAddress2(macStr);
        setDevice2(mMemory->device2());
        setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, false);
    }

    Serial.println("=== Display Task Setup Complete ===");
}

void displayTask::tick(){
    if (!mScreen) {
        Serial.println("ERROR: Screen object is null!");
        return;
    }

    mScreen->lvHandler();
    ui_tick();
}

void displayTask::updateData(){
    if (switchLoop() != whenLoop)
    {
        Serial.println(switchLoop());
        whenLoop = switchLoop();
    }
    int state = currentState();

    if (state != actionState_none)
    {
        Serial.println(state);
    }

    switch (state)
    {
    case actionState_binding: { // binding
        bool bindingResult = mCommunication->binding();
            
        if (getChoiseDevice() == mMemory->device1()) {
            Serial.println("device1");
            macAddress = mMemory->getMac();
        } else if (getChoiseDevice() == mMemory->device2()) {
            Serial.println("device2");
            macAddress = mMemory->getMac1();
        }
        setCurrentState(actionState_none);
        break;
    }

    case actionState_delete_address: { // delete
        bool deleteResult = mMemory->hapusAlamat(getChoiseDevice());
        if (deleteResult) {
            Serial.println("Alamat berhasil dihapus");
        } else {
            Serial.println("Alamat gagal dihapus");
        }
        setCurrentState(actionState_none);
        break;
    }

    case actionState_save_sirine: {
        if (macAddress != nullptr) {
            mCommunication->addPeer(macAddress);
            uint8_t* dataTones = convertTouint8t(getModes(), 8);
            if (dataTones) {
                mCommunication->sendSirineSetting(dataTones);
                delete[] dataTones;
            }
        }
        mMemory->writeMode(getModes(), 8);
        setCurrentState(actionState_none);
        break;
    }

    case actionState_send_volume: {
        if (macAddress != nullptr) {
            mCommunication->addPeer(macAddress);
            mCommunication->sendVolume(getVol());
        }
        mMemory->saveVolume(getVol());
        String presentase = String(getVol()) + "%";
        Serial.println(presentase);
        setCurrentState(actionState_none);
        break;
    }
    
    default:{
        if (mMemory->getMac() == nullptr || mMemory->getMac()[0] == 0) {
            setAddress("");
            setDevice1("");
            setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, true);
        } else {
            uint8_t* mac = mMemory->getMac();
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            setAddress(macStr);
            setDevice1(mMemory->device1());
            setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, false);
        }

        if (mMemory->getMac1() == nullptr || mMemory->getMac1()[0] == 0) {
            setAddress2("");
            setDevice2("");
            setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, true);
        } else {
            uint8_t* mac = mMemory->getMac1();
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            setAddress2(macStr);
            setDevice2(mMemory->device2());
            setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, false);
        }
        break;
    }
    }
}

void displayTask::testSiren(){

    if (whenLoop)
    {
        mCommunication->addPeer();
        if (getButton() == 1) {
            clickCount = (clickCount + 1) % 2;
            if (clickCount == 1) {
                mCommunication->sendButton(getSirenTone());
                Serial.println("button click on");
                Serial.print("play siren :");
                Serial.println(getSirenTone());
                setPlayButton(0);
            } else {
                Serial.println("button click off");
                mCommunication->sendButton(0);
                setPlayButton(0);
            }
        }else if (getButton() == 3)
        {
            clickCount = 0;
            mCommunication->sendButton(0);
        }
        
        
    }else
    {
        mCommunication->addPeer();
        if (getButton() == 2)
        {
            clickCount = 0;
            Serial.println("button press on");
            mCommunication->sendButton(getSirenTone());
            Serial.print("play siren :");
            Serial.println(getSirenTone());
            // currentState = actionState_none;
        }else
        {
            // clickCount = 0;
            mCommunication->sendButton(0);
            setPlayButton(0);
        }
    }
}