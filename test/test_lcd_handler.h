#ifndef TEST_LCD_HANDLER_H
#define TEST_LCD_HANDLER_H

#include <Arduino.h>
#include <unity.h>
#include "lcd_handler.h"

// Declaraties van testfuncties
void test_init_lcd();
void test_render_bmp();
void test_render_jpg();
void test_display_selected_file();
void test_render_video_frame();
void test_play_video();

#endif