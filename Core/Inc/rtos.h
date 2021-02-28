
#ifndef _RTOS_H_
#define _RTOS_H_

#include "FreeRTOS.h"
#include "task.h"

#define TASK_NAME(t)    &((osThreadAttr_t){ .name = t })

extern osThreadId_t appCreateTask(osThreadFunc_t, void *, osThreadAttr_t *);

#endif /* _RTOS_H_ */