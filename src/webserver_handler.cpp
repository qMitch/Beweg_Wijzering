#include "webserver_handler.h"

void initWebserver() {
    Serial.println("🌍 Initialiseren van Webserver...");
    server.begin();
}

void generateWebPage(EthernetClient &client) {
    Serial.println("🌍 Genereren van webpagina...");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<html><head><title>ESP32-S3 Webserver</title></head><body>");
    client.println("<h1>ESP32-S3 Bestandsbeheer</h1>");

    // ✅ Geselecteerd bestand tonen
    client.println("<h2>Geselecteerd bestand:</h2>");
    if (selectedFile.isEmpty()) {
        client.println("<p><b>Geen bestand geselecteerd.</b></p>");
    } else {
        client.println("<p><b>" + selectedFile + "</b> (Geselecteerd via: " + selectionSource + ")</p>");
        if (selectedFile.endsWith(".jpg") || selectedFile.endsWith(".JPG")) {
            client.println("<img src='/download?file=" + selectedFile + "' style='max-width:300px; display:block; margin-top:10px;'>");
        }
    }

    client.println("<h2>Bestanden op de SD-kaart:</h2><ul>");
    File root = SD_MMC.open("/");
    if (!root) {
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
            // ✅ Altijd selecteerbaar
            client.print(" <a href='/select?file=" + filename + "'><button>Selecteer</button></a>");
            // ✅ Alleen download als het GEEN map is en eindigt op .jpg
            if (!file.isDirectory() && filename.endsWith(".jpg")) {
                client.print(" <a href='/download?file=" + filename + "'><button>Download</button></a>");
            }
            client.println("</li>");
            file = root.openNextFile();
        }
        root.close();
    }
    client.println("</ul>");

    // ✅ Upload formulier
    client.print("<h2>Upload een bestand naar de SD-kaart:</h2>");
    client.print("<form method='POST' action='/upload' enctype='multipart/form-data'>");
    client.print("<input type='file' name='uploadfile' id='fileInput'>");
    client.print("<input type='submit' value='Upload'>");
    client.print("<p id='fileName'>Geen bestand gekozen</p>");
    client.print("</form>");

    // ✅ JS om bestandsnaam te tonen
    client.print("<script>");
    client.print("document.getElementById('fileInput').addEventListener('change', function() {");
    client.print("document.getElementById('fileName').innerText = this.files[0] ? this.files[0].name : 'Geen bestand gekozen';");
    client.print("});");
    client.print("</script>");

    client.print("</body></html>");
}

void generateDebugPage(EthernetClient &client) {
    Serial.println("🐞 Genereren van Debug Webpagina...");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    client.println("<html><head><title>Debug Mode</title></head><body>");
    client.println("<h1>ESP32-S3 Debug Mode</h1>");
    
    // ✅ **Toon status van MCP23017 en SD-kaart**
    client.println("<h2>Status</h2>");
    client.println("<p>MCP23017: " + String(mcpConnected ? "Verbonden" : "Niet gevonden") + "</p>");
    client.println("<p>SD-Kaart: " + String(sdConnected ? "Verbonden" : "Niet gevonden") + "</p>");

    // ✅ **Toon binaire input van MCP23017**
    client.println("<h2>MCP23017 Binaire Input</h2>");
    int binaryValue = getBinaryInput();
    client.println("<p>Binaire waarde: " + String(binaryValue, BIN) + " (" + String(binaryValue) + ")</p>");

    // ✅ **Geselecteerd bestand tonen**
    client.println("<h2>Geselecteerd bestand</h2>");
    if (selectedFile.isEmpty()) {
        client.println("<p>⚠ Geen bestand geselecteerd.</p>");
    } else {
        client.println("<p><b>" + selectedFile + "</b> (Geselecteerd via: " + selectionSource + ")</p>");
    }

    client.println("</body></html>");
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

    // ✅ **Bestand selecteren via Webserver**
    if (request.indexOf("GET /select?file=") >= 0) {
        Serial.println("✅ SELECT request gedetecteerd!");
        int start = request.indexOf("file=") + 5;
        int end = request.indexOf(" ", start);
        if (end == -1) end = request.length();
        String requestedFile = request.substring(start, end);
        requestedFile.trim();
        Serial.println("📡 Bestandsselectie verzoek ontvangen: " + requestedFile);
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

    if (debugMode) {
        generateDebugPage(client);
    } else {
        generateWebPage(client);
    }


    delay(10);
    client.stop();
    Serial.println("✅ Webpagina verzonden!");
}




