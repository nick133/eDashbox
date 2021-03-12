
#include <string.h>
#include <math.h>
#include "printf.h"

#include "ds18b20.h"
#include "sh1122.h"

#include "main.h"
#include "rtc.h"
#include "rtos.h"

#include "settings.h"
#include "bitmaps.h"
#include "omgui.h"
#include "screens.h"

/******************************************************************************/
#define EVENT_CLOCK_UPDATE 0x00000001U
#define EVENT_SELECT_DST   0x00000010U
#define EVENT_RESET_DST    0x00000100U

#define MAX_DST 9999.9
#define MAX_RPM_BARS 18
#define BAT_PIE_PCS 20
#define ASCII_NUM '0'

#define DRAW_METER_REG_SZ 10
#define DRAW_METER_NONE   0x00
#define DRAW_METER_SIGNED 0x01
#define DRAW_METER_FORCED 0x10

/******************************************************************************/
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

// typedef struct ScreenData {
//     float Temprt[_DS18B20_MAX_SENSORS];
// } ScreenDataT;

// static ScreenDataT ScreenDat;
// static ScreenDataT ScreenDatPrev;

static osThreadId_t ClockUpdateTask;
static osEventFlagsId_t EvtFlagsMain;

/******************************************************************************/
static void ScreenShowCb(omScreenT *);
static void ScreenHideCb(omScreenT *);
static bool ScreenUpdateCb(omScreenT *);

static void DstSelectCb(IdButtonT, BtnEventKindT, void *Params);
static void DstResetCb(IdButtonT, BtnEventKindT, void *Params);

__NO_RETURN static void ClockUpdate(void *);

static void DrawStatic(void);
static bool DrawMeter(const omBitmapT * [], const omBitmapT * [],
    uint8_t isize, const char* format,
    uint32_t ix, uint32_t iy, uint32_t fx, uint32_t fy,
    float num, float numPrev, uint8_t flags);

static bool DrawSpeedo(bool force);
static bool DrawTacho(bool force);
static bool DrawVolt(bool force);
static bool DrawAmpere(bool force);
static bool DrawOdoDst(bool force);
static bool DrawBatPie(bool force);

/******************************************************************************/
void MainScreen_Init(void)
{
    screenMain.Id = IdScreenMain;
    screenMain.Ui = &oledUi;
    screenMain.ShowCallback = ScreenShowCb;
    screenMain.HideCallback = ScreenHideCb;
    screenMain.UpdateCallback = ScreenUpdateCb;
}


static void ScreenShowCb(omScreenT *screen)
{
    DrawStatic();

    /* Draw all meters zero */
    DrawSpeedo(true);
    DrawTacho(true);
    DrawVolt(true);
    DrawAmpere(true);
    DrawOdoDst(true);
    DrawBatPie(true);

    // memset(ScreenDat.Temprt, 0.0, sizeof(ScreenDat.Temprt));
    // memset(ScreenDatPrev.Temprt, 0.0, sizeof(ScreenDatPrev.Temprt));
 //   for(uint8_t i; i < DS18B20_Quantity(); i++)
 //       { RefreshTemprt(i); }

    if(osThreadGetState(ClockUpdateTask) == osThreadBlocked)
    {
        osThreadResume(ClockUpdateTask);
    }
    else
    {
        EvtFlagsMain = osEventFlagsNew(NULL); // Obviously must be before clock update task creation
        ClockUpdateTask = appCreateTask(ClockUpdate, NULL, TASK_NAME("ClockUpdate"));
    }

    RegButtonEvent(IdButton2, EvtButtonPress, DstSelectCb, NULL);
    RegButtonEvent(IdButton2, EvtButtonLongPress, DstResetCb, NULL);
}


static void ScreenHideCb(omScreenT *screen)
{
    osThreadSuspend(ClockUpdateTask);
}


static bool ScreenUpdateCb(omScreenT *screen)
{
    uint8_t is_update = DrawSpeedo(false)
        + DrawTacho(false)
        + DrawVolt(false)
        + DrawAmpere(false)
        + DrawOdoDst(false)
        + DrawBatPie(false);

    // for(uint8_t i; i < DS18B20_Quantity(); i++)
    // {
    //     ScreenDat.Temprt[i] = SsrGetTemprt(&Sensors, i);

    //     if(ScreenDat.Temprt[i] != ScreenDatPrev.Temprt[i])
    //     {
    //         //RefreshTemprt(i);
    //         ScreenDatPrev.Temprt[i] = ScreenDat.Temprt[i];
    //     }
    // }

    if(osEventFlagsGet(EvtFlagsMain) & EVENT_CLOCK_UPDATE)
    {
        osEventFlagsClear(EvtFlagsMain, EVENT_CLOCK_UPDATE);
        is_update++;
    }

    return (is_update > 0);
}


