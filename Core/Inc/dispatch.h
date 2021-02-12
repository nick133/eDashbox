
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

/* Sensors, Buttons message <<
 * Usage: 
 *   SensorMessageT mesg = {
 *     .Kind = SsrMsgHall,
 *     .Data.HallRpm = 2500 
 *   };
 */
typedef enum SensorMessageKind {
    SsrMsgTmp1,
    SsrMsgTmp2,
    SsrMsgTmp3,
    SsrMsgHall,
    SsrMsgBtn1,
    SsrMsgBtn2,
} SensorMessageKindT;

typedef enum ButtonState {
    BtnPressed, BtnReleased
} ButtonStateT;

typedef union SensorMessageData {
    float Temperature1; // Celsius Degree
    float Temperature2;
    float Temperature3;
    uint16_t HallRpm;
    ButtonStateT Button1;
    ButtonStateT Button2;
} SensorMessageDataT;

typedef struct SensorMessage {
    SensorMessageKindT Kind;
    SensorMessageDataT Data;
} SensorMessageT;

/* >> Sensors, Buttons message */

extern omGuiT oledUi;
extern omScreenT screenMain, screenData, screenTemp, screenSetup;

void Dispatch_Init(void);

void MainScreenInit(void);
void DataScreenInit(void);

#endif /* _SCREENS_H_ */
