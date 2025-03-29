#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "FS.h"
#include "SD_MMC.h"

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
#define MCP_SDA 5
#define MCP_SCL 4
#define MCP_PIN_ETHERNET 0  // Ethernet aan/uit
#define MCP_PIN_START 1      // Eerste pin voor binaire input

// 📡 **Ethernet Configuratie**
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 53);
IPAddress gateway(192, 168, 2, 254);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server(80);

// 🔌 **MCP23017 Instantie**
Adafruit_MCP23X17 mcp;

// 📂 **Debug Status Variabelen**
bool mcpConnected = false;
bool sdConnected = false;
bool debugMode = false;
String selectedFile = "";

String selectionSource = "Geen";  // "MCP23017" of "Webserver"

void printSelectedFile() {
    if (selectedFile.isEmpty()) {
        Serial.println("⚠ Geen bestand geselecteerd.");
    } else {
        Serial.print("✅ Huidig geselecteerd bestand: ");
        Serial.println(selectedFile);
    }
}

// 🛠 **SD-Kaart Initialiseren**
bool initSDCard() {
    Serial.println("SD-Kaart wordt geladen...");
    if (SD_MMC.begin("/sdcard", false, false, 40000)) {
        Serial.println("SD-Kaart correct geladen!");
        sdConnected = true;
        return true;
    }
    Serial.println("ERROR: SD-Kaart niet gevonden!");
    sdConnected = false;
    return false;
}

int getBinaryInput() {
    int binaryValue = 0;
    
    // ✅ **Correcte volgorde lezen**
    for (int i = 0; i < 8; i++) {
        int pinState = mcp.digitalRead(MCP_PIN_START + i);
        binaryValue |= (pinState << i);  // **Plaats de juiste bit op de juiste positie**
    }

    Serial.print("🔢 Correcte binaire waarde: ");
    Serial.println(binaryValue);

    return binaryValue;
}


void generateWebPage(EthernetClient &client) {
    Serial.println("🌍 Genereren van webpagina...");

    client.println("<html><head><title>ESP32-S3 Webserver</title></head><body>");
    client.println("<h1>ESP32-S3 Bestandsbeheer</h1>");

    // ✅ **Geselecteerd bestand tonen**
    client.println("<h2>Geselecteerd bestand:</h2>");
    if (selectedFile.isEmpty()) {
        client.println("<p><b>Geen bestand geselecteerd.</b></p>");
    } else {
        client.println("<p><b>" + selectedFile + "</b> (Geselecteerd via: " + selectionSource + ")</p>");
        client.println("<img src='/download?file=" + selectedFile + "' style='max-width:300px; display:block; margin-top:10px;'>");
    }

    // ✅ **Bestandenlijst genereren**
    client.println("<h2>Bestanden op de SD-kaart:</h2><ul>");
    File root = SD_MMC.open("/");
    if (!root) {
        Serial.println("⛔ ERROR: Kan SD-kaart directory niet openen!");
        client.println("<p>⚠ ERROR: Kan SD-kaart directory niet openen.</p>");
    } else {
        File file = root.openNextFile();
        while (file) {
            String filename = file.name();
            if (filename == "System Volume Information") {
                file = root.openNextFile();
                continue;
            }

            Serial.println("📄 Bestand gevonden: " + filename);
            client.print("<li>" + filename);
            client.print(" <a href='/select?file=" + filename + "'><button>Selecteer</button></a>");
            client.print(" <a href='/download?file=" + filename + "'><button>Download</button></a>");
            client.println("</li>");
            file = root.openNextFile();
        }
    }

    // ✅ **Upload Formulier**
    client.print("<h2>Upload een bestand naar de SD-kaart:</h2>");
    client.print("<form method='POST' action='/upload' enctype='multipart/form-data'>");
    client.print("<input type='file' name='uploadfile' id='fileInput'>");
    client.print("<input type='submit' value='Upload'>");
    client.print("<p id='fileName'>Geen bestand gekozen</p>");
    client.print("</form>");
    
    // ✅ **JavaScript voor bestandsnaamweergave**
    client.print("<script>");
    client.print("document.getElementById('fileInput').addEventListener('change', function() {");
    client.print("document.getElementById('fileName').innerText = this.files[0] ? this.files[0].name : 'Geen bestand gekozen';");
    client.print("});");
    client.print("</script>");
    client.print("</body></html>");

    client.println("</ul>");
    client.println("</body></html>");
}