static void DrawStatic(void)
{
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.RpmBars, 3, 36, false, false);

    /* RPM */
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_4, 60, 55, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_1, 68, 55, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_3, 76, 55, false, false);

    /* KPH | MPH, dot */
    if(Config.SpeedUnits == UnitsMph)
    {
        omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_3, 87, 0, false, false);    
    }
    else if(Config.SpeedUnits == UnitsKph)
    {
        omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_0, 87, 0, false, false);    
    }
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_1, 95, 0, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_2, 103, 0, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot4x4, 79, 26, false, false);

    /* Odo/Dst dot */
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot3x3, 179, 61, false, false);

    /* Volt, Ampere dots, units */
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 156, 10, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 156, 27, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainVoltV, 174, 1, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainAmpA, 174, 18, false, false);

    /* Clock dots */
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 231, 3, false, false);
    omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainDot3x2, 231, 10, false, false);
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

    char reg[DRAW_METER_REG_SZ], regPrev[DRAW_METER_REG_SZ];

    snprintf_(reg, sizeof(reg), format, num);

    if(!(flags & DRAW_METER_FORCED))
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
            omGui_DrawRectangleFilled(&oledUi,
                ix + (font_w * i), iy, ix + (font_w * (i + 1)) - 1, iy + font_h,
                OLED_GRAY_00, OLED_GRAY_00, false);
        }
        else
        {
            omGui_DrawBitmap(&oledUi, ifont[reg[i] - ASCII_NUM], ix + (font_w * i), iy, false, false);
        }
    }

    /* Fractional part */
    if(ffont != NULL && ((flags & DRAW_METER_FORCED) || reg[fnumidx] != regPrev[fnumidx]))
    {
        omGui_DrawBitmap(&oledUi, ffont[reg[fnumidx] - ASCII_NUM], fx, fy, false, false);
    }

    return true;
}


static bool DrawSpeedo(bool force)
{
    static float speed, speedPrev = 0.0;

    speed = SsrGetSpeed(&Sensors);

    bool is_update = DrawMeter(Roboto25x30, Roboto14x17, 3, "%5.1f", 0, 0, 87, 13,
        speed, speedPrev, (force ? DRAW_METER_FORCED : DRAW_METER_NONE));

    speedPrev = speed;
    return is_update;
}


static bool DrawVolt(bool force)
{
    static float volt, voltPrev = 0.0;

    volt = Sensors.Volt;

    bool is_update = DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 0, 161, 0,
        volt, voltPrev, (force ? DRAW_METER_FORCED : DRAW_METER_NONE));

    voltPrev = volt;
    return is_update;
}


static bool DrawAmpere(bool force)
{
    static float amps, ampsPrev = 0.0;

    amps = Sensors.Ampere;

    bool is_update = DrawMeter(Roboto10x12, Roboto10x12, 3, "%5.1f", 124, 17, 161, 17,
        amps, ampsPrev, (force ? DRAW_METER_FORCED : DRAW_METER_NONE));

    ampsPrev = amps;
    return is_update;
}


static bool DrawTacho(bool force)
{
    static float rpm, rpmPrev = 0.0;
    static uint8_t nbars, nbarsPrev = 0;

    static const uint8_t RpmBarsColors[] =
    {
        OLED_GRAY_03, OLED_GRAY_03, OLED_GRAY_03, OLED_GRAY_04, OLED_GRAY_05, OLED_GRAY_06,
        OLED_GRAY_07, OLED_GRAY_08, OLED_GRAY_09, OLED_GRAY_10, OLED_GRAY_11, OLED_GRAY_12,
        OLED_GRAY_13, OLED_GRAY_14, OLED_GRAY_15, OLED_GRAY_15, OLED_GRAY_15, OLED_GRAY_15
    };

    /* Digital tacho */
    rpm = SsrGetMotorRpm(&Sensors);    

    bool is_update = DrawMeter(Roboto14x17, NULL, 4, "%4.0f", 0, 47, 0, 0,
        rpm, rpmPrev, (force ? DRAW_METER_FORCED : DRAW_METER_NONE));

    rpmPrev = rpm;

    /* Bars graph */
    nbars = roundf(SsrGetRpmPerctg(&Sensors) * MAX_RPM_BARS / 100.0);

    if(force) { nbarsPrev = 0; } // Redraw bars only if nbars > 0 in force mode

    if(nbars == nbarsPrev) { return is_update; }

    for(uint8_t i = 0; i < ((nbars > nbarsPrev) ? nbars : nbarsPrev); i++)
    {
        uint8_t color;

        if(nbars > nbarsPrev && i >= nbarsPrev && i < nbars)
        {
            /* Add missing bars */
            color = RpmBarsColors[i];
        }
        else if(nbars < nbarsPrev && i >= nbars && i < nbarsPrev)
        {
            /* Clean excess bars */
            color = OLED_GRAY_00;
        }
        else
        {
            continue; // Update only changed bars
        }

        omGui_DrawRectangleFilled(&oledUi, 4 + (i * 14), 37, 13 + (i * 14), 39, color, color, false);
    }

    nbarsPrev = nbars;
    return true;
}


