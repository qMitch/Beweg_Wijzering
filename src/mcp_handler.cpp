#include "mcp_handler.h"


Adafruit_MCP23X17 mcp;

bool initMCP(int sda, int scl) {
    Wire.begin(sda, scl);
    Serial.println("MCP23017 initialiseren...");
    if (!mcp.begin_I2C()) {
        Serial.println("⛔ ERROR: MCP23017 niet gevonden!");
        mcpConnected = false;
        return false;
    }
    Serial.println("✅ MCP23017 verbonden!");
    mcpConnected = true;
    return true;
}

void configureMCPPins() {
    mcp.pinMode(MCP_PIN_ETHERNET, INPUT_PULLDOWN);
    for (int i = 0; i < 8; i++) {
        mcp.pinMode(MCP_PIN_START + i, INPUT_PULLDOWN);
    }
}

int getBinaryInput() {
    int binaryValue = 0;
    for (int i = 0; i < 8; i++) {
        int pinState = mcp.digitalRead(MCP_PIN_START + i);
        binaryValue |= (pinState << i);
    }
    Serial.print("🔢 Correcte binaire waarde: ");
    Serial.println(binaryValue);
    return binaryValue;
}