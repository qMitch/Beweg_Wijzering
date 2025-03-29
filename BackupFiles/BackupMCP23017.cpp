
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;

void setup() {
    Serial.begin(115200);
    Wire.begin(5, 4);  // Pas aan naar jouw SDA/SCL pinnen

    if (!mcp.begin_I2C()) {
        Serial.println("MCP23017 niet gevonden!");
        while (1);
    }

    Serial.println("MCP23017 gevonden!");

    // Stel pin 0-3 in als input met pull-up
    for (int i = 0; i < 16; i++) {
        mcp.pinMode(i, INPUT_PULLDOWN);  // Interne pull-up aanzetten
    }

    Serial.println("Pinnen 0-3 ingesteld als INPUT_PULLUP");
}

void loop() {
    Serial.print("Lezen van MCP23017: ");
    
    for (int i = 0; i < 16; i++) {
        Serial.print("Pin ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(mcp.digitalRead(i));
        Serial.print("  ");
    }
    
    Serial.println();
    delay(1000);  // Wacht 1 seconde tussen metingen
}
