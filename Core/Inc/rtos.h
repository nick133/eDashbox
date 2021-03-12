
#ifndef _RTOS_H_
#define _RTOS_H_

#include "FreeRTOS.h"
#include "task.h"

#define NUM_BUTTONS 2U

#define TASK_NAME(t)    &((osThreadAttr_t){ .name = t })

typedef enum
{
    IdButton1,
    IdButton2,
} IdButtonT;

typedef enum
{
    EvtButtonPress,
    EvtButtonLongPress,
    EvtButtonEmpty
} BtnEventKindT;

typedef void (*BtnEventCallbackT)(IdButtonT Btn, BtnEventKindT EvtKind, void *Params);

extern osThreadId_t appCreateTask(osThreadFunc_t, void *Params, osThreadAttr_t *);
extern bool RegButtonEvent(IdButtonT Btn, BtnEventKindT EvtKind, BtnEventCallbackT EvtCallback, void *Params);
extern bool UnRegButtonEvent(IdButtonT Btn, BtnEventKindT EvtKind);

#endif /* _RTOS_H_ */