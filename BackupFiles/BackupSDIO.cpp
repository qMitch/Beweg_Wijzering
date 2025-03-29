#include "Arduino.h"
#include "SD_MMC.h"
#include "FS.h"

#define SD_CARD_D0   14    // Data Line 0
#define SD_CARD_CMD  13    // SDIO Command
#define SD_CARD_CLK  12   // SDIO Clock
#define SD_CARD_D3   11   // Data Line 3
#define SD_CARD_D2   10   // Data Line 2
#define SD_CARD_D1   9   // Data Line 1


bool initSDCard(int attempts = 3, int delayTime = 1000) {
    for (int i = 0; i < attempts; i++) {
        Serial.printf("🔄 Attempt %d to initialize SD Card...\n", i + 1);

        if (SD_MMC.begin("/sdcard", false)) {  // false = 4-bit mode
            Serial.println("✅ SD Card Mounted Successfully!");
            return true;
        }

        Serial.println("⚠ SD Card Mount Failed! Retrying...");
        delay(delayTime);  // Wacht en probeer opnieuw
    }

    Serial.println("⛔ ERROR: SD Card failed to mount after multiple attempts.");
    return false;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("📂 Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("⛔ ERROR: Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("⛔ ERROR: Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        String fullPath = String(dirname) + "/" + file.name(); // Correcte paden

        if (file.isDirectory()) {
            Serial.print("📁 DIR : ");
            Serial.println(fullPath);
            if (levels) {
                listDir(fs, fullPath.c_str(), levels - 1);
            }
        } else {
            Serial.print("📄 FILE: ");
            Serial.print(fullPath);
            Serial.print("    SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("🔧 Configuring SD Card for SDIO...");

    // Stel de aangepaste SDIO pinnen in
    if (!SD_MMC.setPins(SD_CARD_CLK, SD_CARD_CMD, SD_CARD_D0, SD_CARD_D1, SD_CARD_D2, SD_CARD_D3)) {
        Serial.println("⛔ ERROR: SD_MMC.setPins() failed! Controleer of deze pinnen SDIO ondersteunen.");
        return;
    }

    Serial.println("🔄 Initializing SD Card...");
    
    if (!initSDCard(5, 1500)) {  // Probeert 5 keer, 1.5s vertraging tussen pogingen
        Serial.println("⛔ ERROR: Could not mount SD Card. Continuing without SD...");
        return;
    }

    // 📝 Lijst alle bestanden in de root directory
    listDir(SD_MMC, "/", 2);
}

void loop() {
    // Als de SD-kaart ooit verloren gaat, probeer hem opnieuw te mounten
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 10000) {  // Elke 10 sec controleren
        lastCheck = millis();
        if (!SD_MMC.cardType()) {
            Serial.println("⚠ SD Card lost! Re-initializing...");
            initSDCard(3, 1000);
        }
    }
}