// 📁 **Bestand Verzenden (Downloadfunctie)**
void serveFile(EthernetClient &client, String filename) {
    File file = SD_MMC.open(("/" + filename).c_str());
    if (!file) {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<h1>Bestand niet gevonden!</h1>");
        return;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/octet-stream");
    client.println("Content-Disposition: attachment; filename=" + filename);
    client.println();

    while (file.available()) {
        client.write(file.read());
    }
    file.close();
}




void handleFileUpload(EthernetClient &client) {
    Serial.println("📤 Bestand upload gestart...");

    // ✅ **Lees header en zoek naar de bestandsnaam**
    String filename = "/uploaded_file.txt"; // **Default naam**
    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.startsWith("Content-Disposition: form-data; name=\"uploadfile\"; filename=\"")) {
            int start = line.indexOf("filename=\"") + 10;
            int end = line.indexOf("\"", start);
            filename = "/" + line.substring(start, end);
            filename.trim();
            break;
        }
    }

    Serial.println("📁 Bestand opslaan als: " + filename);

    // ✅ **Open bestand op de SD-kaart**
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("⛔ ERROR: Kan bestand niet openen om te schrijven!");
        client.println("HTTP/1.1 500 Internal Server Error");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><body><h1>⛔ ERROR: Kan bestand niet openen!</h1></body></html>");
        return;
    }

    // ✅ **Schrijf de inhoud naar de SD-kaart**
    bool headerEndFound = false;
    while (client.available()) {
        char c = client.read();
        if (headerEndFound) {
            file.write(c);
        } else if (c == '\n') {
            headerEndFound = true;
        }
    }
    file.close();
    Serial.println("✅ Bestand succesvol opgeslagen op de SD-kaart als: " + filename);

    // ✅ **Redirect gebruiker naar hoofdpagina**
    client.println("HTTP/1.1 303 See Other");
    client.println("Location: /");
    client.println();
}





void handleClient(EthernetClient &client, String request) {
    if (!client) return;

    Serial.println("📡 Volledige request: " + request);

    // ✅ **Bestand Downloaden via Webserver**
    if (request.indexOf("GET /download?file=") >= 0) {
        Serial.println("✅ DOWNLOAD request gedetecteerd!");

        int start = request.indexOf("file=") + 5;
        int end = request.indexOf(" ", start);
        if (end == -1) end = request.length();

        String requestedFile = request.substring(start, end);
        requestedFile.trim();

        Serial.println("📡 Bestandsdownload verzoek ontvangen: " + requestedFile);

        serveFile(client, requestedFile);
        return;
    }

    // ✅ **Check of het een bestandselectie is**
    if (request.indexOf("GET /select?file=") >= 0) {
        Serial.println("✅ SELECT request gedetecteerd!");

        int start = request.indexOf("file=") + 5;
        int end = request.indexOf(" ", start);
        if (end == -1) end = request.length();

        String requestedFile = request.substring(start, end);
        requestedFile.trim();

        Serial.println("📡 Extracted bestandsnaam: [" + requestedFile + "]");

        int binaryValue = getBinaryInput();
        if (binaryValue == 0) {
            selectedFile = requestedFile;
            selectionSource = "Webserver";
            Serial.println("✅ Bestand succesvol geselecteerd via Webserver: " + selectedFile);
        } else {
            Serial.println("❌ Webserver mag geen bestand selecteren, MCP heeft prioriteit.");
        }

        client.println("HTTP/1.1 303 See Other");
        client.println("Location: /");
        client.println();
        return;
    }

    // ✅ **Check of "POST /upload" aanwezig is in de request**
    if (request.indexOf("POST /upload") >= 0) {
        Serial.println("✅ UPLOAD verzoek gedetecteerd!");
        handleFileUpload(client);
        return;
    }

    // ✅ **Verstuur de webpagina**
    Serial.println("🌍 Genereren van webpagina...");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    generateWebPage(client);

    delay(10);
    client.stop();
    Serial.println("✅ Webpagina verzonden!");
}








