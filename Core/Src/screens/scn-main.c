
#include <string.h>
#include <math.h>
#include "printf.h"

#include "ds18b20.h"
#include "sh1122.h"

#include "main.h"
#include "rtc.h"

#include "settings.h"
#include "bitmaps.h"
#include "omgui.h"
#include "screens.h"

/*******************************************************************************
 ** Defines and macros
 */
#define MAX_RPM_BARS 18
#define BAT_PIE_PCS 20
#define ASCII_NUM '0'

#define METER_REG_SZ 10
#define DRAW_METER_SIGNED 0x01
#define DRAW_METER_FORCED 0x10

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
    &AssetBitmaps.Roboto25x30_9,
};

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

static const omBitmapT *Roboto10x12[] = {
    &AssetBitmaps.Roboto10x12_0,
    &AssetBitmaps.Roboto10x12_1,
    &AssetBitmaps.Roboto10x12_2,
    &AssetBitmaps.Roboto10x12_3,
    &AssetBitmaps.Roboto10x12_4,
    &AssetBitmaps.Roboto10x12_5,
    &AssetBitmaps.Roboto10x12_6,
    &AssetBitmaps.Roboto10x12_7,
    &AssetBitmaps.Roboto10x12_8,
    &AssetBitmaps.Roboto10x12_9
};

static const omBitmapT *BatPie14x14[] = {
    &AssetBitmaps.BatPie14x14_0,
    &AssetBitmaps.BatPie14x14_1,
    &AssetBitmaps.BatPie14x14_2,
    &AssetBitmaps.BatPie14x14_3,
    &AssetBitmaps.BatPie14x14_4,
    &AssetBitmaps.BatPie14x14_5,
    &AssetBitmaps.BatPie14x14_6,
    &AssetBitmaps.BatPie14x14_7,
    &AssetBitmaps.BatPie14x14_8,
    &AssetBitmaps.BatPie14x14_9,
    &AssetBitmaps.BatPie14x14_10,
    &AssetBitmaps.BatPie14x14_11,
    &AssetBitmaps.BatPie14x14_12,
    &AssetBitmaps.BatPie14x14_13,
    &AssetBitmaps.BatPie14x14_14,
    &AssetBitmaps.BatPie14x14_15,
    &AssetBitmaps.BatPie14x14_16,
    &AssetBitmaps.BatPie14x14_17,
    &AssetBitmaps.BatPie14x14_18,
    &AssetBitmaps.BatPie14x14_19
};

typedef struct ScreenData {
    float Speed;
    float Rpm;
    float Odo;
    float Temprt[_DS18B20_MAX_SENSORS];
    float Volt;
    float Ampere;
    uint8_t RpmBarsN;
    uint8_t BatPieN;
    RTC_TimeTypeDef Time;
} ScreenDataT;

static ScreenDataT ScreenDat;
static ScreenDataT ScreenDatPrev;

static osThreadId_t ClockUpdateTask;

/*******************************************************************************
 ** Functions declaration
 */
static void ScreenShowCb(omScreenT *);
static void ScreenHideCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *);

__NO_RETURN static void ClockUpdate(void *);

static void DrawStatic(void);
static bool DrawMeter(const omBitmapT *ifont[], const omBitmapT *ffont[],
    uint8_t isize, const char* format,
    uint32_t ix, uint32_t iy, uint32_t fx, uint32_t fy,
    float num, float numPrev, uint8_t flags);
static bool DrawRpmBars(uint8_t nbars, uint8_t nbarsPrev);
static bool DrawBatPie(uint8_t batpien, uint8_t batpienPrev);

/*******************************************************************************
 ** Functions definition
 */
void MainScreenInit(void)
{
    screenMain.Id = IdScreenMain;
    screenMain.Ui = &oledUi;
    screenMain.ShowCallback = ScreenShowCb;
    screenMain.HideCallback = ScreenHideCb;
    screenMain.UpdateCallback = ScreenUpdateCb;
}


