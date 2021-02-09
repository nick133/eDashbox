
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
  oledUi.ResX = OLED_WIDTH;
  oledUi.ResY = OLED_HEIGHT;
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
  
  uint8_t Buffer[OLED_WIDTH * OLED_HEIGHT / 2] = {0};

  // for (uint32_t i = 0; i < OLED_HEIGHT; i++)
  // {
  //   for (uint32_t j = 0; j < OLED_WIDTH - 1; j = j + 2)
  //   {
  //     Buffer[(j + i * OLED_WIDTH) / 2] = (FrameBuffer[j][i] & 0xF0) | ((FrameBuffer[j + 1][i]) & 0x0F);
  //   }
  // }

  // get byte, containing a pixel
  int x = 34, y = 124;
  uint16_t idx = y*OLED_WIDTH/2 + x/2;
  uint8_t byte = Buffer[idx];
  // uint8_t left_1st_pixel = byte >> 4;
  // uint8_t right_2nd_pixel = byte & 0x0f;
  uint8_t color = 0xff;

  if (x & 1) // x is odd, we set second/right pixel
  {
      // apply right 4 bits
      byte = ( byte & 0xf0 ) | color; 
  }
  else // x is even, we set first/left pixel
  {
      // apply left 4 bits
      byte = (( byte & 0x0f ) << 4) | color;
  }
  Buffer[idx] = color;

  Buffer[128] = 0xf0;
  SH1122_WriteData(Buffer, OLED_WIDTH * OLED_HEIGHT / 2);
  Sleep(2000);

  Buffer[128] = 0x0f;
  SH1122_WriteData(Buffer, OLED_WIDTH * OLED_HEIGHT / 2);
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
