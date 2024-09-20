#include <WiFi.h>
#include <esp_now.h>
#include <SPIFFS.h>

// Pin untuk tombol
const int buttonPin = 0; // Sesuaikan dengan pin yang digunakan
unsigned long lastPress = 0;
const unsigned long debounce = 300;
int pressCount = 0;
volatile bool stateBinding = false;

// Variabel untuk menyimpan MAC Address
uint8_t receiverMAC[6];
uint8_t transmitterMAC[6];

// Semaphore untuk sinkronisasi antar core
SemaphoreHandle_t bindingSemaphore;

// Callback ketika data diterima
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.print("Data diterima dari: ");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(macStr);
  
  // Simpan MAC Address Receiver
  memcpy(receiverMAC, mac, 6);
  
  // Simpan ke SPIFFS
  // Periksa apakah file receiverMAC.bin sudah ada
  if (!SPIFFS.exists("/receiverMAC.bin")) {
    File file = SPIFFS.open("/receiverMAC.bin", FILE_WRITE);
    if (file) {
      file.write(receiverMAC, 6);
      file.close();
      Serial.println("Receiver MAC disimpan ke SPIFFS");
    } else {
      Serial.println("Gagal membuka file untuk menulis");
    }
  } else {
    Serial.println("File receiverMAC.bin sudah ada, tidak perlu menulis ulang");
  }
  
  // Cetak nilai dari incomingData di serial monitor
  Serial.print("Nilai incomingData: ");
  String receivedData = String((char*)incomingData);
  Serial.println(receivedData);
}

// Task untuk Core 0 (proses binding)
void bindingTask(void * parameter) {
  for(;;) {
    if (xSemaphoreTake(bindingSemaphore, portMAX_DELAY) == pdTRUE) {
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
            stateBinding = true;
            // Lakukan binding
            Serial.println("Proses Binding Dimulai...");
            // Mengirim broadcast untuk mencari receiver
            esp_now_peer_info_t peerInfo;
            memset(&peerInfo, 0, sizeof(peerInfo));
            peerInfo.channel = 0;
            peerInfo.encrypt = false;
            peerInfo.ifidx = WIFI_IF_STA;
            for (int i = 0; i < 6; i++) {
              peerInfo.peer_addr[i] = 0xFF;
            }
            
            if (esp_now_add_peer(&peerInfo) != ESP_OK) {
              Serial.println("Gagal menambahkan peer broadcast");
              continue;
            }
            
            // Kirim broadcast untuk meminta MAC Address
            esp_err_t result = esp_now_send(peerInfo.peer_addr, transmitterMAC, 6);
            if (result == ESP_OK) {
              Serial.println("Broadcast Transmitter MAC berhasil dikirim");
            } else {
              Serial.println("Gagal mengirim broadcast Transmitter MAC");
            }
            
            // Hapus peer broadcast setelah selesai mengirim
            if (esp_now_del_peer(peerInfo.peer_addr) == ESP_OK) {
              Serial.println("Peer broadcast berhasil dihapus");
            } else {
              Serial.println("Gagal menghapus peer broadcast");
            }
          }
        }
      }
      xSemaphoreGive(bindingSemaphore);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Task untuk Core 1 (esp_now_send saat SPIFFS exist)
void sendingTask(void * parameter) {
  for(;;) {
    if (SPIFFS.exists("/receiverMAC.bin")) {
      static unsigned long lastSend = 0;
      uint8_t macReciv[6];
      File file = SPIFFS.open("/receiverMAC.bin", FILE_READ);
      if (file) {
        file.read(macReciv, 6);
        file.close();
        Serial.print("Receiver MAC dimuat dari SPIFFS: ");
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 macReciv[0], macReciv[1], macReciv[2],
                 macReciv[3], macReciv[4], macReciv[5]);
        Serial.println(macStr);
        
        // Tambahkan Receiver sebagai peer
        esp_now_peer_info_t peerInfo;
        memset(&peerInfo, 0, sizeof(peerInfo));
        memcpy(peerInfo.peer_addr, macReciv, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        peerInfo.ifidx = WIFI_IF_STA; 
        
        if (!esp_now_is_peer_exist(macReciv)) {
          esp_err_t result = esp_now_add_peer(&peerInfo);
          if (result == ESP_OK) {
            Serial.println("Receiver ditambahkan sebagai peer");
          } else {
            Serial.print("Gagal menambahkan Receiver sebagai peer. Error code: ");
            Serial.println(result);
          }
        }

        unsigned long currentMillis = millis();
        if (currentMillis - lastSend >= 2000) {
          const char* message = "data_dari_transmiter";
          esp_err_t result = esp_now_send(macReciv, (const uint8_t*)message, strlen(message));
          if (result == ESP_OK) {
            Serial.println("Data berhasil dikirim");
          } else {
            Serial.println("Gagal mengirim data");
          }
          lastSend = currentMillis;
        }
      } else {
        Serial.println("Gagal membuka file MAC address");
      }
    } else {
      Serial.println("Menunggu proses binding...");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 detik sebelum mengecek lagi
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
  WiFi.disconnect(); 
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

  } else {
    
  }

  // Dapatkan MAC Address Transmitter
  memcpy(transmitterMAC, WiFi.macAddress().c_str(), 6);
  
  Serial.print("Transmitter MAC: ");
  Serial.println(WiFi.macAddress());

  // Inisialisasi semaphore
  bindingSemaphore = xSemaphoreCreateMutex();

  // Buat task untuk Core 0 dan Core 1
  xTaskCreatePinnedToCore(
    bindingTask,   /* Task function. */
    "BindingTask", /* name of task. */
    10000,         /* Stack size of task */
    NULL,          /* parameter of the task */
    1,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    0);            /* pin task to core 0 */

  xTaskCreatePinnedToCore(
    sendingTask,   /* Task function. */
    "SendingTask", /* name of task. */
    15000,         /* Stack size of task */
    NULL,          /* parameter of the task */
    1,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
}

void loop() {
  // Loop utama kosong karena semua pekerjaan dilakukan di task terpisah
}
