
#include "omgui.h"
#include "screens.h"
#include "main.h"
#include "bitmaps.h"

#include "sh1122.h"
//#include "stm32l4xx_hal.h"
#include "tim.h"


omDisplayT oled1;
omScreenT screenMain, screenData, screenTemp, screenSetup;

static void DisplayInitCb(omDisplayT *);
static void DisplayUpdateCb(omDisplayT *);
static void DisplayDrawPixelCb(omDisplayT *, uint32_t x, uint32_t y, uint8_t color);


void GUI_Init(void)
{
  Bitmaps_Init();
  oled1.Id = 0;
  oled1.ResX = OLED_WIDTH;
  oled1.ResY = OLED_HEIGHT;
  oled1.InitCallback = DisplayInitCb;
  oled1.DeInitCallback = NULL;
  oled1.UpdateCallback = DisplayUpdateCb;
  oled1.DrawPixelCallback = DisplayDrawPixelCb;

  omDisplayInit(&oled1);

//  MainScreenInit();

  // Show logo
//  Frame_DrawLine(13,60,200,17, 0xaa);

  Bitmaps_Init();
  omDrawBitmap(&oled1, &bitmap_test80x25, 0, 0);
  omDrawBitmap(&oled1, &bitmap_test80x25, 120, 20);
  omDisplayUpdate(&oled1);
  Sleep(1000);

  //  omScreenSelect(&screenMain);
}


static void DisplayInitCb(omDisplayT *displ)
{
  Display_Init();
  return;
}


static void DisplayUpdateCb(omDisplayT *displ)
{
  Display_SendFrame();
}


static void DisplayDrawPixelCb(omDisplayT *displ, uint32_t x, uint32_t y, uint8_t color)
{

  Frame_DrawPixel(x, y, color);
  return;
}
