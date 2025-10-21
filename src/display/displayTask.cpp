#include "displayTask.h"

bool whenLoop;

#include "../../lib/ui/eez-flow.h"
#include "Arduino.h"

using namespace eez;
using namespace eez::flow;

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
#define TRANSPORT_HEADER_SIZE 0
#endif

#define ESP_NOW_WIFI_CHANNEL 1

uint8_t transportHeader[TRANSPORT_HEADER_SIZE] = {};
// int32_t clickCount = 0;

displayTask::displayTask(){
    _smphr = xSemaphoreCreateMutex();
    _initSemaphore = xSemaphoreCreateBinary(); // Buat binary semaphore
    mScreen = new Screen();
    mMemory = new storage();
    mOutputBuffer = new Buffer(300 * 16);
    mCommunication = new commEspNow(mOutputBuffer, mMemory, ESP_NOW_WIFI_CHANNEL);
    mCommunication->setHeader(TRANSPORT_HEADER_SIZE, transportHeader);

    macAddress = nullptr;
    addr1 = nullptr;
    addr2 = nullptr;
    m_device1 = nullptr;
    m_device2 = nullptr;
    toneSelected = nullptr;
    _getMode = new int32_t[9];
    memset(_getMode, 0, sizeof(int32_t) * 9);
    // latsVol = 0;
    memset(macStr, 0, sizeof(macStr));
}

displayTask::~displayTask()
{
    if (_smphr) { vSemaphoreDelete(_smphr); _smphr = nullptr; }
    if (mCommunication) { delete mCommunication; mCommunication = nullptr; }
    if (mOutputBuffer) { delete mOutputBuffer; mOutputBuffer = nullptr; }
    if (mMemory) { delete mMemory; mMemory = nullptr; }
    if (mScreen) { delete mScreen; mScreen = nullptr; }
    if (_getMode) { delete[] _getMode; _getMode = nullptr; }
}

uint8_t* displayTask::convertTouint8t(int32_t* dataInt, size_t size){
    if (!dataInt || size == 0) return nullptr;
    uint8_t* result = new uint8_t[size];
    for (size_t i = 0; i < size; i++) {
        result[i] = static_cast<uint8_t>(dataInt[i]);
    }
    return result;
}

bool displayTask::initializeProcessData(){
    mMemory->init();
    
    // Inisialisasi komunikasi dan komponen lain
    if (!mCommunication->begin()) {
        Serial.println("ERROR: Communication initialization failed!");
        return false;
    }
    Serial.println("Communication initialized successfully");
    
    xSemaphoreTake(_smphr, portMAX_DELAY);
    toneSelected = mMemory->readModeTones();
    vol = mMemory->getVolume();
    addr1 = mMemory->getMac();
    addr2 = mMemory->getMac1();
    m_device1 = mMemory->device1();
    m_device2 = mMemory->device2();
    xSemaphoreGive(_smphr);

    xSemaphoreGive(_initSemaphore); // Beri sinyal bahwa inisialisasi data selesai

    return true;
}

bool displayTask::lvglInit(){
    xSemaphoreTake(_initSemaphore, portMAX_DELAY); // Tunggu sinyal dari initializeProcessData

    // Inisialisasi screen
    if (!mScreen->begin()) {
        Serial.println("ERROR: Screen initialization failed!");
        return false;
    }
    Serial.println("Screen initialized successfully");
    
    // Inisialisasi UI (EEZ/LVGL)
    ui_init();
    xSemaphoreTake(_smphr, portMAX_DELAY);
    setVol(vol);

    if (toneSelected != nullptr) {
        setModes(toneSelected, 9);
    } else {
        int32_t defaultModes[9] = {0,0,0,0,0,0,0,0,0};
        setModes(defaultModes, 9);
    }

    if (addr1 == nullptr) {
        setAddress("");
        setDevice1("");
        setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, true);
    } else {
        uint8_t* mac = addr1;
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        setAddress(macStr);
        setDevice1(m_device1);
        setHidden(FLOW_GLOBAL_VARIABLE_TOP_BUTTON_HIDDEN, false);
    }

    if (addr2 == nullptr) {
        setAddress2("");
        setDevice2("");
        setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, true);
    } else {
        uint8_t* mac = addr2;
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        setAddress2(macStr);
        setDevice2(m_device2);
        setHidden(FLOW_GLOBAL_VARIABLE_HIDDEN_MASSAGE, false);
    }
    xSemaphoreGive(_smphr);
    Serial.println("UI initialized successfully");

    return true;
}