static void ScreenShowCb(omScreenT *screen)
{
    ScreenDat.Speed = ScreenDatPrev.Speed
        = ScreenDat.Rpm = ScreenDatPrev.Rpm
        = ScreenDatPrev.Odo
        = ScreenDat.Volt = ScreenDatPrev.Volt
        = ScreenDat.Ampere = ScreenDatPrev.Ampere
        = 0.0;

    ScreenDat.RpmBarsN = ScreenDatPrev.RpmBarsN
        = ScreenDat.BatPieN = ScreenDatPrev.BatPieN
        = 0;

    ScreenDat.Odo = Sensors.Odo;

    // for ClockUpdate task start and resume
    ScreenDatPrev.Time.Hours = ScreenDatPrev.Time.Minutes = 0;

    memset(ScreenDat.Temprt, 0.0, sizeof(ScreenDat.Temprt));
    memset(ScreenDatPrev.Temprt, 0.0, sizeof(ScreenDatPrev.Temprt));

    DrawStatic();

    /* Draw all meters zero */
    // Speedo
    DrawMeter(Roboto25x30, Roboto14x17, 3, "%5.1f", 0, 0, 87, 13, 0.0, 0.0, DRAW_METER_FORCED);

    // RPM
    DrawMeter(Roboto14x17, NULL, 4, "%4.0f", 0, 47, 0, 0, 0.0, 0.0, DRAW_METER_FORCED);

    // Volt
    DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 0, 161, 0, ScreenDat.Volt, 0.0, DRAW_METER_FORCED);

    // Ampere
    DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 17, 161, 17, ScreenDat.Ampere, 0.0, DRAW_METER_FORCED);

    // Odo
    DrawMeter(Roboto14x17, Roboto14x17, 6, "%8.1f", 95, 47, 190, 47, ScreenDat.Odo, 0.0, DRAW_METER_FORCED);

    // Battery pie chart
    DrawBatPie(0, 1);

 //   for(uint8_t i; i < DS18B20_Quantity(); i++)
 //       { RefreshTemprt(i); }

    if (osThreadGetState(ClockUpdateTask) == osThreadBlocked)
    {
        osThreadResume(ClockUpdateTask);
    }
    else
    {
        ClockUpdateTask = osThreadNew(ClockUpdate, NULL, NULL);
    }
}


static void ScreenHideCb(omScreenT *screen)
{
    osThreadSuspend(ClockUpdateTask);
}


static void ScreenUpdateCb(omScreenT *screen)
{
    ScreenDat.Speed = SsrGetSpeed(&Sensors);
    ScreenDat.Rpm = SsrGetMotorRpm(&Sensors);
    ScreenDat.RpmBarsN = roundf(SsrGetRpmPerctg(&Sensors) * MAX_RPM_BARS / 100.0);
    ScreenDat.BatPieN = SsrGetBatPerctg(&Sensors) / (100 / BAT_PIE_PCS); // implicit cast to int
    ScreenDat.Volt = Sensors.Volt;
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
    DrawMeter(Roboto25x30, Roboto14x17, 3, "%5.1f", 0, 0, 87, 13,
        ScreenDat.Speed, ScreenDatPrev.Speed, 0);

    // RPM
    DrawMeter(Roboto14x17, NULL, 4, "%4.0f", 0, 47, 0, 0,
        ScreenDat.Rpm, ScreenDatPrev.Rpm, 0);
    DrawRpmBars(ScreenDat.RpmBarsN, ScreenDatPrev.RpmBarsN);

    // Volt
    DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 0, 161, 0,
        ScreenDat.Volt, ScreenDatPrev.Volt, 0);

    // Ampere
//    DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 17, 161, 17,
//        ScreenDat.Ampere, ScreenDatPrev.Ampere, 0);

    // Odo
    DrawMeter(Roboto14x17, Roboto14x17, 6, "%8.1f", 95, 47, 190, 47,
        ScreenDat.Odo, ScreenDatPrev.Odo, 0);

    // Battery pie chart
    DrawBatPie(ScreenDat.BatPieN, ScreenDatPrev.BatPieN);

    ScreenDatPrev.Speed = ScreenDat.Speed;
    ScreenDatPrev.Rpm = ScreenDat.Rpm;
    ScreenDatPrev.RpmBarsN = ScreenDat.RpmBarsN;
    ScreenDatPrev.Volt = ScreenDat.Volt;
    ScreenDatPrev.Odo = ScreenDat.Odo;
    ScreenDatPrev.BatPieN = ScreenDat.BatPieN;
}


static void DrawStatic(void)
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

    /* Volt, Ampere dots, units */
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 156, 10, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 156, 27, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainVoltV, 174, 1, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainAmpA, 174, 18, false, false);

    /* Clock dots */
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 231, 3, false, false);
    omDrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 231, 10, false, false);
}


