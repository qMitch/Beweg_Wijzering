#include "ethernet_handler.h"
#include <unity.h>

// 📌 **Ethernet W5500 SPI-pinnen**
#define W5500_MISO 38
#define W5500_MOSI 39
#define W5500_SCLK 40
#define W5500_CS 41

IPAddress testIP(192, 168, 2, 53);
IPAddress testGateway(192, 168, 2, 1);
IPAddress testSubnet(255, 255, 255, 0);

void test_ethernet_initialization() {
    TEST_ASSERT_TRUE(initEthernet(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS, testIP, testGateway, testSubnet));
}

void test_ethernet_server_status() {
    TEST_ASSERT_TRUE(server); // Moet waar zijn als de webserver correct is opgestart
}

void test_ethernet() {
    RUN_TEST(test_ethernet_initialization);
    RUN_TEST(test_ethernet_server_status);
}

