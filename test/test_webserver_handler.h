#ifndef TEST_WEBSERVER_HANDLER_H
#define TEST_WEBSERVER_HANDLER_H

#include <Arduino.h>
#include <unity.h>
#include "webserver_handler.h"

// Testfuncties declareren
void test_generate_web_page();
void test_generate_debug_page();
void test_handle_file_selection();
void test_handle_file_upload();
void test_webserver();

#endif // TEST_WEBSERVER_HANDLER_H
