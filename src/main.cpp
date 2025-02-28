#include <string>
#include<Arduino.h>
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dragon.h"
#include "epd_highlevel.h"
#include "epdiy.h"
#include "55.h"
EpdiyHighlevelState hl;


#ifdef CONFIG_IDF_TARGET_ESP32
#define DEMO_BOARD epd_board_v6
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define DEMO_BOARD epd_board_v7
#endif


int read(EpdRect image_area,uint8_t *framebuffer,int name);
bool load_SD();


#define epd_poweron() digitalWrite(46,1)
#define epd_poweroff() digitalWrite(46,0)


const EpdDisplay_t ES120 = {
    .width = 2560,
    .height = 1600,
    .bus_width = 16,
    .bus_speed = 17,
    .default_waveform = &epdiy_ED047TC2,
    .display_type = DISPLAY_TYPE_GENERIC,
};

void idf_loop() {
    EpdRect dragon_area = {.x = 0, .y = 0, .width = 2560, .height = 1600};

    int temperature = 22;

    uint8_t* fb = epd_hl_get_framebuffer(&hl);
    epd_poweron();
    epd_fullclear(&hl, temperature);
    epd_hl_set_all_white(&hl);
    // epd_copy_to_framebuffer(dragon_area, a_data, fb);
    enum EpdDrawError _err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
    epd_poweroff();
    for(int i=0;i<104;i++){
    // vTaskDelay(5000);
      epd_fullclear(&hl, temperature);
      epd_poweron();
      read(dragon_area,fb,i);
      _err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
      epd_poweroff();
    }
    vTaskDelay(15000);
}

void idf_setup() {
    epd_init(&DEMO_BOARD, &ES120, EPD_LUT_64K);
    epd_set_vcom(1560);
    hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
    
}

#ifndef ARDUINO_ARCH_ESP32
void app_main() {
    idf_setup();

    while (1) {
        idf_loop();
    };
}
#endif
