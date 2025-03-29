#include "lcd_handler.h"

LGFX tft;

String lastDisplayedFile = "";

void displaySelectedFile(const String& file) {
    File lcdFile = SD_MMC.open("/" + file);
    if (!lcdFile) {
        Serial.println("⛔ Kan bestand niet openen: " + file);
        return;
    }

    Serial.println("📟 Bestand geselecteerd via " + selectionSource + ": " + file);

    if (lcdFile.isDirectory()) {
        Serial.println("🎞 Start video uit map: " + file);
        playVideoBMP(("/" + file).c_str());
    } else {
        String ext = file.substring(file.lastIndexOf('.') + 1);
        ext.toLowerCase();

        if (ext == "jpg" || ext == "jpeg") {
            Serial.println("🖼 Weergeven JPG afbeelding op LCD...");
            renderPhotoJPG(("/" + file).c_str());
        } else if (ext == "bmp") {
            Serial.println("🖼 Weergeven BMP afbeelding op LCD...");
            renderPhotoBMP(("/" + file).c_str());
        } else {
            Serial.println("⚠️ Onbekend formaat: " + file);
        }
    }

    lcdFile.close();
}


void renderPhotoBMP(const char* filepath) {
    File bmpFile = SD_MMC.open(filepath);
    if (!bmpFile) {
        Serial.println("❌ Kan BMP bestand niet openen!");
        return;
    }

    bmpFile.seek(18);
    int32_t bmpWidth = 0, bmpHeight = 0;
    bmpFile.read((uint8_t*)&bmpWidth, 4);
    bmpFile.read((uint8_t*)&bmpHeight, 4);
    bmpFile.seek(0);

    int xpos = (tft.width() - bmpWidth) / 2;
    int ypos = (tft.height() - bmpHeight) / 2;

    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);

    if (!tft.drawBmp(&bmpFile, xpos, ypos)) {
        Serial.println("❌ BMP render mislukt");
    } else {
        Serial.println("✔️ BMP weergegeven: " + String(filepath));
    }
    bmpFile.close();
}


void renderPhotoJPG(const char* filepath) {
  Serial.print("📸 Afbeelding laden: ");
  Serial.println(filepath);

  File jpgFile = SD_MMC.open(filepath);
  if (!jpgFile) {
    Serial.println("❌ Kan JPG bestand niet openen!");
    return;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  if (JpegDec.decodeFsFile(jpgFile) == 0) {
    Serial.println("❌ Decoderen van JPG mislukt!");
    jpgFile.close();
    return;
  }

  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;

  int16_t maxWidth = JpegDec.width;
  int16_t maxHeight = JpegDec.height;

  int xpos = (tft.width() - maxWidth) / 2;
  int ypos = (tft.height() - maxHeight) / 2;

  uint16_t *pImg;
  uint16_t lineBuffer[mcu_w * mcu_h];

  Serial.println("✅ JPG decode gelukt, tekenen...");
  tft.startWrite();

  while (JpegDec.read()) {
    pImg = JpegDec.pImage;

    int x = xpos + JpegDec.MCUx * mcu_w;
    int y = ypos + JpegDec.MCUy * mcu_h;
    int w = JpegDec.MCUWidth;
    int h = JpegDec.MCUHeight;

    // Beveiliging voor schermranden
    if (x + w > tft.width())  w = tft.width() - x;
    if (y + h > tft.height()) h = tft.height() - y;
    if (w <= 0 || h <= 0) continue;

    for (int i = 0; i < w * h; i++) {
      uint16_t color = pImg[i];

      // Simpele brightness boost
      uint8_t r = (color >> 11) & 0x1F;
      uint8_t g = (color >> 5) & 0x3F;
      uint8_t b = color & 0x1F;

      r = std::min((int)(r * 1.1), 31);
      g = std::min((int)(g * 1.1), 63);
      b = std::min((int)(b * 1.1), 31);

      lineBuffer[i] = (r << 11) | (g << 5) | b;
    }

    tft.pushImage(x, y, w, h, lineBuffer);
  }

  tft.endWrite();
  jpgFile.close();

  Serial.println("✅ JPEG weergegeven!");
}



void renderVideoFrame(const char* filepath) {
  File bmpFile = SD_MMC.open(filepath);
  if (!bmpFile) {
    Serial.print("❌ Kan frame niet openen: ");
    Serial.println(filepath);
    return;
  }

  bmpFile.seek(18);
  int32_t bmpWidth = 0, bmpHeight = 0;
  bmpFile.read((uint8_t*)&bmpWidth, 4);
  bmpFile.read((uint8_t*)&bmpHeight, 4);
  bmpFile.seek(0);

  int xpos = (tft.width() - bmpWidth) / 2;
  int ypos = (tft.height() - bmpHeight) / 2;

  tft.setSwapBytes(true);
  if (tft.drawBmp(&bmpFile, xpos, ypos)) {
    Serial.print("✔️ Frame weergegeven: ");
    Serial.println(filepath);
  }
  bmpFile.close();
}

void playVideoBMP(const char* folder, int delayMs) {
  File dir = SD_MMC.open(folder);
  if (!dir || !dir.isDirectory()) {
    Serial.println("❌ Ongeldige videomap");
    return;
  }

  std::vector<std::string> frameFiles;
  File file;
  while ((file = dir.openNextFile())) {
    String name = file.name();
    if (name.endsWith(".bmp")) {
      frameFiles.push_back(std::string(name.c_str()));
    }
    file.close();
  }

  std::sort(frameFiles.begin(), frameFiles.end());

  unsigned long frameStart, totalStart = millis();
  int frameCounter = 0;

  for (const auto& f : frameFiles) {
    frameStart = millis();
    std::string fullPath = std::string(folder) + "/" + f;
    renderVideoFrame(fullPath.c_str());
    frameCounter++;
    int frameTime = millis() - frameStart;
    Serial.printf("🟦 Frame %d: render time = %d ms\n", frameCounter, frameTime);
    delay(delayMs);
  }

  int totalTime = millis() - totalStart;
  float fps = frameCounter * 1000.0 / totalTime;
  Serial.printf("🎬 Gemiddelde FPS: %.2f (%d frames in %d ms)\n", fps, frameCounter, totalTime);
}

// Testmodus voor verticale kleurvergelijking
void initLCD() {
  tft.init();
  tft.setSwapBytes(true);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  const uint16_t colors[] = {
    0xF800, // RED
    0x07E0, // GREEN
    0x001F, // BLUE
    0xFFE0, // YELLOW
    0x07FF, // CYAN
    0xF81F  // MAGENTA
  };

  int numColors = sizeof(colors) / sizeof(colors[0]);
  int blockWidth = tft.width() / numColors;
  int blockHeight = 160; // Bovenste helft

  // Boven: fillRect kleuren
  for (int i = 0; i < numColors; i++) {
    tft.fillRect(i * blockWidth, 0, blockWidth, blockHeight, colors[i]);
  }

  // Onder: bijbehorende BMP
  File bmpFile = SD_MMC.open("/255_rgbtest.bmp");
  if (bmpFile) {
    tft.drawBmp(&bmpFile, 0, 170);
    bmpFile.close();
  } else {
    Serial.println("⚠️ /255_rgbtest niet gevonden");
  }
  delay(5000);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(9);
  tft.drawString("LED-E-LUX", 0, 130);
  delay(2000);
}