static void DstSelectCb(IdButtonT Btn, BtnEventKindT EvtKind, void *Params)
{
    osEventFlagsSet(EvtFlagsMain, EVENT_SELECT_DST);
}
static void DstResetCb(IdButtonT Btn, BtnEventKindT EvtKind, void *Params)
{
    osEventFlagsSet(EvtFlagsMain, EVENT_RESET_DST);
}


static bool DrawOdoDst(bool force)
{
    static float odo, odoPrev = 0.0, dst, dstPrev = 0.0, dstStart = 0.0;
    static bool odoIsActive = true;
    uint8_t flags = DRAW_METER_NONE;
    bool is_update = false;

    if(!dstStart) { dstStart = Sensors.Odo; }

    if(osEventFlagsGet(EvtFlagsMain) & EVENT_SELECT_DST)
    {
        odoIsActive = !odoIsActive;

        /* No need to clean digits area in force mode (at startup), only here at ODO/DST change */
        omGui_DrawRectangleFilled(&oledUi, 91, 47, 174, 63, OLED_GRAY_00, OLED_GRAY_00, false);
        omGui_DrawRectangleFilled(&oledUi, 186, 47, 199, 63, OLED_GRAY_00, OLED_GRAY_00, false);

        osEventFlagsClear(EvtFlagsMain, EVENT_SELECT_DST);
        force = true;
    }

    if(osEventFlagsGet(EvtFlagsMain) & EVENT_RESET_DST)
    {
        dstStart = Sensors.Odo;
        osEventFlagsClear(EvtFlagsMain, EVENT_RESET_DST);
    }

    if(force)
    {
        if(odoIsActive) // write: ODO
        {
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_5, 204, 55, false, false);
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_6, 212, 55, false, false);
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_5, 220, 55, false, false);        
        }
        else // write: DST
        {
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_6, 204, 55, false, false);
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_7, 212, 55, false, false);
            omGui_DrawBitmap(&oledUi, &AssetBitmaps.MainKphmrodst8x9_8, 220, 55, false, false);
        }

        flags = DRAW_METER_FORCED;
    }

    odo = Sensors.Odo;
    dst = odo - dstStart;

    if(dst > MAX_DST)
    {
        dstStart = odo;
        dst = 0.0;
    }

    is_update = (odoIsActive
        ? DrawMeter(Roboto14x17, Roboto14x17, 6, "%8.1f", 91, 47, 186, 47, odo, odoPrev, flags)
        : DrawMeter(Roboto14x17, Roboto14x17, 4, "%6.1f", 119, 47, 186, 47, dst, dstPrev, flags));

    odoPrev = odo;
    dstPrev = dst;

    return is_update;
}


static bool DrawBatPie(bool force)
{
    static uint8_t batpien, batpienPrev = 0;

    batpien = SsrGetBatPerctg(&Sensors) / (100 / BAT_PIE_PCS); // implicit cast to int

    if(force) { batpienPrev = batpien + 1; }

    if(batpien == batpienPrev || batpien >= BAT_PIE_PCS) { return false; }

    omGui_DrawBitmap(&oledUi, BatPie14x14[batpien], 238, 46, false, false);

    batpienPrev = batpien;
    return true;
}


__NO_RETURN static void ClockUpdate(void *params)
{
    static RTC_TimeTypeDef timeRtc, timeRtcPrev;
    RTC_DateTypeDef dateRtc;
    uint8_t flags = DRAW_METER_FORCED;
 
    timeRtcPrev.Hours = timeRtcPrev.Minutes = 0;

    do {
        /* STM32 HAL Manual: "You must call HAL_RTC_GetDate() after HAL_RTC_GetTime()
         * to unlock the values in the higher-order calendar shadow registers to
         * ensure consistency between the time and date values. Reading RTC current
         * time locks the values in calendar shadow registers until current date is read."
         */
        HAL_RTC_GetTime(&hrtc, &timeRtc, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN);

        DrawMeter(Roboto10x12, NULL, 2, "%02.0f", 209, 0, 0, 0,
            (float)timeRtc.Hours, (float)timeRtcPrev.Hours, flags);

        DrawMeter(Roboto10x12, NULL, 2, "%02.0f", 236, 0, 0, 0,
            (float)timeRtc.Minutes, (float)timeRtcPrev.Minutes, flags);

        if(flags) { flags = 0; }

        timeRtc = timeRtcPrev;

        osEventFlagsSet(EvtFlagsMain, EVENT_CLOCK_UPDATE);
    } while(osDelay(1000) == osOK);

    osThreadExit();
}