String getSelectedFile() {
    int binaryValue = getBinaryInput();

    // ✅ **Als er via MCP23017 een bestand is geselecteerd**
    if (binaryValue > 0) {
        char filenamePrefix[20];
        sprintf(filenamePrefix, "%03d_", binaryValue);

        Serial.println("🔍 Zoeken naar bestand beginnend met: " + String(filenamePrefix));

        File root = SD_MMC.open("/");
        if (!root) {
            Serial.println("⛔ ERROR: Kan de SD-kaart directory niet openen!");
            return "";
        }

        bool fileFound = false;
        File file = root.openNextFile();
        while (file) {
            String name = file.name();
            if (name == "System Volume Information") {
                file = root.openNextFile();
                continue;
            }

            Serial.println("📄 Bestand gevonden: " + name);
            if (name.startsWith(filenamePrefix)) {
                Serial.println("✅ MCP23017 heeft bestand geselecteerd: " + name);
                return name;
            }
            file = root.openNextFile();
        }

        Serial.println("⛔ Geen bestand gevonden met prefix: " + String(filenamePrefix));
        return "";
    }

    // ✅ **Als er via de webpagina een bestand is geselecteerd**
    if (!selectedFile.isEmpty()) {
        Serial.println("✅ Webserver heeft bestand geselecteerd: " + selectedFile);
        return selectedFile;
    }

    Serial.println("⛔ Geen bestand geselecteerd.");
    return "";
}







// 🛠 **Setup**
void setup() {
    Serial.begin(115200);
    Wire.begin(MCP_SDA, MCP_SCL);
    if (!SD_MMC.setPins(SD_CARD_CLK, SD_CARD_CMD, SD_CARD_D0, SD_CARD_D1, SD_CARD_D2, SD_CARD_D3)) {
        Serial.println("⛔ ERROR: Kan SD-Kaart pinnen niet instellen!");
        return;
    }
    Serial.println("MCP23017 initialiseren...");
    if (!mcp.begin_I2C()) {
        Serial.println("ERROR: MCP23017 niet gevonden!");
        mcpConnected = false;
    } else {
        Serial.println("MCP23017 verbonden!");
        mcpConnected = true;
    }

    mcp.pinMode(MCP_PIN_ETHERNET, INPUT_PULLDOWN);
    for (int i = 0; i < 8; i++) {
        mcp.pinMode(MCP_PIN_START + i, INPUT_PULLDOWN);
    }

    initSDCard();

    SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);
    Ethernet.init(W5500_CS);
    
    if (Ethernet.begin(mac)) {
        Serial.print("DHCP IP: ");
        Serial.println(Ethernet.localIP());
    } else {
        Ethernet.begin(mac, ip, gateway, subnet);
        Serial.print("Statisch IP: ");
        Serial.println(Ethernet.localIP());
    }

    server.begin();
    Serial.println("Webserver gestart!");
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

    // ✅ **Controleer of we in Debug Mode moeten zijn**
    bool newDebugMode = (!ethernetEnabled && binaryValue == 0);
    if (newDebugMode != debugMode) {
        debugMode = newDebugMode;
        if (debugMode) {
            Serial.println("🛠 Debug Mode geactiveerd!");
        } else {
            Serial.println("✅ Webserver terug naar normale modus.");
        }
    }

    // ✅ **Bestandsselectie via MCP23017**
    String mcpSelectedFile = getSelectedFile();
    if (!mcpSelectedFile.isEmpty()) {
        if (mcpSelectedFile != selectedFile) {
            selectedFile = mcpSelectedFile;
            selectionSource = "MCP23017";
            Serial.println("📟 Bestand geselecteerd via MCP23017: " + selectedFile);
        } else {
            Serial.println("📂 Bestand is al geselecteerd via MCP23017: " + selectedFile);
        }
    } else {
        Serial.println("❌ Geen geldig bestand geselecteerd via MCP23017.");
    }

    // ✅ **Webserver blijft ALTIJD draaien**
    EthernetClient client = server.available();
    if (client) {
        Serial.println("📡 Nieuwe client verbonden!");

        // ✅ **Lees de eerste regel van de request**
        String firstLine = "";
        while (client.available()) {
            char c = client.read();
            if (c == '\n') break;  // Stop bij de eerste lijn
            firstLine += c;
        }

        Serial.println("📡 Verzoek verwerkt: " + firstLine);

        // ✅ **Geef de ingelezen request door aan handleClient**
        handleClient(client, firstLine);
    }

    delay(500);
}
