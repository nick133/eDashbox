
#include "main.h"
#include "omgui.h"
#include "screens.h"
#include "bitmaps.h"
#include "sh1122.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif


omGuiT oledUi;
omScreenT screenMain, screenData, screenTemp, screenSetup;
omScreenT *uiScreens[4];

static void DisplayInitCb(omGuiT *);
static void DisplayUpdateCb(omGuiT *);
static void DisplayClearCb(omGuiT *);
static void DisplayDrawPixelCb(omGuiT *, uint32_t x, uint32_t y, uint8_t color);


void Screens_Init(void)
{
    oledUi.Id = 0;
    oledUi.ResX = SH1122_OLED_WIDTH;
    oledUi.ResY = SH1122_OLED_HEIGHT;
    oledUi.InitCallback = DisplayInitCb;
    oledUi.DeInitCallback = NULL;
    oledUi.UpdateCallback = DisplayUpdateCb;
    oledUi.ClearCallback = DisplayClearCb;
    oledUi.DrawPixelCallback = DisplayDrawPixelCb;

    uiScreens[IdScreenMain] = &screenMain;
    uiScreens[IdScreenData] = &screenData;

    omGuiInit(&oledUi);
    MainScreenInit();
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
