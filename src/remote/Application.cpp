#include "Application.h"

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <lvgl.h>
#include <ui.h>

#include "lovyanGfxSetup.h"
#include "EspNowTransport.h"
#include "OutputBuffer.h"
#include "config.h"
#include "spiffsHandler.h"

#define TFT_HOR_RES SCREEN_WIDTH
#define TFT_VER_RES SCREEN_HEIGHT
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

LGFX tft;

extern lv_event_t g_event_sending;
extern lv_event_t g_event_binding;
extern lv_event_t g_event_delete;
extern bool g_binding;
extern bool g_sending;
extern bool g_delete;

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}
#endif

static void application_task(void *param) {
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application() {
    m_output_buffer = new OutputBuffer(300 * 16);
    // tft = new LGFX();
    spiffs = new spiffsHandler();  
    m_transport = new EspNowTransport(m_output_buffer, spiffs, ESP_NOW_WIFI_CHANNEL);
    m_transport->set_header(TRANSPORT_HEADER_SIZE, transport_header);
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = lv_area_get_width(area);   // Get width of the area to flush
    uint32_t h = lv_area_get_height(area);  // Get height of the area to flush

    Serial.printf("Flush Area: x1=%d, y1=%d, x2=%d, y2=%d, w=%d, h=%d\n", 
                  area->x1, area->y1, area->x2, area->y2, w, h);

    // Ensure the area is within the bounds of the screen
    if (area->x1 >= SCREEN_WIDTH || area->y1 >= SCREEN_HEIGHT) {
        lv_disp_flush_ready(disp); // If out of bounds, return immediately
        return;
    }

    // Clip the area to fit the screen resolution (if necessary)
    if (area->x2 >= SCREEN_WIDTH) {
        w = SCREEN_WIDTH - area->x1; // Adjust width if it exceeds screen bounds
    }

    if (area->y2 >= SCREEN_HEIGHT) {
        h = SCREEN_HEIGHT - area->y1; // Adjust height if it exceeds screen bounds
    }

    // Flush the pixels to the screen
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);  // Set the address window to the flush area
    tft.writePixels((lgfx::rgb565_t *)px_map, w * h);  // Write the pixels to the screen
    tft.endWrite();

    lv_disp_flush_ready(disp); // Notify LVGL that flushing is complete
}

/* Read the touchpad. */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
  else
  {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touchX;
    data->point.y = touchY;
    // x = touchX;
    // y = touchY;
#if 0
    Serial.printf("x: %03d, y: %03d\n", data->point.x, data->point.y);
#endif
  }
}

void Application::begin() {
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());

    m_transport->begin();
    spiffs->init();
    
    tft.begin();
    tft.setRotation(0);
    tft.setBrightness(255);

    lv_init();
    /* Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb((lv_tick_get_cb_t)millis);

      /* Create a display. */
    lv_color_t *disp_draw_buf = (lv_color_t *)heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (disp_draw_buf == NULL)
    {
        Serial.println("Failed");
        return;
    }
    
    lv_display_t *disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, disp_draw_buf, NULL, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Initialize the (dummy) input device driver. */
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    Serial.println("Setup done");

    ui_init();

    TaskHandle_t task_handler;
    xTaskCreate(application_task, "application_task", 16384, this, 4,
                &task_handler);
}

void Application::loop() {
    while (true) {
        lv_timer_handler();
        ui_tick();

        if (g_binding){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_binding);
            // Serial.printf("Received event from obj: %u\n", obj);
            Serial.println("Proses binding dimulai");
            m_transport->statusBinding();
            g_binding = false;
        }

        if (g_delete){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_delete);
            // Serial.printf("Received event from obj: %u\n", obj);
            spiffs->deleteAddress();
            g_delete = false;
        }

        if (g_sending){
            // lv_obj_t *obj = lv_event_get_target_obj(&g_event_sending);
            // Serial.printf("Received event from obj: %u\n", obj);
            m_transport->peerReady();
            m_transport->sendChar("esp32remote");
            g_sending = false;
        }
  
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}