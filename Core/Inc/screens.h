
#ifndef _SCREENS_H_
#define _SCREENS_H_

#include "omgui.h"


typedef enum {
    IdScreenLogo,
    IdScreenMain,
    IdScreenData,
    IdScreenTemp,
    IdScreenSetup,
    IdScreensNumOf
} omScreenIdT;

extern omGuiT oledUi;
extern omScreenT screenMain, screenData, screenTemp, screenSetup;

void Screens_Init(void);

void MainScreenInit(void);
//void DataScreenInit(void);

#endif /* _SCREENS_H_ */
