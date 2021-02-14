
#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "omgui.h"
#include "ds18b20.h"


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

void Screens_Init(void);
void MainScreenInit(void);
//void DataScreenInit(void);

#endif /* _SCREENS_H_ */