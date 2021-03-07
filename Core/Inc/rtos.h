
#ifndef _RTOS_H_
#define _RTOS_H_

#include "FreeRTOS.h"
#include "task.h"

#define NUM_BUTTONS 2U

#define MAX(x, y)   ((x > y) ? x : y)

#define TASK_NAME(t)    &((osThreadAttr_t){ .name = t })

typedef enum
{
    EvtButtonPress,
    EvtButtonLongPress,
    EvtButtonEmpty
} BtnEventKindT;

typedef void (*BtnEventCallbackT)(uint8_t Btn, BtnEventKindT EvtKind, void *Params);

extern osThreadId_t appCreateTask(osThreadFunc_t, void *Params, osThreadAttr_t *);
extern bool RegButtonEvent(uint8_t Btn, BtnEventKindT EvtKind, BtnEventCallbackT EvtCallback, void *Params);
extern bool UnRegButtonEvent(uint8_t Btn, BtnEventKindT EvtKind);

#endif /* _RTOS_H_ */