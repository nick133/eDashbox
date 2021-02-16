
#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "omgui.h"
#include "ds18b20.h"

#define KILOS_PER_MILE 1.60934


typedef enum {
    UpdateScreenSpeed   = 0b0000000000000001U,
    UpdateScreenTacho   = 0b0000000000000010U,
    UpdateScreenTemp1   = 0b0000000000000100U,
    UpdateScreenTemp2   = 0b0000000000001000U,
    UpdateScreenTemp3   = 0b0000000000010000U,
    UpdateScreenTemp4   = 0b0000000000100000U,
    UpdateScreenVolt    = 0b0000000001000000U,
    UpdateScreenAmpere  = 0b0000000010000000U,
    UpdateScreenPower   = 0b0000000100000000U,
    UpdateScreenOdo     = 0b0000001000000000U,
    UpdateScreenClock   = 0b0000010000000000U
} UpdateScreenFlagT;


typedef enum {
    IdScreenMain,
    IdScreenData,
    IdScreenRpm,
    IdScreenSetup,
    IdScreenSetupEditor,

    IdScreensNumOf
} IdScreenT;

extern omGuiT oledUi;
extern omScreenT screenMain, screenData, screenSetup;
extern omScreenT *uiScreens[4];


extern void Screens_Init(void);
extern void MainScreenInit(void);
//void DataScreenInit(void

extern float SsrGetRpmPerctg(SensorsDataT *);
extern float SsrGetMotorRpm(SensorsDataT *);
extern float SsrGetSpeed(SensorsDataT *);
extern float SsrGetTemprt(SensorsDataT *, uint8_t index);

#endif /* _SCREENS_H_ */