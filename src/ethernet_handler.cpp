#include "ethernet_handler.h"


EthernetServer server(80);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
bool ethernetEnabled = false;

bool initEthernet(int sclk, int miso, int mosi, int cs, IPAddress ip, IPAddress gateway, IPAddress subnet) {
    Serial.println("🌐 Initialiseren van Ethernet...");

    SPI.begin(sclk, miso, mosi, cs);
    Ethernet.init(cs);

    if (Ethernet.begin(mac)) {
        Serial.print("✅ DHCP IP: ");
        Serial.println(Ethernet.localIP());
        ethernetEnabled = true;
    } else {
        Ethernet.begin(mac, ip, gateway, subnet);
        Serial.print("✅ Statisch IP: ");
        Serial.println(Ethernet.localIP());
        ethernetEnabled = true;
    }

    if (!ethernetEnabled) {
        Serial.println("⛔ ERROR: Ethernet kon niet worden geïnitialiseerd!");
        return false;  // ❌ Ethernet faalt
    }

    server.begin();
    Serial.println("🌍 Webserver gestart!");

    // ✅ Test of server draait
    if (!server) {
        Serial.println("⛔ ERROR: Webserver kon niet starten!");
        return false;
    }

    return true; // ✅ Alles correct gestart
}

void handleEthernetClient() {
    EthernetClient client = server.available();
    if (client) {
        Serial.println("📡 Nieuwe client verbonden!");
        String firstLine = "";
        while (client.available()) {
            char c = client.read();
            if (c == '\n') break;
            firstLine += c;
        }
        Serial.println("📡 Verzoek verwerkt: " + firstLine);
        
        handleClient(client, firstLine);
    }
}
