#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define BUTTON_PIN 0

uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0xD2, 0x13, 0x4B};
const uint8_t defaultAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bool buttonPressedTwice = false;
unsigned long lastPressTime = 0;
const unsigned long doublePressThreshold = 500;
const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;
int lastButtonState = HIGH;

bool isDefaultAddress(const uint8_t* addr) {
    return memcmp(addr, defaultAddress, 6) == 0;
}

void updatePeer(const uint8_t *_broadcastAddress) {
    esp_now_del_peer(_broadcastAddress);
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, _broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\nLast Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void receiveCallback(const uint8_t *mac_addr, const uint8_t *data, int len) {
    Serial.print("Received data from MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(data[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    memcpy(broadcastAddress, data, 6);
    updatePeer(broadcastAddress);
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(receiveCallback);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void loop() {
    int reading = digitalRead(BUTTON_PIN);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading == LOW && lastButtonState == HIGH) {
            unsigned long currentTime = millis();
            if (currentTime - lastPressTime < doublePressThreshold) {
                buttonPressedTwice = true;
            }
            lastPressTime = currentTime;
        }
    }

    lastButtonState = reading;

    if (buttonPressedTwice) {
        memcpy(broadcastAddress, defaultAddress, 6);
        updatePeer(broadcastAddress);
        buttonPressedTwice = false;
        Serial.println("Reset to default broadcast address");
    }

    if (!isDefaultAddress(broadcastAddress)) {
        esp_err_t result = esp_now_send(broadcastAddress, broadcastAddress, sizeof(broadcastAddress));
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }
    }
    
    delay(1000);
}