#include <unity.h>
#include "sd_handler.h"

// 📌 **SDIO Pinnen**
#define SD_CARD_D0   14
#define SD_CARD_CMD  13
#define SD_CARD_CLK  12
#define SD_CARD_D3   11
#define SD_CARD_D2   10
#define SD_CARD_D1   9

void test_sd_card_initialization() {
    TEST_ASSERT_TRUE(initSDCard(SD_CARD_CLK, SD_CARD_CMD, SD_CARD_D0, SD_CARD_D1, SD_CARD_D2, SD_CARD_D3));
}

void test_sd_file_write() {
    File file = SD_MMC.open("/testfile.txt", FILE_WRITE);
    TEST_ASSERT_TRUE(file);  // ✅ Correcte check
    file.println("Unit test check");
    file.close();
    
    File checkFile = SD_MMC.open("/testfile.txt");
    TEST_ASSERT_TRUE(checkFile);  // ✅ Correcte check
    checkFile.close();
}

void test_sd() { 
    RUN_TEST(test_sd_card_initialization);
    RUN_TEST(test_sd_file_write);
}

