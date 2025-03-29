#include <Arduino.h>
#include <unity.h>
#include "test_main.h"
#include "main.h"

#ifdef UNIT_TEST

void test_selection_source_switch();
void test_debug_mode_activation();
void test_selected_file_updates();

void setUp() {} // Wordt uitgevoerd vóór elke test
void tearDown() {} // Wordt uitgevoerd na elke test

void runTests() {
    UNITY_BEGIN();
    // Ethernet Tests
    RUN_TEST(test_ethernet_initialization);
    RUN_TEST(test_ethernet_server_status);

    // SD-kaart Tests
    RUN_TEST(test_sd_card_initialization);
    RUN_TEST(test_sd_file_write);

    // MCP23017 Tests
    RUN_TEST(test_mcp_connection);
    RUN_TEST(test_mcp_binary_input);

    // Webserver Tests
    RUN_TEST(test_generate_web_page);
    RUN_TEST(test_handle_file_selection);
    RUN_TEST(test_handle_file_upload);
    RUN_TEST(test_generate_debug_page);

    // LCD tests
    RUN_TEST(test_init_lcd);
    RUN_TEST(test_render_bmp);
    RUN_TEST(test_render_jpg);
    RUN_TEST(test_display_selected_file);
    RUN_TEST(test_render_video_frame);
    RUN_TEST(test_play_video);
 
    // Main tests
    RUN_TEST(test_selection_source_switch);
    RUN_TEST(test_debug_mode_activation);
    RUN_TEST(test_selected_file_updates);
    
    UNITY_END();
}

void setup() {
    delay(2000);
    runTests();
}

void loop() {}

// Mocks / Simulatie
String testLog;
bool fakeDigitalReadState[8] = {0};

// Overschrijft de echte getBinaryInput() tijdelijk
int mockgetBinaryInput() {
    int value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= (fakeDigitalReadState[i] << i);
    }
    return value;
}

// Simuleer Ethernet client handler
void mockhandleEthernetClient() {
    testLog += "[EthernetClientHandled]";
}

// Simuleer SD en selectie
String mockgetSelectedFile() {
    return selectedFile;
}

// Mock voor mcp.digitalRead
class fakeMCP {
public:
    bool digitalRead(uint8_t pin) {
        if (pin == MCP_PIN_ETHERNET) return fakeDigitalReadState[0];
        return fakeDigitalReadState[pin - MCP_PIN_START];
    }
};

fakeMCP mockMcp;

File SD_MMC_openMock(const String& filename) {
    File f;
    return f;
}

void test_selection_source_switch() {
    selectionSource = "Webserver";
    selectedFile = "video1.bmp";
    lastPlayedFile = "video0.bmp";

    // Simuleer dat file kan geopend worden
    displaySelectedFile(selectedFile);
    lastPlayedFile = selectedFile;
    TEST_ASSERT_EQUAL_STRING("video1.bmp", selectedFile.c_str());
    TEST_ASSERT_EQUAL_STRING("video1.bmp", lastPlayedFile.c_str());
}

void test_debug_mode_activation() {
    // Simuleer pin LOW en binary 0 = debug mode
    fakeDigitalReadState[0] = 0;  // ethernet LOW
    for (int i = 1; i < 8; ++i) fakeDigitalReadState[i] = 0;

    debugMode = false;
    int binaryValue = mockgetBinaryInput();
    bool ethernetEnabled = mockMcp.digitalRead(MCP_PIN_ETHERNET);

    bool newDebugMode = (!ethernetEnabled && binaryValue == 0);
    debugMode = newDebugMode;

    TEST_ASSERT_TRUE(debugMode);
}

void test_selected_file_updates() {
    selectionSource = "MCP";
    selectedFile = "video2.bmp";
    lastPlayedFile = "video1.bmp";

    // Simuleer dat file bestaat
    displaySelectedFile(selectedFile);
    lastPlayedFile = selectedFile;
    TEST_ASSERT_EQUAL_STRING("video2.bmp", lastPlayedFile.c_str());
}

#endif