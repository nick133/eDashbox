
#include "omgui.h"
#include "screens.h"
#include "main.h"
#include "bitmaps.h"

#include "sh1122.h"
//#include "stm32l4xx_hal.h"
#include "tim.h"


omGuiT oledUi;
omScreenT screenMain, screenData, screenTemp, screenSetup;

static void DisplayInitCb(omGuiT *);
static void DisplayUpdateCb(omGuiT *);
static void DisplayClearCb(omGuiT *);
static void DisplayDrawPixelCb(omGuiT *, uint32_t x, uint32_t y, uint8_t color);


void GUI_Init(void)
{
  Bitmaps_Init();
  oledUi.Id = 0;
  oledUi.ResX = SH1122_OLED_WIDTH;
  oledUi.ResY = SH1122_OLED_HEIGHT;
  oledUi.InitCallback = DisplayInitCb;
  oledUi.DeInitCallback = NULL;
  oledUi.UpdateCallback = DisplayUpdateCb;
  oledUi.ClearCallback = DisplayClearCb;
  oledUi.DrawPixelCallback = DisplayDrawPixelCb;

  omGuiInit(&oledUi);

//  MainScreenInit();

  // Show logo

  // SH1122_WriteData(omBitmap_test80x25.RawData, (omBitmap_test80x25.Width * omBitmap_test80x25.Height)/2);
  Bitmaps_Init();
  // SH1122_DisplayUpdate();
  // SH1122_ClearRAM();
  omDrawBitmap(&oledUi, &omBitmap_test_gradient, 10, 0, True);
  omDrawLine(&oledUi, 0,0, 255,63, OLED_GRAY_06, False);
  omDrawLine(&oledUi, 0,63, 255,0, OLED_GRAY_06, True);
  // omDrawBitmap(&oledUi, &omBitmap_test_gradient, 100, 30, True);
  // omDrawBitmap(&oledUi, &omBitmap_test80x25, 0, 20, True);
  

  // SH1122_DrawPixel(0, 0, OLED_GRAY_15);
  // SH1122_DisplayUpdate();
  // SH1122_DrawPixel(1, 0, OLED_GRAY_15);
  // SH1122_DisplayUpdate();
  // SH1122_DrawPixel(2, 0, OLED_GRAY_15);
  // SH1122_DisplayUpdate();
  // SH1122_DrawPixel(3, 0, OLED_GRAY_15);
  // SH1122_DisplayUpdate();
  Sleep(2000);
  
  // omGuiUpdate(&oledUi);
  //  omScreenSelect(&screenMain);
}


static void DisplayInitCb(omGuiT *ui)
{
  SH1122_DisplayInit();
}


static void DisplayUpdateCb(omGuiT *ui)
{
  SH1122_DisplayUpdate();
}


static void DisplayClearCb(omGuiT *ui)
{
  SH1122_ClearRAM();
}


static void DisplayDrawPixelCb(omGuiT *ui, uint32_t x, uint32_t y, uint8_t color)
{
  SH1122_DrawPixel(x, y, color);
}
