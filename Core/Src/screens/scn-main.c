
#include <string.h>
#include "ds18b20.h"

#include "main.h"
#include "settings.h"
#include "omgui.h"
#include "screens.h"
#include "bitmaps.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif


static SensorsDataT prevSensors;

static void ScreenShowCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *);
static void RefreshRpm(void);
static void RefreshSpeed(void);
static void RefreshVolt(void);
static void RefreshAmpere(void);

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

    omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo0, 15+24, 2, false, false);
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

    if(sensors.HallRpm != prevSensors.HallRpm)
    {
        RefreshRpm();
        RefreshSpeed();
    }

    if(sensors.Volt != prevSensors.Volt)
    {
        RefreshVolt();
        RefreshAmpere();
    }
}


static void RefreshRpm(void)
{
 //   omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo100, 0, 2, false, false);
}


static void RefreshSpeed(void)
{
    // Hall sensor is on wheel
    float speed = sensors.HallRpm * 60 * config.WheelCirc / 1000000;

    // Hall sensor is on motor's rotor
    if(config.HallOnWheel == false)
        { speed /= config.GearRatio; }

    if(config.SpeedUnits == UnitsMph)
        { speed /= KILOS_PER_MILE; }
}


static void RefreshVolt(void)
{
    
}


static void RefreshAmpere(void)
{
    
}