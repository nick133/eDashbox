
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

/* Array size must not be a 'const', as compiler can't guess from 'const' and uses 0 */
#define RpmDisplRegs   4 // i.e. 4500
#define SpeedDisplRegs 5 // i.e. 123.5 (including dot)


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


static void ScreenShowCb(omScreenT *);
static void ScreenUpdateCb(omScreenT *);
static void RefreshOdo(void);
//static bool RefreshRpm(float rpm, float rpmPrev);
static bool DrawSpeed(uint32_t x, uint32_t y, float speed, float speedPrev);
static void RefreshBars(void);
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
    ScreenDat.Speed = ScreenDatPrev.Speed
        = ScreenDat.Rpm = ScreenDatPrev.Rpm
        = ScreenDat.Odo = ScreenDatPrev.Odo
        = ScreenDat.Volt = ScreenDatPrev.Volt
        = ScreenDat.Ampere = ScreenDatPrev.Ampere
        = 0.0;
    memset(ScreenDat.Temprt, 0.0, sizeof(ScreenDat.Temprt));
    memset(ScreenDatPrev.Temprt, 0.0, sizeof(ScreenDatPrev.Temprt));

    //omDrawBitmap(&oledUi, &AssetBitmaps.MainSpeedo0, 15+24, 2, false, false);

    /* Force update to zero */
    //RefreshRpm(0.0, 9999.0);
    //RefreshSpeed(0.0, 199.9);
    RefreshBars();

    RefreshVolt();
    RefreshAmpere();

    for(uint8_t i; i < DS18B20_Quantity(); i++)
        { RefreshTemprt(i); }
}


static void ScreenUpdateCb(omScreenT *screen)
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

    //RefreshSpeed(ScreenDat.Speed, ScreenDatPrev.Speed);
    //RefreshRpm(ScreenDat.Rpm, ScreenDatPrev.Rpm);
    RefreshBars();

    // if(ScreenDat.Odo != ScreenDatPrev.Odo)
    // {
    //     RefreshOdo();

    //     ScreenDatPrev.Odo = ScreenDat.Odo;
    // }

    // if(ScreenDat.Volt != ScreenDatPrev.Volt)
    // {
    //     RefreshVolt();
    //     RefreshAmpere();
        
    //     ScreenDatPrev.Volt = ScreenDat.Volt;
    // }

    ScreenDatPrev.Speed = ScreenDat.Speed;
    ScreenDatPrev.Rpm = ScreenDat.Rpm;
    ScreenDatPrev.RpmPerctg = ScreenDat.Rpm;
    ScreenDatPrev.Odo = ScreenDat.Odo;
}


static void RefreshOdo(void)
{

}

/* Update only changed digits */

/* Update RPM */
// static bool RefreshRpm(float rpm, float rpmPrev)
// {
//     if(rpm == rpmPrev) { return false; }

//     static const uint32_t rpmPosX = 2;
//     static const uint32_t rpmPosY = 47;

//     uint32_t fntWidth = RpmFont[0]->Width;
//     uint32_t fntHeight = RpmFont[0]->Height;

//     char reg[RpmDisplRegs+1], regPrev[RpmDisplRegs+1];

//     snprintf_(reg, sizeof(reg), "%4.0f", rpm);
//     snprintf_(regPrev, sizeof(regPrev), "%4.0f", rpmPrev);

//     for(uint8_t i = 0; i < RpmDisplRegs; i++)
//     {
//         if(reg[i] == regPrev[i]) { continue; }

//         if(reg[i] == ' ')
//         {
//             omDrawRectangleFilled(&oledUi,
//                 rpmPosX+(fntWidth * i), rpmPosY, rpmPosX+fntWidth+(fntWidth * i)-1, rpmPosY+fntHeight-1,
//                 OLED_GRAY_00, OLED_GRAY_00, true); //// false!
//         }
//         else
//         {
//             omDrawBitmap(&oledUi, RpmFont[reg[i] - '0'], rpmPosX+(fntWidth * i), rpmPosY, false, false);
//         }
//     }

//     return true;
// }


    /* Update Speed */
// s = 160.4
// x = s*10.0 - (int)s*10;
bool DrawSpeed(uint32_t x, uint32_t y, float speed, float speedPrev)
{
    if(speed == speedPrev) { return false; }

    static const uint32_t speedPosX = 0;
    static const uint32_t speedPosY = 2;

    uint32_t fntWidth = Roboto25x30[0]->Width;
    uint32_t fntHeight = Roboto25x30[0]->Height;
//    uint32_t fntWidth1XX = Roboto25x30[10]->Width;

    char reg[SpeedDisplRegs+1], regPrev[SpeedDisplRegs+1];

    snprintf_(reg, sizeof(reg), "%5.1f", speed);
    snprintf_(regPrev, sizeof(regPrev), "%5.1f", speedPrev);
    //SEGGER_RTT_printf(0,"RPM: %d, prev: %d\n", (int)rpm, (int)rpmPrev);

    // if(reg[0] != regPrev[0])
    // {
    //     if(reg[0] == ' ')
    //     {
    //         omDrawRectangleFilled(&oledUi,
    //             speedPosX, speedPosY, speedPosX+fntWidth1XX-1, speedPosY+fntHeight,
    //             OLED_GRAY_00, OLED_GRAY_00, false);
    //     }
    //     else
    //     {
    //         omDrawBitmap(&oledUi, Roboto25x30[10], speedPosX, speedPosY, false, false);
    //     }
    // }

    // for(uint8_t i = 1; i < 3; i++)
    // {
    //     if(reg[i] == regPrev[i]) { continue; }

    //     if(reg[i] == ' ')
    //     {
    //         omDrawRectangleFilled(&oledUi,
    //             speedPosX+fntWidth1XX+(fntWidth * (i-1)), speedPosY, speedPosX+fntWidth1XX+(fntWidth * i)-1, speedPosY+fntHeight,
    //             OLED_GRAY_00, OLED_GRAY_00, false);
    //     }
    //     else
    //     {
    //         omDrawBitmap(&oledUi, Roboto25x30[reg[i] - '0'], speedPosX+(fntWidth * i), speedPosY, false, false);
    //     }
    // }

    return true;
}


static void RefreshBars(void)
{

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
