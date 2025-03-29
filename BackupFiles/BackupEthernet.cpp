#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

#define W5500_MISO 38
#define W5500_MOSI 39
#define W5500_SCLK 40
#define W5500_CS 41 // Chip Select 

// MAC-adres (mag random zijn als je geen label hebt)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Statisch IP instellen (aanpassen aan jouw netwerk)
IPAddress ip(192, 168, 2, 53);
IPAddress gateway(192, 168, 2, 254);
IPAddress subnet(255, 255, 255, 0);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("ESP32-S3 W5500 SPI Instellen...");

    // SPI-bus handmatig instellen
    SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);
    Ethernet.init(W5500_CS); // W5500 gebruiken met eigen CS

    Serial.println("Ethernet wordt gestart...");
    if (Ethernet.begin(mac)) {
        Serial.print("DHCP IP: ");
        Serial.println(Ethernet.localIP());
    } else {
        Serial.println("DHCP faalde, proberen met statisch IP...");
        Ethernet.begin(mac, ip, gateway, subnet);
        Serial.print("Static IP: ");
        Serial.println(Ethernet.localIP());
    }

    Serial.println("Ethernet instellingen:");
    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet: ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(Ethernet.gatewayIP());
}

void loop() {
    Serial.println("Ethernet Test...");
    delay(2000);
}
