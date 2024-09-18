#include <WiFi.h>
#include <esp_now.h>
#include <SPIFFS.h>

// Pin untuk tombol
const int buttonPin = 0; // Sesuaikan dengan pin yang digunakan
unsigned long lastPress = 0;
const unsigned long debounce = 300;
int pressCount = 0;

// Variabel untuk menyimpan MAC Address
uint8_t receiverMAC[6];
uint8_t transmitterMAC[6];

// Callback ketika data diterima
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.print("Data diterima dari: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(macStr);
  
  // Simpan MAC Address Receiver
  memcpy(receiverMAC, incomingData, 6);
  
  // Simpan ke SPIFFS
  File file = SPIFFS.open("/receiverMAC.bin", FILE_WRITE);
  if (file) {
    file.write(receiverMAC, 6);
    file.close();
    Serial.println("Receiver MAC disimpan ke SPIFFS");
  } else {
    Serial.println("Gagal membuka file untuk menulis");
  }
  
  // Tambahkan Receiver sebagai peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0; // Sesuaikan channel jika diperlukan
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("Receiver ditambahkan sebagai peer");
  } else {
    Serial.println("Gagal menambahkan Receiver sebagai peer");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi tombol
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Inisialisasi SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS");
    return;
  }
  
  // Inisialisasi WiFi dalam mode Station
  WiFi.mode(WIFI_STA);
  Serial.println("Mode WiFi: STA");
  
  // Inisialisasi ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback
  esp_now_register_recv_cb(OnDataRecv);
  
  // Cek apakah Receiver MAC sudah ada di SPIFFS
  if (SPIFFS.exists("/receiverMAC.bin")) {
    File file = SPIFFS.open("/receiverMAC.bin", FILE_READ);
    if (file) {
      file.read(receiverMAC, 6);
      file.close();
      Serial.print("Receiver MAC dimuat dari SPIFFS: ");
      char macStr[18];
      snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
               receiverMAC[0], receiverMAC[1], receiverMAC[2],
               receiverMAC[3], receiverMAC[4], receiverMAC[5]);
      Serial.println(macStr);
      
      // Tambahkan Receiver sebagai peer
      esp_now_peer_info_t peerInfo;
      memcpy(peerInfo.peer_addr, receiverMAC, 6);
      peerInfo.channel = 0;
      peerInfo.encrypt = false;
      
      if (esp_now_add_peer(&peerInfo) == ESP_OK){
        Serial.println("Receiver ditambahkan sebagai peer dari SPIFFS");
      } else {
        Serial.println("Gagal menambahkan Receiver sebagai peer dari SPIFFS");
      }
    }
  } else {
    Serial.println("Receiver MAC belum terikat");
  }

  
  // Dapatkan MAC Address Transmitter
  memcpy(transmitterMAC, WiFi.macAddress().c_str(), 6);
  
  Serial.print("Transmitter MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) { // Asumsikan tombol terhubung ke GND
    unsigned long currentTime = millis();
    if (currentTime - lastPress > debounce) {
      pressCount++;
      lastPress = currentTime;
      Serial.print("Tombol ditekan: ");
      Serial.println(pressCount);
      
      if (pressCount == 2) {
        // Reset count
        pressCount = 0;
        
        // Lakukan binding
        Serial.println("Proses Binding Dimulai...");
        // Mengirim broadcast untuk mencari receiver
        esp_now_peer_info_t peerInfo;
        memset(&peerInfo, 0, sizeof(peerInfo));
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        peerInfo.ifidx = WIFI_IF_STA;
        peerInfo.peer_addr[0] = 0xFF;
        peerInfo.peer_addr[1] = 0xFF;
        peerInfo.peer_addr[2] = 0xFF;
        peerInfo.peer_addr[3] = 0xFF;
        peerInfo.peer_addr[4] = 0xFF;
        peerInfo.peer_addr[5] = 0xFF;
        
        esp_now_add_peer(&peerInfo);
        
        // Kirim broadcast untuk meminta MAC Address
        esp_now_send(peerInfo.peer_addr, transmitterMAC, 6);
        Serial.println("Broadcast Transmitter MAC dikirim");
      }
    }
  }
}
