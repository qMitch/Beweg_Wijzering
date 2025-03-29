#include <unity.h>
#include "mcp_handler.h"

// 📌 **MCP23017 Pinnen (I2C via ESP32)**
#define MCP_SDA 18
#define MCP_SCL 17

void test_mcp_connection() {
    TEST_ASSERT_TRUE(initMCP(MCP_SDA , MCP_SCL)); // Moet waar zijn als de MCP23017 correct werkt
}

void test_mcp_binary_input() {
    int value = getBinaryInput();
    TEST_ASSERT_GREATER_OR_EQUAL(0, value);
    TEST_ASSERT_LESS_OR_EQUAL(255, value); // Waarde moet binnen 0-255 liggen
}

void test_mcp() {
    RUN_TEST(test_mcp_connection);
    RUN_TEST(test_mcp_binary_input);
}

