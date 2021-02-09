
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
  oledUi.DrawPixelCallback = DisplayDrawPixelCb;

  omGuiInit(&oledUi);

//  MainScreenInit();

  // Show logo
//  Frame_DrawLine(13,60,200,17, 0xaa);

  Bitmaps_Init();
//  omDrawBitmap(&oledUi, &bitmap_test80x25, 0, 0);
//  omDrawBitmap(&oledUi, &bitmap_test80x25, 120, 20);
  
    // uint16_t x1 = 0,
    //   y1 = 0, x2 = 127, y2 = 63;

    // const int16_t deltaX = abs(x2 - x1);
    // const int16_t deltaY = abs(y2 - y1);
    // const int16_t signX = x1 < x2 ? 1 : -1;
    // const int16_t signY = y1 < y2 ? 1 : -1;

    // int16_t error = deltaX - deltaY;

    // SH1122_DrawPixel(x2, y2, OLED_GRAY_07);

    // while (x1 != x2 || y1 != y2)
    // {
    //     SH1122_DrawPixel(x1, y1, OLED_GRAY_15);
    //     const int16_t error2 = error * 2;

    //     if (error2 > -deltaY)
    //     {
    //         error -= deltaY;
    //         x1 += signX;
    //     }
    //     if (error2 < deltaX)
    //     {
    //         error += deltaX;
    //         y1 += signY;
    //     }
    // }

  SH1122_DrawPixel(0, 0, OLED_GRAY_15);
  SH1122_DrawPixel(0, 62, OLED_GRAY_15);
  SH1122_DrawPixel(255, 0, OLED_GRAY_15);
  SH1122_DrawPixel(255, 62, OLED_GRAY_15);
  SH1122_DisplayUpdate();
  Sleep(2000);
  
  // omGuiUpdate(&oledUi);
  //  omScreenSelect(&screenMain);
}


static void DisplayInitCb(omGuiT *ui)
{
  Display_Init();
  return;
}


static void DisplayUpdateCb(omGuiT *ui)
{
  Display_SendFrame();
}


static void DisplayDrawPixelCb(omGuiT *ui, uint32_t x, uint32_t y, uint8_t color)
{

  Frame_DrawPixel(x, y, color);
  return;
}
