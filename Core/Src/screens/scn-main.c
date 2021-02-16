
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

static char speedreg_prev[3];


typedef struct ScreenData {
    float Speed;
    float Rpm;
    float Odo;
    float Temprt[_DS18B20_MAX_SENSORS];
    float Volt;
    float Ampere;
    float RpmPerctg;
} ScreenDataT;

static ScreenDataT ScreenDat;
static ScreenDataT ScreenDatPrev;


static void ScreenShowCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *, uint32_t flags);
static void RefreshOdo(void);
static void RefreshRpm(void);
static void RefreshBars(void);
static void RefreshSpeed(void);
static void RefreshTemprt(uint8_t);
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
    ScreenDatPrev.Rpm = 0.0;
    ScreenDatPrev.Odo = 0.0;
    ScreenDatPrev.Volt = 0.0;
    ScreenDatPrev.Ampere = 0.0;
    memset(ScreenDatPrev.Temprt, 0.0, sizeof(ScreenDatPrev.Temprt));
    memset(speedreg_prev, '0', sizeof(speedreg_prev));

    omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo0, 15+24, 2, false, false);

    RefreshRpm();
    RefreshBars();
    RefreshSpeed();
    RefreshOdo();
}


static void ScreenUpdateCb(omScreenT *screen, uint32_t flags)
{
    ScreenDat.Speed = SsrGetSpeed(&Sensors);
    ScreenDat.Rpm = SsrGetMotorRpm(&Sensors);
    ScreenDat.RpmPerctg = SsrGetRpmPerctg(&Sensors);
    ScreenDat.Odo = Sensors.Odo;

    for(uint8_t i; i < DS18B20_Quantity(); i++)
    {
        ScreenDat.Temprt[i] = SsrGetTemprt(&Sensors, i);

        if(ScreenDat.Temprt[i] != ScreenDatPrev.Temprt[i])
        {
            RefreshTemprt(i);
            ScreenDatPrev.Temprt[i] = ScreenDat.Temprt[i];
        }
    }

    if(ScreenDat.Rpm != ScreenDatPrev.Rpm)
    {
        RefreshRpm();
        RefreshBars();
        RefreshSpeed();

        ScreenDatPrev.Rpm = ScreenDat.Rpm;
    }

    if(ScreenDat.Odo != ScreenDatPrev.Odo)
    {
        RefreshOdo();

        ScreenDatPrev.Odo = ScreenDat.Odo;
    }

    if(ScreenDat.Volt != ScreenDatPrev.Volt)
    {
        RefreshVolt();
        RefreshAmpere();
        
        ScreenDatPrev.Volt = ScreenDat.Volt;
    }

// char buf[10];
// snprintf(buf, 10, "%f", Sensors.HallRpm);
// SEGGER_RTT_printf(0, "task->RPM: %s\n", buf);
}


static void RefreshOdo(void)
{

}


static void RefreshRpm(void)
{

}


static void RefreshBars(void)
{

}


static void RefreshSpeed(void)
{
    char spd[3];
    float speed = ScreenDat.Speed;

    snprintf(spd, sizeof(spd), "%3.0f", speed);

    if(spd[0] != speedreg_prev[0])
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

    if(spd[1] != speedreg_prev[1] && spd[1] != ' ')
    {
        omDrawBitmap(&oledUi, SpeedoFont[spd[1] - AsciiShift], 15, 2, false, false);
    }

    if(spd[2] != speedreg_prev[2])
    {
       // omDrawBitmap(&oledUi, SpeedoFont[spd[1] - AsciiShift], 15+24, 2, false, false);
    }

    speedreg_prev[0] = spd[0];
    speedreg_prev[1] = spd[1];
    speedreg_prev[2] = spd[2];
}


static void RefreshVolt(void)
{
    
}


static void RefreshAmpere(void)
{
    
}


static void RefreshTemprt(uint8_t index)
{


}