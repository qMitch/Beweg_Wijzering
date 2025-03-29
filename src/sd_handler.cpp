#include "sd_handler.h"


bool initSDCard(int clk, int cmd, int d0, int d1, int d2, int d3) {
    Serial.println("SD-Kaart wordt geladen...");

    if (!SD_MMC.setPins(clk, cmd, d0, d1, d2, d3)) {
        Serial.println("⛔ ERROR: Kan SD-Kaart pinnen niet instellen!");
        return false;
    }

    if (SD_MMC.begin("/sdcard", false, false, 40000)) {
        Serial.println("✅ SD-Kaart correct geladen!");
        sdConnected = true;
        return true;
    }
    
    Serial.println("⛔ ERROR: SD-Kaart niet gevonden!");
    return false;
}

void serveFile(EthernetClient &client, String filename) {
    File file = SD_MMC.open("/" + filename);
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
   uint8_t buffer[512];
    while (file.available()) {
        int len = file.read(buffer, sizeof(buffer));
        client.write(buffer, len);
    }
    file.close();
}

void handleFileUpload(EthernetClient &client) {
    Serial.println("📤 Bestand upload gestart...");

    String filename = "/uploaded_file.txt";
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
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("⛔ ERROR: Kan bestand niet openen om te schrijven!");
        return;
    }
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
    client.println("HTTP/1.1 303 See Other");
    client.println("Location: /"); // Dit zorgt ervoor dat de pagina na uploaden herlaadt
    client.println("Connection: close");
    client.println();
}


String getSelectedFile() {
    int binaryValue = getBinaryInput();
    if (binaryValue > 0) {
        char filenamePrefix[20];
        sprintf(filenamePrefix, "%03d_", binaryValue);

        Serial.println("🔍 Zoeken naar bestand beginnend met: " + String(filenamePrefix));

        File root = SD_MMC.open("/");
        if (!root) {
            Serial.println("⛔ ERROR: Kan de SD-kaart directory niet openen!");
            return "";
        }

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
    if (!selectedFile.isEmpty()) {
        Serial.println("✅ Webserver heeft bestand geselecteerd: " + selectedFile);
        return selectedFile;
    }
    Serial.println("⛔ Geen bestand geselecteerd.");
    return "";
}
