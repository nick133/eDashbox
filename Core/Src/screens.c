
#include <math.h>
#include "main.h"
#include "omgui.h"
#include "screens.h"
#include "settings.h"
#include "bitmaps.h"
#include "sh1122.h"


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

    omGui_Init(&oledUi);

    /* Assets must be initialized from function, not by global assignment
     * in bitmaps.c, or strange behaviour occurs - only first bitmap is
     * accessible, others are all zeros. Also some global pointers become lost.
     */
    Bitmaps_Init();

    MainScreen_Init();
    NumEditScreen_Init();
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


/* Shared utilities for screens */

float SsrGetSpeed(SensorsDataT *data)
{
    if(data->HallRpm == 0.0) { return 0.0; }

    float speed = data->HallRpm * 60.0 * (float)Config.WheelCirc / 1000000.0;

    if(Config.HallOnWheel)
        { speed /= Config.GearRatio; }

    if(Config.SpeedUnits == UnitsMph)
        { speed /= KILOS_PER_MILE; }

    return speed;
}


float SsrGetMotorRpm(SensorsDataT *data)
{
    if(data->HallRpm == 0.0) { return 0.0; }

    return (Config.HallOnWheel ? data->HallRpm * Config.GearRatio : data->HallRpm);
}


float SsrGetRpmPerctg(SensorsDataT *data)
{
    if(data->HallRpm == 0.0) { return 0.0; }

    return (SsrGetMotorRpm(data) * 100.0 / (float)Config.MaxRpm);
}


float SsrGetTemprt(SensorsDataT *data, uint8_t index)
{
    if(Config.TempUnits == UnitsCelsius)
    {
        return data->Temperature[index];
    }
    else if(Config.TempUnits == UnitsFahrenheit)
    {
        return ((data->Temperature[index] * 9.0/5.0) + 32.0);
    }
}


float SsrGetBatPerctg(SensorsDataT *data)
{
    return (data->Volt < Config.BatLowV) ? 0.0
        : ((data->Volt - Config.BatLowV) * 100.0 / (Config.BatHighV - Config.BatLowV));
}