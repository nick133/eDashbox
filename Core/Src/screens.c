
#include "omgui.h"
#include "screens.h"
#include "main.h"

#include "ssd1306.h"
//#include "stm32l4xx_hal.h"
#include "tim.h"


omDisplayT oled1;
omScreenT screenLogo, screenMain, screenData, screenTemp, screenSetup;

void DisplayInitCb(omDisplayT *);


void OLED_GUI_Init(void)
{
  oled1.Id = 0;
  //oled1.Screen = &screenLogo;
  oled1.Screen = &screenMain;
  oled1.InitCallback = DisplayInitCb;

//  LogoScreenInit();
  MainScreenInit();

  omDisplayInit(&oled1);

  HAL_Delay(1000);

//  omScreenSelect(&screenMain);

/* This DOES NOT WORK: uint32_t image_data[] = { ... }
 * Array size is a MUST!
 * Display_Color = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
 * Gray_00 = BLACK?
 * Gray_15 = WHITE?
 */
uint32_t image_data[256] = {
  0,   0,   0,   0,   0,   0, 150,  75,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0, 105,   0,   0, 150,  75,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0, 210, 105,   0,   0,   0, 105, 210, 105,   0,   0,   0,   0,   0,
  0,   0,   0,   0, 210, 225, 225, 225, 210,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0, 165, 165,   0,   0,   0,   0, 165, 210, 165,   0,   0,   0,   0,
  0, 120, 210, 165, 195, 165,   0,   0, 210, 180, 105, 210, 105,   0,   0,   0,
  0,  75, 150,   0, 165, 165,   0, 165, 165,   0,   0, 165, 165,   0,   0,   0,
  0,   0,   0,   0, 225, 225,   0, 210, 105, 225, 210, 210, 105,   0,   0,   0,
  0,   0,   0,   0,   0,   0, 210, 225,   0,   0, 105,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0, 105, 225,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0, 165,   0,   0,   0, 105, 210,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0, 165, 210, 165, 165, 165, 225,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0, 105, 165, 165, 165,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

  omBitmapT bmp;
  bmp.Width = 16;
  bmp.Height = 16;
  bmp.ColorsNumOf = 16;
  bmp.IsAlpha = False;
  bmp.RawData = &image_data[0];
  bmp.Display = &oled1;

  omDrawBitmap(&bmp);
  omDisplayUpdate(&oled1);
}

void DisplayInitCb(omDisplayT *displ)
{
  ssd1306_Init();
  return;
}
