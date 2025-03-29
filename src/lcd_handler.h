#ifndef LCD_HANDLER_H
#define LCD_HANDLER_H

#include <LovyanGFX.hpp>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <vector>
#include <algorithm>
#include <JPEGDecoder.h>
#include "main.h"

void renderPhotoBMP(const char* filepath);
void renderVideoFrame(const char *filepath);
void playVideoBMP(const char* folder, int delayMs = 33);
void initLCD();
void displaySelectedFile(const String &filepath);
void renderPhotoJPG(const char *filepath);

class LGFX : public lgfx::LGFX_Device {
  // lgfx::Panel_ILI9488 _panel;
  lgfx::Panel_ILI9486 _panel;
  lgfx::Bus_Parallel8 _bus;
  lgfx::Light_PWM _light;

public:
  LGFX() {


    // Bus configuratie (8-bit parallel)
    auto cfg = _bus.config();
    cfg.freq_write = 20000000; //8000000
    cfg.pin_wr = 20;
    cfg.pin_rd = 19;
    cfg.pin_rs = 42;

    cfg.pin_d0 = 1;
    cfg.pin_d1 = 2;
    cfg.pin_d2 = 3;
    cfg.pin_d3 = 4;
    cfg.pin_d4 = 5;
    cfg.pin_d5 = 6;
    cfg.pin_d6 = 7;
    cfg.pin_d7 = 8;
    _bus.config(cfg);
    _panel.setBus(&_bus);

    // Panel configuratie
    auto panel_cfg = _panel.config();
    panel_cfg.pin_cs = 21;
    panel_cfg.pin_rst = 47;
    panel_cfg.pin_busy = -1;
    panel_cfg.memory_width = 320;
    panel_cfg.memory_height = 480;
    panel_cfg.panel_width = 320;
    panel_cfg.panel_height = 480;
    panel_cfg.offset_x = 0;
    panel_cfg.offset_y = 0;
    panel_cfg.offset_rotation = 0;
    panel_cfg.dummy_read_pixel = 8;
    panel_cfg.dummy_read_bits = 1;
    panel_cfg.readable = true;
    panel_cfg.invert = false;
    panel_cfg.rgb_order = false;
    panel_cfg.dlen_16bit = false;
    panel_cfg.bus_shared = false;
    _panel.config(panel_cfg);

    setPanel(&_panel);
  }
};

extern LGFX tft;


#endif