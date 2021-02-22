
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

/*******************************************************************************
 ** Defines and macros
 */

/* Array size must not be a 'const', as compiler can't guess from 'const' and uses 0 */
#define RpmDisplRegs   4 // i.e. 4500
#define SpeedDisplRegs 5 // i.e. 123.5 (including dot)

#define METER_REG_SZ 10
#define ASCII_NUM '0'

/*******************************************************************************
 ** Global variables and types
 */
/* Speedo font */
static const omBitmapT *Roboto25x30[] = {
    &AssetBitmaps.Roboto25x30_0,
    &AssetBitmaps.Roboto25x30_1,
    &AssetBitmaps.Roboto25x30_2,
    &AssetBitmaps.Roboto25x30_3,
    &AssetBitmaps.Roboto25x30_4,
    &AssetBitmaps.Roboto25x30_5,
    &AssetBitmaps.Roboto25x30_6,
    &AssetBitmaps.Roboto25x30_7,
    &AssetBitmaps.Roboto25x30_8,
    &AssetBitmaps.Roboto25x30_9
};

/* Tacho/Odo font */
static const omBitmapT *Roboto14x17[] = {
    &AssetBitmaps.Roboto14x17_0,
    &AssetBitmaps.Roboto14x17_1,
    &AssetBitmaps.Roboto14x17_2,
    &AssetBitmaps.Roboto14x17_3,
    &AssetBitmaps.Roboto14x17_4,
    &AssetBitmaps.Roboto14x17_5,
    &AssetBitmaps.Roboto14x17_6,
    &AssetBitmaps.Roboto14x17_7,
    &AssetBitmaps.Roboto14x17_8,
    &AssetBitmaps.Roboto14x17_9
};

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

/*******************************************************************************
 ** Functions declaration
 */
static void ScreenShowCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *);
static void DrawStatic(void);
static bool DrawMeter(const omBitmapT *ifont[], const omBitmapT *ffont[],
    uint8_t isize, const char* format,
    uint32_t ix, uint32_t iy, uint32_t fx, uint32_t fy,
    float num, float numPrev, bool sign);

/*******************************************************************************
 ** Functions definition
 */
void MainScreenInit(void)
{
    screenMain.Id = IdScreenMain;
    screenMain.Ui = &oledUi;
    screenMain.ShowCallback = ScreenShowCb;
    screenMain.HideCallback = NULL;
    screenMain.UpdateCallback = ScreenUpdateCb;
}


void ScreenShowCb(omScreenT *screen)
{
    ScreenDat.Speed = ScreenDatPrev.Speed
        = ScreenDat.Rpm = ScreenDatPrev.Rpm
        = ScreenDatPrev.Odo
        = ScreenDat.Volt = ScreenDatPrev.Volt
        = ScreenDat.Ampere = ScreenDatPrev.Ampere
        = 0.0;
    ScreenDat.Odo = Sensors.Odo;
    memset(ScreenDat.Temprt, 0.0, sizeof(ScreenDat.Temprt));
    memset(ScreenDatPrev.Temprt, 0.0, sizeof(ScreenDatPrev.Temprt));

    DrawStatic();

    /* Force update to zero */
    // Speedo
    DrawMeter(Roboto25x30, Roboto14x17, 3, "%5.1f", 0, 0, 87, 13, 0.0, 999.9, false);
    // RPM
    DrawMeter(Roboto14x17, NULL, 4, "%4.0f", 0, 47, 0, 0, 0.0, 9999.0, false);
    // Odo
    DrawMeter(Roboto14x17, Roboto14x17, 6, "%8.1f", 95, 47, 190, 47, ScreenDat.Odo, ScreenDatPrev.Odo, false);

 //   for(uint8_t i; i < DS18B20_Quantity(); i++)
 //       { RefreshTemprt(i); }
}


void ScreenUpdateCb(omScreenT *screen)
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
            //RefreshTemprt(i);
            ScreenDatPrev.Temprt[i] = ScreenDat.Temprt[i];
        }
    }

    // Speedo
    DrawMeter(Roboto25x30, Roboto14x17, 3, "%5.1f", 0, 0, 87, 13, ScreenDat.Speed, ScreenDatPrev.Speed, false);
    // RPM
    DrawMeter(Roboto14x17, NULL, 4, "%4.0f", 0, 47, 0, 0, ScreenDat.Rpm, ScreenDatPrev.Rpm, false);
    // Odo
    DrawMeter(Roboto14x17, Roboto14x17, 6, "%8.1f", 95, 47, 190, 47, ScreenDat.Odo, ScreenDatPrev.Odo, false);

    ScreenDatPrev.Speed = ScreenDat.Speed;
    ScreenDatPrev.Rpm = ScreenDat.Rpm;
    ScreenDatPrev.RpmPerctg = ScreenDat.Rpm;
    ScreenDatPrev.Odo = ScreenDat.Odo;
}


void DrawStatic(void)
{
    omDrawBitmap(&oledUi, &AssetBitmaps.RpmBars, 3, 36, false, false);

    /* RPM */
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_4, 60, 55, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_1, 68, 55, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_3, 76, 55, false, false);

    /* KPH | MPH, dot */
    if(Config.SpeedUnits == UnitsMph)
    {
        omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_3, 87, 0, false, false);    
    }
    else if(Config.SpeedUnits == UnitsKph)
    {
        omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_0, 87, 0, false, false);    
    }
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_1, 95, 0, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_2, 103, 0, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot4x4, 79, 26, false, false);

    /* KM (Odo), dot */
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot3x3, 183, 61, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_0, 208, 55, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainKphmr8x9_3, 216, 55, false, false);
}


bool DrawMeter(const omBitmapT *ifont[], const omBitmapT *ffont[],
    uint8_t isize, const char* format,
    uint32_t ix, uint32_t iy, uint32_t fx, uint32_t fy,
    float num, float numPrev, bool sign)
{
    if(num == numPrev) { return false; }

    uint32_t font_w = ifont[0]->Width;
    uint32_t font_h = ifont[0]->Height;

    uint8_t fnumidx = isize + 1;

    char reg[METER_REG_SZ], regPrev[METER_REG_SZ];

    snprintf_(reg, sizeof(reg), format, num);
    snprintf_(regPrev, sizeof(regPrev), format, numPrev);

    for(uint8_t i = 0; i < isize; i++)
    {
        /* Update only changed digits */
        if(reg[i] == regPrev[i]) { continue; }

        if(reg[i] == ' ')
        {
            omDrawRectangleFilled(&oledUi,
                ix + (font_w * i), iy, ix + (font_w * (i + 1)) - 1, iy + font_h,
                OLED_GRAY_00, OLED_GRAY_00, false);
        }
        else
        {
            omDrawBitmap(&oledUi, ifont[reg[i] - ASCII_NUM], ix + (font_w * i), iy, false, false);
        }
    }

    /* Fractional part */
    if(ffont != NULL && reg[fnumidx] != regPrev[fnumidx]) 
    {
        omDrawBitmap(&oledUi, ffont[reg[fnumidx] - ASCII_NUM], fx, fy, false, false);
    }

    return true;
}