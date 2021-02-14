
#include "main.h"
#include "settings.h"
#include "omgui.h"
#include "screens.h"
#include "bitmaps.h"
#include "ds18b20.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif


static SensorsDataT prevSensors;

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
    prevSensors.HallRpm = 0;
    prevSensors.Volt = 0;
    memset(prevSensors.Temperature, 0.0, sizeof(prevSensors.Temperature));
}


static void ScreenUpdateCb(omScreenT *screen)
{
    for(uint8_t i; i < DS18B20_Quantity(); i++)
    {
        if(sensors.Temperature[i] != prevSensors.Temperature[i])
        {
            SEGGER_RTT_printf(0, "temp[%u]: %u\n", i, (uint16_t)sensors.Temperature[i]);
            prevSensors.Temperature[i] = sensors.Temperature[i];
        }
    }
}