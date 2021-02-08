
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

extern omDisplayT oled1;
extern omScreenT screenMain, screenData, screenTemp, screenSetup;

void MainScreenInit(void);
void DataScreenInit(void);
void GUI_Init(void);

#endif /* _SCREENS_H_ */
