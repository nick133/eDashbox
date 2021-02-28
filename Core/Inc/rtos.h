
#ifndef _RTOS_H_
#define _RTOS_H_

#include "FreeRTOS.h"
#include "task.h"

#define TASK_NAME(t)    &((osThreadAttr_t){ .name = t })

typedef enum
{
    EvtButtonPress,
    EvtButtonLongPress,
    EvtButtonNumOf
} BtnEventKindT;

extern osThreadId_t appCreateTask(osThreadFunc_t, void *, osThreadAttr_t *);

extern bool RegButtonEvent(uint8_t Btn, BtnEventKindT EvtKind, void (*EvtCallback)(void *), void *);
extern bool UnRegButtonEvent(uint8_t Btn, BtnEventKindT EvtKind);

#endif /* _RTOS_H_ */