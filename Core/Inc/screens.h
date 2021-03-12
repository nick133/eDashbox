
#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "omgui.h"
#include "ds18b20.h"

#define KILOS_PER_MILE 1.60934


typedef enum
{
    IdScreenMain,
    IdScreenData,
    IdScreenStats,
    IdScreenSettings,
    IdScreenNumEditor,

    IdScreensNumOf
} IdScreenT;

extern omGuiT oledUi;
extern omScreenT screenMain, screenData, screenSetup;
extern omScreenT *uiScreens[4];


extern void Screens_Init(void);
extern void MainScreen_Init(void);
extern void NumEditScreen_Init(void);

extern float SsrGetRpmPerctg(SensorsDataT *);
extern float SsrGetMotorRpm(SensorsDataT *);
extern float SsrGetSpeed(SensorsDataT *);
extern float SsrGetTemprt(SensorsDataT *, uint8_t index);
extern float SsrGetBatPerctg(SensorsDataT *data);

#endif /* _SCREENS_H_ */