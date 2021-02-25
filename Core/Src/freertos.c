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
//#include "cmsis_os2.h"
#include "tim.h"
#include "adc.h"
#include "stdbool.h"

#include "printf.h"
#include "ds18b20.h"

#include "screens.h"
#include "settings.h"
#include "bitmaps.h"

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
static osThreadId_t TempPollTask;
static osThreadId_t AdcPollTask;
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
__NO_RETURN static void TemperaturePoll(void *);
__NO_RETURN static void AdcPoll(void *);
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
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    TempPollTask = osThreadNew(TemperaturePoll, NULL, NULL);
    AdcPollTask = osThreadNew(AdcPoll, NULL, NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
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

    /* Hall sensor timer starts after scheduler as we use RTOS API from ISR.
     * Enable HAL_TIM_PeriodElapsedCallback() before HAL_TIM_IC_Start_IT()
     * as it's not activated by default. Further reading:
     * https://community.st.com/s/question/0D50X00009hpBdlSAE/timer3-update-event-interrupt-not-working-properly
     */
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    Screens_Init();

    if(Config.ShowLogo)
    {
        omDrawBitmap(&oledUi, &AssetBitmaps.Logo, 4, 4, false, true);
        omDrawBitmap(&oledUi, &AssetBitmaps.LogoText1, 69, 30, false, true);
        omDrawBitmap(&oledUi, &AssetBitmaps.LogoText2, 92, 9, false, true);
        osDelay(1200);
    }

    omScreenSelect(uiScreens[(Config.Screen1 >= 0 && Config.Screen1 < 4)
        ? Config.Screen1 : IdScreenMain]);

    do {
        omScreenUpdate(&oledUi);
    } while(osDelay(MAIN_THREAD_DELAY) == osOK); // fixed fps

  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
__NO_RETURN static void TemperaturePoll(void *params)
{
    do {
        // uint8_t ROM_tmp[8];

        DS18B20_ReadAll();
        DS18B20_StartAll();

        for(uint8_t i = 0; i < DS18B20_Quantity(); i++)
        {
            if(DS18B20_GetTemperature(i, &Sensors.Temperature[i]));
            {
                // DS18B20_GetROM(i, ROM_tmp);
            }
        }
    } while(osDelay(DS18B20_POLL_DELAY) == osOK);
}

__NO_RETURN static void AdcPoll(void *params)
{
    do {
        HAL_ADC_Start_IT(&hadc1);
    } while(osDelay(ADC_POLL_DELAY) == osOK);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/