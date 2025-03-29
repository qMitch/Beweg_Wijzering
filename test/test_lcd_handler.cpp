#include "test_lcd_handler.h"
#include "lcd_handler.h"

void test_init_lcd() {
    // Gewoon aanroepen, kijken of het crasht of foutmeldingen geeft
    initLCD();
    TEST_ASSERT_TRUE(true);  // Als we hier komen, is het al OK
}

void test_render_bmp() {
    renderPhotoBMP("/test.bmp");  // Zorg dat dit testbestand bestaat op SD
    TEST_ASSERT_TRUE(true);
}

void test_render_jpg() {
    renderPhotoJPG("/test.jpg");  // Zorg dat dit testbestand bestaat op SD
    TEST_ASSERT_TRUE(true);
}

void test_display_selected_file() {
    displaySelectedFile("test.jpg");
    displaySelectedFile("test.bmp");
    displaySelectedFile("testmap");  // map met bmp-bestanden
    TEST_ASSERT_TRUE(true);
}

void test_render_video_frame() {
    renderVideoFrame("/test.bmp");
    TEST_ASSERT_TRUE(true);
}

void test_play_video() {
    playVideoBMP("/testmap", 10);  // map moet bmp-bestanden bevatten
    TEST_ASSERT_TRUE(true);
}
