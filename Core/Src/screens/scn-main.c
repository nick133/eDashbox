
#include "main.h"
#include "settings.h"
#include "omgui.h"
#include "screens.h"
#include "bitmaps.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif


static void ScreenShowCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *);


void MainScreenInit(void)
{
    screenMain.Id = IdScreenMain;
    screenMain.Ui = &oledUi;
    screenMain.ShowCallback = ScreenShowCb;
    screenMain.HideCallback = NULL;
    screenMain.UpdateCallback = ScreenUpdateCb;
}


static void ScreenShowCb(omScreenT *screen)
{
    SEGGER_RTT_printf(0, "ScreenMain!");

    SensorsDataT prevData = { 0, { 0.0, 0.0, 0.0, 0.0 }, 0.0 };
    return;
}


static void ScreenUpdateCb(omScreenT *screen)
{
    return;
}