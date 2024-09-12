#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define BUTTON_PIN 0  // Ganti dengan pin tombol yang sesuai

// Alamat MAC broadcast sementara
uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0xD2, 0x13, 0x4B};

bool buttonPressedTwice = false;
unsigned long lastPressTime = 0;
const int doublePressThreshold = 500;

void updatePeer(const uint8_t *_broadcastAddress) {
    // Hapus peer lama jika ada
    esp_now_del_peer(_broadcastAddress);

    // Setel ulang peer info
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, _broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Tambahkan peer baru
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\nLast Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                  : "Delivery Fail");
}

void receiveCallback(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Misalnya kita hanya perlu mengambil alamat MAC yang dikirimkan
    Serial.print("Received data from MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(data[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Simpan alamat MAC yang diterima ke SPIFFS
    // saveMacAddressToSPIFFS(mac_addr);

    // Update broadcastAddress dengan alamat MAC yang diterima
    memcpy(broadcastAddress, data, 6);
    updatePeer(broadcastAddress);
}

void setup() {
    // Inisialisasi serial monitor
    Serial.begin(115200);

    // Inisialisasi tombol input
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Set ESP32 sebagai Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Inisialisasi ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Set callback ketika data terkirim
    esp_now_register_send_cb(OnDataSent);

    // Set callback untuk menerima data
    esp_now_register_recv_cb(receiveCallback);

    // Register perangkat penerima
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Tambahkan peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void loop() {
    uint8_t address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    // Deteksi tombol ditekan dua kali
    if (digitalRead(BUTTON_PIN) == LOW) {
        unsigned long currentTime = millis();
        if (currentTime - lastPressTime < doublePressThreshold) {
            buttonPressedTwice = true;
        }
        lastPressTime = currentTime;
    }

    if (buttonPressedTwice) {
        // Update peer dengan alamat MAC yang baru
        memcpy(broadcastAddress, address, 6);
        updatePeer(broadcastAddress);
        buttonPressedTwice = false;  // Reset tombol tekan
    }

    if (broadcastAddress != address) {
        esp_err_t result = esp_now_send(broadcastAddress, broadcastAddress,
                                        sizeof(broadcastAddress));
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }
}
