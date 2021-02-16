
#include <string.h>
#include <math.h>
#include "printf.h"
#include "ds18b20.h"
#include "sh1122.h"

#include "main.h"
#include "settings.h"
#include "bitmaps.h"
#include "omgui.h"
#include "screens.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif


static const uint8_t AsciiShift = 48;

static const omBitmapT *SpeedoFont[10] = {
    &AssetBitmaps.MainSpeedo0,
    &AssetBitmaps.MainSpeedo1,
    &AssetBitmaps.MainSpeedo2,
    &AssetBitmaps.MainSpeedo3,
    &AssetBitmaps.MainSpeedo4,
    &AssetBitmaps.MainSpeedo5,
    &AssetBitmaps.MainSpeedo6,
    &AssetBitmaps.MainSpeedo7,
    &AssetBitmaps.MainSpeedo8,
    &AssetBitmaps.MainSpeedo9
};

static char spdreg_prev[3];
static float gf_SpeedFactor;

static SensorsDataT SensorsPrev;


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

    gf_SpeedFactor = 60.0 * (float)config.WheelCirc / 1000000.0;

    /* Hall sensor is on motor's rotor */
    if(config.HallOnWheel == false)
        { gf_SpeedFactor /= config.GearRatio; }

    if(config.SpeedUnits == UnitsMph)
        { gf_SpeedFactor /= KILOS_PER_MILE; }
}


static void ScreenShowCb(omScreenT *screen)
{
    SensorsPrev.HallRpm = 0.0;
    SensorsPrev.Volt = 0.0;
    memset(SensorsPrev.Temperature, 0.0, sizeof(SensorsPrev.Temperature));
    memset(spdreg_prev, '0', sizeof(spdreg_prev));

    omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo0, 15+24, 2, false, false);
}


static void ScreenUpdateCb(omScreenT *screen)
{
    for(uint8_t i; i < DS18B20_Quantity(); i++)
    {
        if(Sensors.Temperature[i] != SensorsPrev.Temperature[i])
        {
//SEGGER_RTT_printf(0, "temp[%u]: %u\n", i, (uint16_t)gf_Temperature[i]);
            SensorsPrev.Temperature[i] = Sensors.Temperature[i];
        }
    }

    if(Sensors.HallRpm != SensorsPrev.HallRpm)
    {
        RefreshRpm();
        RefreshSpeed();
    }

    if(Sensors.Volt != SensorsPrev.Volt)
    {
        RefreshVolt();
        RefreshAmpere();
    }
}


static void RefreshRpm(void)
{
    uint16_t rpm = (config.HallOnWheel == true)
        ? (int)roundf(Sensors.HallRpm * config.GearRatio) : Sensors.HallRpm;
}


static void RefreshSpeed(void)
{
    char spd[3];
    //SEGGER_RTT_printf(0, "from screen: %u\n", gf_HallRpm);
    // Hall sensor is on wheel
    float speed = Sensors.HallRpm * gf_SpeedFactor;

    snprintf(spd, sizeof(spd), "%3.0f", speed);

    if(spd[0] != spdreg_prev[0])
    {
        if(spd[0] == '1')
        {
            omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo1XX, 0, 2, false, false);
        }
        else
        {
            omDrawRectangleFilled(&oledUi, 0, 2, 14, 25, OLED_GRAY_00, OLED_GRAY_00, false);
        }
    }

    if(spd[1] != spdreg_prev[1] && spd[1] != ' ')
    {
        omDrawBitmap(&oledUi, SpeedoFont[spd[1] - AsciiShift], 15, 2, false, false);
    }

    if(spd[2] != spdreg_prev[2])
    {
       // omDrawBitmap(&oledUi, SpeedoFont[spd[1] - AsciiShift], 15+24, 2, false, false);
    }

    spdreg_prev[0] = spd[0];
    spdreg_prev[1] = spd[1];
    spdreg_prev[2] = spd[2];
}


static void RefreshVolt(void)
{
    
}


static void RefreshAmpere(void)
{
    
}