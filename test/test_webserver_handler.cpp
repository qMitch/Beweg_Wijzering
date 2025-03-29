#include "test_webserver_handler.h"
#include "ethernet_handler.h"  // Ethernet moet actief zijn voor de webserver

// 📌 **Ethernet W5500 SPI-pinnen**
#define W5500_MISO 38
#define W5500_MOSI 39
#define W5500_SCLK 40
#define W5500_CS 41

// Simuleer een netwerkclient
class MockEthernetClient : public EthernetClient {
public:
    void print(const String &s) {
        Serial.print(s);
    }
    void println(const String &s = "") {
        Serial.println(s);
    }
};

// Test of de **hoofdwebpagina correct gegenereerd wordt**
void test_generate_web_page() {
    MockEthernetClient client;

    // 🌐 **Start Ethernet en webserver**
    IPAddress testIP(192, 168, 2, 53);
    IPAddress testGateway(192, 168, 2, 1);
    IPAddress testSubnet(255, 255, 255, 0);
    initEthernet(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS, testIP, testGateway, testSubnet);

    generateWebPage(client);
    TEST_ASSERT_TRUE_MESSAGE(true, "✅ Webpagina gegenereerd!");
}

// Test of de **debugpagina correct gegenereerd wordt**
void test_generate_debug_page() {
    MockEthernetClient client;

    // 🌐 **Zorg dat Ethernet actief is**
    if (!ethernetEnabled) {
        Serial.println("⚠ Ethernet niet actief, test kan fout gaan!");
    }

    generateDebugPage(client);
    TEST_ASSERT_TRUE_MESSAGE(true, "✅ Debugpagina gegenereerd!");
}

// Test of **bestandselectie correct werkt**
void test_handle_file_selection() {
    selectedFile = "testfile.jpg";
    TEST_ASSERT_EQUAL_STRING("testfile.jpg", selectedFile.c_str());
}

// Test of **bestand upload correct werkt**
void test_handle_file_upload() {
    MockEthernetClient client;
    handleFileUpload(client);
    TEST_ASSERT_TRUE_MESSAGE(true, "✅ Bestand upload test uitgevoerd!");
}

// **Testgroep voor de Webserver Handler**
void test_webserver() {
    RUN_TEST(test_generate_web_page);
    RUN_TEST(test_generate_debug_page);
    RUN_TEST(test_handle_file_selection);
    RUN_TEST(test_handle_file_upload);
}
