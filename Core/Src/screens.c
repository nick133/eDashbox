
#include "omgui.h"
#include "screens.h"
#include "main.h"
#include "bitmaps.h"

#include "sh1122.h"
//#include "stm32l4xx_hal.h"
#include "tim.h"


omDisplayT oled1;
omScreenT screenLogo, screenMain, screenData, screenTemp, screenSetup;

static void DisplayInitCb(omDisplayT *);
static void DisplayUpdateCb(omDisplayT *);
static void DisplayDrawPixelCb(omDisplayT *, uint32_t x, uint32_t y, uint32_t color);


void OLED_GUI_Init(void)
{
  oled1.Id = 0;
  oled1.ResX = 256;
  oled1.ResY = 64;
  oled1.InitCallback = DisplayInitCb;
  oled1.DeInitCallback = NULL;
  oled1.UpdateCallback = DisplayUpdateCb;
  oled1.DrawPixelCallback = DisplayDrawPixelCb;

//  LogoScreenInit();
  MainScreenInit();

  omDisplayInit(&oled1);

  Bitmaps_Init();
  // Show logo
  //omDrawBitmap(&oled1, &bitmap_logo, 0, 0);
//
  Sleep(1000);
  omDisplayUpdate(&oled1);

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


static void DisplayDrawPixelCb(omDisplayT *displ, uint32_t x, uint32_t y, uint32_t color)
{

  Frame_DrawPixel(x, y, color);
  return;
}
