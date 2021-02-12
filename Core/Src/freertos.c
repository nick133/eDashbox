/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "screens.h"
#include "bitmaps.h"

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t SensorsQueue;

// osThreadId_t taskTempPoll;
// const osThreadAttr_t taskTempPoll_attr = {
//   .name = "taskTempPoll",
//   .priority = (osPriority_t) osPriorityNormal,
//   .stack_size = 128 * 4
// };
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void TemperaturePoll(void *);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    osTimerId_t TimerTempPoll = osTimerNew(TemperaturePoll, osTimerPeriodic, NULL, NULL);
  
    if(TimerTempPoll != NULL)
    {
        // Periodic timer created
        osStatus_t tmStatus = osTimerStart(TimerTempPoll, 600U);
 
        if(tmStatus != osOK)
        {
            // Timer could not be started
        }
    }

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    SensorsQueue = osMessageQueueNew(16, sizeof(SensorMessageT), NULL);

    if(SensorsQueue == NULL)
    {
        ; // Message Queue object not created, handle failure
    }
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
//    taskTempPoll = osThreadNew(TaskTemperaturePoll, NULL, &taskTempPoll_attr); 
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    // Show logo
    omDrawBitmap(&oledUi, &AssetBitmaps.Logo, 0, 0, False, True);
    osDelay(1500);
    omScreenSelect(&screenMain);
  
    while(1)
    {
        SensorMessageT SsrMsg;
        SsrMsg.Kind = SsrMsgTmp1;

        osStatus_t getStatusQ = osMessageQueueGet(SensorsQueue, &SsrMsg, NULL, 0U);
        if(getStatusQ == osOK)
        {
            SEGGER_RTT_printf(0, "Temp: %u\n", (int)SsrMsg.Data.Temperature1);
        }

        osDelay(2000);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TemperaturePoll(void *params)
{
    uint8_t ROM_tmp[8];

    SensorMessageT SsrMsg;
    SsrMsg.Kind = SsrMsgTmp1;

    DS18B20_ReadAll();
    DS18B20_StartAll();

    for(uint8_t i = 0; i < DS18B20_Quantity(); i++)
    {
        if(DS18B20_GetTemperature(i, &SsrMsg.Data.Temperature1))
        {
            DS18B20_GetROM(i, ROM_tmp);

            osMessageQueuePut(SensorsQueue, &SsrMsg, 0U, 0U);
        }   
    }

    osDelay(600);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
