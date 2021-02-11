
#include "omgui.h"
#include "screens.h"
#include "main.h"
#include "assets.h"

/* <<<< FreeRTOS >>>> */
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>

//#include "stm32l4xx_hal.h"
#include "sh1122.h"


omGuiT oledUi;
omScreenT screenMain, screenData, screenTemp, screenSetup;

static void DisplayInitCb(omGuiT *);
static void DisplayUpdateCb(omGuiT *);
static void DisplayClearCb(omGuiT *);
static void DisplayDrawPixelCb(omGuiT *, uint32_t x, uint32_t y, uint8_t color);

static void MainGuiManager(void *pvParams);


void Dispatch_Init(void)
{
  TaskHandle_t taskMainGui = NULL;
  BaseType_t taskMainGuiRet = xTaskCreate(
    MainGuiManager,
    "main-gui-manager",        /* Text name for the task. */
    configMINIMAL_STACK_SIZE,  /* Stack size in words, not bytes. */
    (void *) 1,                /* Parameter passed into the task. */
    configMAX_PRIORITIES / 2,  /* Priority of the task created. */
    &taskMainGui );

  oledUi.Id = 0;
  oledUi.ResX = SH1122_OLED_WIDTH;
  oledUi.ResY = SH1122_OLED_HEIGHT;
  oledUi.InitCallback = DisplayInitCb;
  oledUi.DeInitCallback = NULL;
  oledUi.UpdateCallback = DisplayUpdateCb;
  oledUi.ClearCallback = DisplayClearCb;
  oledUi.DrawPixelCallback = DisplayDrawPixelCb;

  omGuiInit(&oledUi);

  Assets_Init();
  MainScreenInit();

  // Show logo
  omDrawBitmap(&oledUi, Assets.Bitmap_Logo, 0, 0, False, True);
  Sleep(2000);

  // if (taskTempPollRet == pdPASS)
  // {
  //   ssd1306_WriteString("Task ok!", Font_7x10, White);
  //   omGuiUpdate(&oled1);
  // }
  // else if (taskTempPollRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
  // {
  //   ssd1306_WriteString("Task FAILED!", Font_7x10, White);
  //   omGuiUpdate(&oled1);
  // }
}


static void MainGuiManager(void *pvParams)
{
  omScreenSelect(&screenMain);

  while(1)
  {


  }
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
