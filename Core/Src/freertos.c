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
#include "tim.h"
#include "stdbool.h"
#include "screens.h"
#include "settings.h"
#include "bitmaps.h"
#include "ds18b20.h"

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
osEventFlagsId_t SensorEvent;
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
        osStatus_t tmStatus = osTimerStart(TimerTempPoll, DS18B20_POLL_DELAY);
 
        if(tmStatus != osOK)
        {
            // Timer could not be started
        }
    }

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
//    taskTempPoll = osThreadNew(TaskTemperaturePoll, NULL, &taskTempPoll_attr); 
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    
    if((SensorEvent = osEventFlagsNew(NULL)) == NULL)
    {
        // errror
    }
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

    /* Hall sensor timer, start after scheduler as we use RTOS
     * API from interrupts.
     */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

//SEGGER_RTT_printf(0, "OS Systick Freq: %u\n", osKernelGetTickFreq());
//SEGGER_RTT_printf(0, "MCU Systick Freq: %u\n", osKernelGetSysTimerFreq());

    if(config.ShowLogo)
    {
        omDrawBitmap(&oledUi, &AssetBitmaps.Logo, 0, 0, false, true);
        osDelay(1200);
    }

    omScreenSelect(uiScreens[
        (config.Screen1 >= 0 && config.Screen1 < 4) ? config.Screen1 : IdScreenMain]);
    
    while(1)
    {
        // Wait for event from sensors
        uint32_t flags = osEventFlagsWait(
            SensorEvent, EVENT_SENSOR_UPDATE, osFlagsWaitAny, osWaitForever);

        omScreenUpdate(&oledUi);
        osDelay(50); // fixed fps if sensors data are coming too fast
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TemperaturePoll(void *params)
{
    // uint8_t ROM_tmp[8];
    DS18B20_ReadAll();
    DS18B20_StartAll();

    for(uint8_t i = 0; i < DS18B20_Quantity(); i++)
    {
        if(DS18B20_GetTemperature(i, &sensors.Temperature[i]));
        {
            osEventFlagsSet(SensorEvent, EVENT_SENSOR_UPDATE);
            // DS18B20_GetROM(i, ROM_tmp);
        }   
    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
