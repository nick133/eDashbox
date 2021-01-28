
#include "omgui.h"
#include "screens.h"
#include "main.h"

#include "ssd1306.h"
#include "stm32l4xx_hal.h"
#include "tim.h"


omDisplayT oled1;
omScreenT screenLogo, screenMain, screenData, screenTemp, screenSetup;

void DisplayInitCb(omDisplayT *);


void OLED_GUI_Init(void)
{
  MainScreenInit();
  oled1.Id = 0;
//  oled1.Screen = (config.ShowLogo == True) ? &screenLogo : &screenMain;
  oled1.Screen = &screenMain;
  oled1.InitCallback = DisplayInitCb;
omDisplayT *d=&oled1;
//  LogoScreenInit();

  omDisplayInit(&oled1);

  HAL_Delay(1000);

//  omScreenSelect(&screenMain);

//  ssd1306_Init();
//  ssd1306_SetCursor(1, 1);
  ssd1306_WriteString(" Test: 7", Font_7x10, White);
  ssd1306_UpdateScreen();

uint8_t image_data[] = {
1, 0, 0, 0, 0, 0, 150, 75, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 105, 0, 0, 150, 75, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 210, 105, 0, 0, 0, 105, 210, 105, 0, 0, 0, 0, 0,
0, 0, 0, 0, 210, 225, 225, 225, 210, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 165, 165, 0, 0, 0, 0, 165, 210, 165, 0, 0, 0, 0,
0, 120, 210, 165, 195, 165, 0, 0, 210, 180, 105, 210, 105, 0, 0, 0,
0, 75, 150, 0, 165, 165, 0, 165, 165, 0, 0, 165, 165, 0, 0, 0,
0, 0, 0, 0, 225, 225, 0, 210, 105, 225, 210, 210, 105, 0, 0, 0,
0, 0, 0, 0, 0, 0, 210, 225, 0, 0, 105, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 105, 225, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 165, 0, 0, 0, 105, 210, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 165, 210, 165, 165, 165, 225, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 105, 165, 165, 165, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
  omBitmapT bmp;
  bmp.Width = 16;
  bmp.Height = 16;
  bmp.ColorsNumOf = 16;
  bmp.IsAlpha = False;
  bmp.RawData = &image_data[0];
//  bmp.RawData = (uint32_t*)&image_data[0];


  uint32_t i = 0;
  for (uint32_t y = 0; y < bmp.Height; y++)
    for (uint32_t x = 0; x < bmp.Width; x++)
    {
      uint32_t pxl = bmp.RawData[i];

      if (pxl > 0)
      {
        ssd1306_DrawPixel(x, y, White);
      }
      else
      {
        ssd1306_DrawPixel(x, y, Black);
      }
      i++;
    }

 ssd1306_DrawPixel(66, 28, White);
  ssd1306_UpdateScreen();


//  struct omBitmap
//    uint16_t Width, Height;
//    uint32_t *RawData;
//    uint16_t ColorsNumOf;
//    Bool IsAlpha;
//    uint32_t AlphaColor;
}

void DisplayInitCb(omDisplayT *displ)
{
  /* HAL_TIM_IC_Start_IT is not enabling HAL_TIM_PeriodElapsedCallback,
   * so we need to manually enable it. See:
   * https://community.st.com/s/question/0D50X00009hpBdlSAE/timer3-update-event-interrupt-not-working-properly
   */
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

  ssd1306_Init();
  return;
}
