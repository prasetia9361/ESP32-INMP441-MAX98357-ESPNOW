#include <Arduino.h>

#include <WiFi.h>
#include <driver/i2s.h>
// #include <lvgl.h>
#include "./ui/ui.h"

// #include "lovyanGfxSetup.h"
#include "./screen/screen.h"
#include "EspNowTransport.h"
#include "OutputBuffer.h"
#include "config.h"
#include "memory.h"

Transport *m_transport;
OutputBuffer *m_output_buffer;
memory *m_memory; 
Screen *m_screen;

extern lv_event_t g_event_sending;
extern lv_event_t g_event_binding;
extern lv_event_t g_event_delete;
extern bool g_binding;
extern bool g_sending;
extern bool g_delete;

void application_task(void *param);


void setup()
{
    Serial.begin(115200);
    // start up the application
    m_output_buffer = new OutputBuffer(300 * 16);
    m_screen = new Screen();
    // tft = new LGFX();
    m_memory = new memory(); 
    m_transport = new EspNowTransport(m_output_buffer, m_memory, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
    Serial.println("Application started");

    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    m_transport->begin();
    m_memory->init();

    m_screen->begin();

    Serial.println("Setup done");

    ui_init();

    TaskHandle_t task_handler;
    xTaskCreate(application_task, "application_task", 16384, NULL, 4,
                &task_handler);
}

void loop()
{
    // application->tick();
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void application_task(void *param) {
    while (true) {
        // lv_timer_handler();
        m_screen->lvHandler();
        ui_tick();
        // m_transport->getChar();
        lv_label_set_text(objects.data_from_receiver, m_transport->getChar());


        if (g_binding){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_binding);
            // Serial.printf("Received event from obj: %u\n", obj);
            // Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            g_binding = false;
        }

        if (g_delete){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_delete);
            // Serial.printf("Received event from obj: %u\n", obj);
            m_memory->deleteAddress(); 
            g_delete = false;
        }

        if (g_sending){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_sending);
            // Serial.printf("Received event from obj: %u\n", obj);
            m_transport->peerReady();
            m_transport->sendChar('1');
            g_sending = false;
        }
  
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
