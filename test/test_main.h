#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <unity.h>
// Inclusie van alle handlers
#include "test_ethernet_handler.h"
#include "test_mcp_handler.h"
#include "test_sd_handler.h"
#include "test_webserver_handler.h"
#include "test_lcd_handler.h"
#include "main.h"


// Declaraties van functies in main.cpp
void test_selection_source_switch();
void test_debug_mode_activation();
void test_selected_file_updates();

int mockgetBinaryInput();
void mockhandleEthernetClient();
String mockgetSelectedFile();
File SD_MMC_openMock(const String &filename);

void setup();
void loop();

#endif // MAIN_H
