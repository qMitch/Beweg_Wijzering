#include "main.h"

// 📂 **Debug Status Variabelen**
String lastPlayedFile = "";
bool debugMode = false;
bool mcpConnected = false;
bool sdConnected = false;
String selectedFile = "";
String selectionSource = "Geen";  // "MCP23017" of "Webserver"


// 📌 **Ethernet W5500 SPI-pinnen**
#define W5500_MISO 38
#define W5500_MOSI 39
#define W5500_SCLK 40
#define W5500_CS 41

// 📌 **SDIO Pinnen**
#define SD_CARD_D0   14
#define SD_CARD_CMD  13
#define SD_CARD_CLK  12
#define SD_CARD_D3   11
#define SD_CARD_D2   10
#define SD_CARD_D1   9

// 📌 **MCP23017 Pinnen (I2C via ESP32)**
#define MCP_SDA 18
#define MCP_SCL 17
#define MCP_PIN_ETHERNET 0  // Ethernet aan/uit
#define MCP_PIN_START 1      // Eerste pin voor binaire input

// 📡 **Ethernet Configuratie Mama**
// IPAddress ip(192, 168, 2, 53);
// IPAddress gateway(192, 168, 2, 254);
// 📡 **Ethernet Configuratie Papa**
IPAddress ip(192, 168, 178, 53);
IPAddress gateway(192, 168, 178, 1);
// 📡 **Ethernet Configuratie Led-e-Lux**
// IPAddress ip(192, 168, 1, 98);
// IPAddress gateway(192, 168, 1, 200);

IPAddress subnet(255, 255, 255, 0);

#ifndef UNIT_TEST // Alleen uitvoeren als we NIET in testmodus zitten

void setup() {
    Serial.begin(115200);

    if (!initMCP(MCP_SDA, MCP_SCL)) {
        Serial.println("⛔ ERROR: MCP23017 niet gevonden!");
        return;
    }

    if (!initSDCard(SD_CARD_CLK, SD_CARD_CMD, SD_CARD_D0, SD_CARD_D1, SD_CARD_D2, SD_CARD_D3)) {
        Serial.println("⛔ ERROR: SD-Kaart niet gevonden!");
        return;
    }

    if (!initEthernet(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS, ip, gateway, subnet)) {
        Serial.println("⛔ ERROR: Server start niet op!");
        return;
    }

    initLCD();

    Serial.println("✅ Setup voltooid!");
}


void loop() {
    int binaryValue = getBinaryInput();
    bool ethernetEnabled = mcp.digitalRead(MCP_PIN_ETHERNET);

    Serial.print("📥 MCP23017 Pin Status: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(mcp.digitalRead(MCP_PIN_START + i));
    }
    Serial.print(" | Binaire waarde: ");
    Serial.println(binaryValue, BIN);

    Serial.print("🌐 Ethernet Pin Status (MCP Pin 0): ");
    Serial.println(ethernetEnabled ? "HIGH (Webserver Aan)" : "LOW (Geen Webserver indien selectie actief)");

    // Debug Mode controleren
    bool newDebugMode = (!ethernetEnabled && binaryValue == 0);
    if (newDebugMode != debugMode) {
        debugMode = newDebugMode;
        if (debugMode) {
            Serial.println("🛠 Debug Mode geactiveerd!");
        } else {
            Serial.println("✅ Webserver terug naar normale modus.");
        }
    }

    // Alleen verwerken als MCP de selectie doet

    if (selectionSource == "Webserver" && !selectedFile.isEmpty() && selectedFile != lastPlayedFile) {
    displaySelectedFile(selectedFile);
    lastPlayedFile = selectedFile;
    }

    String mcpSelectedFile = getSelectedFile();
    if (!mcpSelectedFile.isEmpty()) {
        if (selectionSource != "MCP") {
            Serial.println("🔄 MCP selectie overschrijft Webserver selectie");
            selectionSource = "MCP";
        }

        selectedFile = mcpSelectedFile;

        File testFile = SD_MMC.open("/" + mcpSelectedFile);
        if (!testFile) {
            Serial.println("⛔ Kan bestand niet openen: " + mcpSelectedFile);
        } else {
            if (selectedFile != lastPlayedFile) {
                displaySelectedFile(selectedFile);
                lastPlayedFile = selectedFile;
            }
            else if (selectedFile == mcpSelectedFile && testFile.isDirectory()) {
                Serial.println("🔁 Zelfde video geselecteerd, herstarten...");
                playVideoBMP(("/" + selectedFile).c_str());
            }
            testFile.close();
        }
    }
    else if (selectionSource != "Webserver") {
        Serial.println("❌ Geen geldig bestand geselecteerd via MCP23017.");
    }

    // Webserver geselecteerd bestand tonen op LCD als er geen MCP selectie is
    if (selectionSource == "Webserver" && !selectedFile.isEmpty() && selectedFile != lastPlayedFile) {
        displaySelectedFile(selectedFile);
        lastPlayedFile = selectedFile;
    }



    // Alleen webserver draaien als pin HIGH of debugMode actief is
    if (ethernetEnabled || debugMode) {
        handleEthernetClient();
    }
delay(0); //Voor Test cases Monitor
}

#endif // UNIT_TEST