bool displayTask::begin(){
    Serial.println("Application started");
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());
    return true;
}
// core 1
void displayTask::tick(){
    if (!mScreen) {
        Serial.println("ERROR: Screen object is null!");
        return;
    }

    mScreen->lvHandler();
    ui_tick();
}
// core 1
void displayTask::loop(){
    
    const char* choice = getChoiseDevice();
    if (deviceSelected && choice && strcmp(deviceSelected, choice) != 0)
    {
        deviceSelected = choice;
    } else if (!deviceSelected && choice) {
        deviceSelected = choice;
    }

    if (memcmp(_getMode, getModes(), 9 * sizeof(int32_t)) != 0)
    {
        for (size_t i = 0; i < 9; i++)
        {
            _getMode[i] = getModes()[i];
        }
        
    }
    if (vol != getVol()) {
        vol = getVol();
    }
    if (_switch != switchLoop()) {
        _switch = switchLoop();
    }

    if (_button != getButton()) {
        _button = getButton();
    }
    if (sirenTone != getSirenTone())
    {
        sirenTone = getSirenTone();
    }
    
    // --- LOGIKA STATE YANG DIPERBAIKI ---
    // int ui_state = currentState();
    // if (ui_state != actionState_none) {
    //     // Ada aksi baru dari UI
    //     xSemaphoreTake(_smphr, portMAX_DELAY);
    //     if (state == actionState_none) {
    //         // Jika tidak ada aksi yang sedang diproses, ambil aksi baru ini
    //         state = ui_state;
    //     }
    //     xSemaphoreGive(_smphr);
    //     // Langsung reset state di UI agar tidak memicu aksi yang sama berulang kali
    //     setCurrentState(actionState_none);
    // }
    // --- AKHIR PERBAIKAN ---

    if (state != currentState())
    {
        state = currentState();
    }
    


    if (!_switch)
    {
        if (_button != 2)
        {
            setPlayButton(0);
        }
    }
    
}
// core 0
void displayTask::processData(){
    // --- LOGIKA STATE YANG DIPERBAIKI ---
    if (state == actionState_none) {
        // Tidak ada yang perlu dilakukan
        return;
    }

    if (lastState != state)
    {
        lastState = state;
        switch (lastState)
        {
        case actionState_binding: { // binding
            bool bindingResult = mCommunication->binding();
            if (bindingResult) {
                if (strcmp(deviceSelected, m_device1) == 0) {
                    Serial.println("device1 selected for binding");
                    macAddress = mMemory->getMac();
                } else if (strcmp(deviceSelected, m_device2) == 0) {
                    Serial.println("device2 selected for binding");
                    macAddress = mMemory->getMac1();
                }
            } else {
                Serial.println("Binding failed");
            }
            break;
        }

        case actionState_delete_address: { // delete
            bool deleteResult = mMemory->hapusAlamat(deviceSelected);
            if (deleteResult) {
                Serial.println("Alamat berhasil dihapus");
            } else {
                Serial.println("Alamat gagal dihapus");
            }
            break;
        }

        case actionState_save_sirine: {
            if (macAddress != nullptr) {
                mCommunication->addPeer(macAddress);
                uint8_t* dataTones = convertTouint8t(_getMode, 9);
                if (dataTones) {
                    mCommunication->sendModeSiren(dataTones);
                    delete[] dataTones;
                }
            }
            mMemory->writeMode(_getMode, 9);
            break;
        }

        case actionState_send_volume: {
            if (macAddress != nullptr) {
                mCommunication->addPeer(macAddress);
                if (mCommunication->sendDataInt(vol,"vol") == 1)
                {
                    mMemory->saveVolume(vol);
                }
                
            }
            break;
        }
        
        default:
            // Aksi tidak diketahui, abaikan.
            break;
        
        }
    }
    



    // Setelah aksi selesai, reset state lokal
    // xSemaphoreTake(_smphr, portMAX_DELAY);
    // state = actionState_none;
    // xSemaphoreGive(_smphr);
    // --- AKHIR PERBAIKAN ---
}

void displayTask::testSiren(){
    if (macAddress != nullptr) {
        mCommunication->addPeer(macAddress);
        mCommunication->sendDataBool(_switch);
        if (_switch)
        {
            if (_button == 1) {
                mCommunication->sendDataInt(0,"test");
            } else if (_button == 2) {
                mCommunication->sendDataInt(sirenTone,"test");
            } else if (_button == 3) {
                mCommunication->sendDataInt(63,"test");
            }
            
        } else
        {
            if (_button == 2) {
                Serial.println("button press on");
                mCommunication->sendDataInt(sirenTone,"test");
                Serial.print("play siren :");
                Serial.println(sirenTone);
            } else {
                mCommunication->sendDataInt(0,"test");
            }
        }
    }
}