static bool DrawMeter(const omBitmapT *ifont[], const omBitmapT *ffont[],
    uint8_t isize, const char* format,
    uint32_t ix, uint32_t iy, uint32_t fx, uint32_t fy,
    float num, float numPrev, uint8_t flags)
{
    if(num == numPrev && !(flags & DRAW_METER_FORCED)) { return false; }

    uint32_t font_w = ifont[0]->Width;
    uint32_t font_h = ifont[0]->Height;

    uint8_t fnumidx = isize + 1;

    char reg[METER_REG_SZ], regPrev[METER_REG_SZ];

    snprintf_(reg, sizeof(reg), format, num);

    if (!(flags & DRAW_METER_FORCED))
        { snprintf_(regPrev, sizeof(regPrev), format, numPrev); }

    for(uint8_t i = 0; i < isize; i++)
    {
        /* Update only changed digits.
         * No need to clean prev. number in forced mode (first run)
         */
        if((!(flags & DRAW_METER_FORCED) && reg[i] == regPrev[i])
                || ((flags & DRAW_METER_FORCED) && reg[i] == ' '))
            { continue; }
 
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
    if(ffont != NULL && ((flags & DRAW_METER_FORCED) || reg[fnumidx] != regPrev[fnumidx]))
    {
        omDrawBitmap(&oledUi, ffont[reg[fnumidx] - ASCII_NUM], fx, fy, false, false);
    }

    return true;
}


static bool DrawRpmBars(uint8_t nbars, uint8_t nbarsPrev)
{
    if(nbars == nbarsPrev) { return false; }

    static const uint8_t RpmBarsColors[] = {
        OLED_GRAY_03, OLED_GRAY_03, OLED_GRAY_03, OLED_GRAY_04, OLED_GRAY_05, OLED_GRAY_06,
        OLED_GRAY_07, OLED_GRAY_08, OLED_GRAY_09, OLED_GRAY_10, OLED_GRAY_11, OLED_GRAY_12,
        OLED_GRAY_13, OLED_GRAY_14, OLED_GRAY_15, OLED_GRAY_15, OLED_GRAY_15, OLED_GRAY_15
    };

    uint8_t reg[MAX_RPM_BARS] = {0};
    uint8_t regPrev[MAX_RPM_BARS] = {0};
    uint8_t i;

/******!!! FIXME !!! algo is dumb, rework!! *******/

    for(i = 0; i < nbars; i++) { reg[i] = 1; }
    for(i = 0; i < nbarsPrev; i++) { regPrev[i] = 1; }

    for(i = 0; i < MAX_RPM_BARS; i++)
    {
        if(reg[i] == regPrev[i]) { continue; }

        /* nothing left to redraw */
        if((!reg[i] && (nbars >= nbarsPrev)) || (!regPrev[i] && (nbars <= nbarsPrev))) { break; }

        uint8_t color = reg[i] ? RpmBarsColors[i] : OLED_GRAY_00;

        omDrawRectangleFilled(&oledUi, 4 + (i * 14), 37, 13 + (i * 14), 39, color, color, false);
    }

    return true;
}


static bool DrawBatPie(uint8_t batpien, uint8_t batpienPrev)
{
    if(batpien == batpienPrev || batpien > BAT_PIE_PCS - 1) { return false; }

    omDrawBitmap(&oledUi, BatPie14x14[batpien], 238, 46, false, false);

    return true;
}


__NO_RETURN static void ClockUpdate(void *params)
{
    RTC_DateTypeDef dateRtc;
    uint8_t flags = DRAW_METER_FORCED;
 
    do {
       /* STM32 HAL Manual: "You must call HAL_RTC_GetDate() after HAL_RTC_GetTime()
         * to unlock the values in the higher-order calendar shadow registers to
         * ensure consistency between the time and date values. Reading RTC current
         * time locks the values in calendar shadow registers until current date is read."
         */
        HAL_RTC_GetTime(&hrtc, &(ScreenDat.Time), RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN);

        DrawMeter(Roboto10x12, NULL, 2, "%02.0f", 209, 0, 0, 0,
            (float)ScreenDat.Time.Hours, (float)ScreenDat.Time.Hours, flags);

        DrawMeter(Roboto10x12, NULL, 2, "%02.0f", 236, 0, 0, 0,
            (float)ScreenDat.Time.Minutes, (float)ScreenDatPrev.Time.Minutes, flags);

        if(flags & DRAW_METER_FORCED) { flags = 0; }

        ScreenDatPrev.Time = ScreenDat.Time;
    } while(osDelay(1000) == osOK);
}