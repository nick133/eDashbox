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
#include "rtos.h"

#include "printf.h"
#include "ds18b20.h"

#include "screens.h"
#include "settings.h"
#include "bitmaps.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    BtnEventCallbackT Callback;
    void *Params;
} BtnEvtCallbackT;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_POLL_DELAY 250U
#define ADC_NCHANNELS 2
#define ADC_CHANNEL_VOLT 0
#define ADC_CHANNEL_AMPS 1
#define ADC_VREF 3.3
#define ADC_12BIT_MAX 4095U

#define MAIN_THREAD_DELAY 50U
#define DS18B20_POLL_DELAY 600U

#define BTN_POLL_DELAY 80U // debounce
#define BTN_LONGPRESS_TIME 1000U // hold at least this time before release
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static osThreadId_t TempPollTask;
static osThreadId_t AdcPollTask;
static osThreadId_t BtnPollTask;

/* Use static allocation as pointers are randomly being corrupted if
 * dynamic allocation is used without malloc()
 */
static BtnEvtCallbackT BtnCallbacks[NUM_BUTTONS][EvtButtonEmpty];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
__NO_RETURN static void TemperaturePoll(void *);
__NO_RETURN static void AdcPoll(void *);
__NO_RETURN static void ButtonsPoll(void *);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
/* "Stack overflow checking introduces a context switch overhead so its use is only
 * recommended during the development or testing phases." Further reading:
 * https://www.freertos.org/Stacks-and-stack-overflow-checking.html
 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
   debug_printf("Stack overflow: %s\n", pcTaskName);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    for(uint8_t i = 0; i < NUM_BUTTONS; i++)
    {
        for(uint8_t j = 0; j < EvtButtonEmpty; j++)
        {
            BtnCallbacks[i][j] = (BtnEvtCallbackT){ NULL, NULL };
        }
    }
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
        omGui_DrawBitmap(&oledUi, &AssetBitmaps.Logo, 4, 4, false, true);
        omGui_DrawBitmap(&oledUi, &AssetBitmaps.LogoText1, 69, 30, false, true);
        omGui_DrawBitmap(&oledUi, &AssetBitmaps.LogoText2, 92, 9, false, true);
        osDelay(1200);
    }

    Sensors.StandByTimeoutTick = osKernelGetTickCount();

    TempPollTask = appCreateTask(TemperaturePoll, NULL, TASK_NAME("TemperaturePoll"));
    AdcPollTask = appCreateTask(AdcPoll, NULL, TASK_NAME("AdcPoll"));
    BtnPollTask = appCreateTask(ButtonsPoll, NULL, TASK_NAME("ButtonsPoll"));

    omScreen_Select(uiScreens[
        (Config.Screen1 >= 0 && Config.Screen1 < 4) ? Config.Screen1 : IdScreenMain], NULL);

    do {
        omScreen_Update(&oledUi);

        // if(osKernelGetTickCount() - Sensors.TimeoutTick > Config.StandByTimeout * 60 * 1000)
        // {
        //     // StandBy mode on!
        // }
    } while(osDelay(MAIN_THREAD_DELAY) == osOK); // fixed fps

    debug_printf("Error: exiting main thread!\n");
    osThreadExit();
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* (!) Increase FreeRTOS TOTAL_HEAP_SIZE if new task creation fails with osThreadError.
 * Further reading: https://stackoverflow.com/questions/40054233/i-can-not-create-more-than-5-tasks-in-freertos
 * (!) Increase FreeRTOS MINIMAL_STACK_SIZE if stack overflow occurs in tasks.
 * Working settings so far:
 * ( FreeRTOS )
 *     TOTAL_HEAP_SIZE = 8192
 *     MINIMAL_STACK_SIZE = 256
 * ( CubeMX )
 *     Max heap size: 0x400
 *     Max stack size: 0x800
 * 
 * (!) Don't forget to disable stack overflow detection in CubeMX for release (!)
 */

osThreadId_t appCreateTask(osThreadFunc_t func, void *args, osThreadAttr_t *attr)
{
    char *error;
    osThreadId_t tid = osThreadNew(func, args, attr);

    if(tid == NULL)
    {
        osThreadState_t state = osThreadGetState(tid);

        switch(state)
        {
            case osThreadInactive:   error = "Inactive"; break;
            case osThreadReady:      error = "Ready"; break;
            case osThreadRunning:    error = "Running"; break;
            case osThreadBlocked:    error = "Blocked"; break;
            case osThreadTerminated: error = "Terminated"; break;
            case osThreadError:      error = "Error"; break;
            default:                 error = "Unexpected";
        }

        debug_printf("Thread '%s' creation failed: %s\n", attr->name, error);
    }

    return tid;
}


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

    osThreadExit();
}


__NO_RETURN static void AdcPoll(void *params)
{
    static uint32_t ADC_channels[ADC_NCHANNELS]; // 0 - Volt, 1 - Ampere
    static const double AdcFactor = (double)ADC_VREF / (double)(ADC_12BIT_MAX + 1);

    do {
        HAL_StatusTypeDef status = HAL_ADC_Start_DMA(&hadc1, ADC_channels, ADC_NCHANNELS);

        if(status == HAL_OK)
        {
            Sensors.Volt = (double)(ADC_channels[ADC_CHANNEL_VOLT] + 1) * AdcFactor;
            Sensors.Ampere = (double)(ADC_channels[ADC_CHANNEL_AMPS] + 1) * AdcFactor;

            HAL_ADC_Stop_DMA(&hadc1);
        }
        else
        {
            debug_printf("ADC status: %s\n", (status == HAL_BUSY) ? "BUSY" : "TIMEOUT|ERROR");
        }
    } while(osDelay(ADC_POLL_DELAY) == osOK);

    osThreadExit();
}


__NO_RETURN static void ButtonsPoll(void *params)
{
    static struct BtnState
    {
        uint32_t PressTick;
        bool IsIdle;
        bool IsEvent;
        GPIO_TypeDef *GPIOx;
        uint16_t Pin;
    } Btn[] =
    {
        0, true, false, BTN1_GPIO_Port, BTN1_Pin,
        0, true, false, BTN2_GPIO_Port, BTN2_Pin
    };

    void evtfire(uint8_t btn, BtnEventKindT evt)
    {
        BtnEvtCallbackT *evtcb = &BtnCallbacks[btn][evt];

        if(evtcb->Callback != NULL)
        {
            evtcb->Callback(btn, evt, evtcb->Params);
        }
    }

    do {
        uint32_t Tick = osKernelGetTickCount();

        /* Maximum 2^32 ticks (~49 days), counter resets. One press per 49 days is ignored, not a huge trade-off */
        if(Btn[0].PressTick > Tick || Btn[1].PressTick > Tick)
        {
            Btn[0].PressTick = Btn[1].PressTick = 0;
            Btn[0].IsIdle = Btn[1].IsIdle = true;
            Btn[0].IsEvent = Btn[1].IsEvent = false;
        }

        /* Detect and save buttons state */
        for(uint8_t i = 0; i < NUM_BUTTONS; i++)
        {
            if(HAL_GPIO_ReadPin(Btn[i].GPIOx, Btn[i].Pin)) // Press
            {
                /* Process only first press, skip multiple presses, debounce */
                if(Btn[i].IsIdle && !Btn[i].IsEvent)
                {
                    Btn[i].PressTick = Tick;
                    Btn[i].IsIdle = false;
                }
                else if(!Btn[i].IsIdle && !Btn[i].IsEvent && (Tick - Btn[i].PressTick) > BTN_LONGPRESS_TIME)
                {
                    evtfire(i, EvtButtonLongPress);
                    Btn[i].IsEvent = true;
                }
            }
            else if(!Btn[i].IsIdle) // Release
            {
                if(!Btn[i].IsEvent)
                {
                    evtfire(i, EvtButtonPress);
                }
                else
                {
                    Btn[i].IsEvent = false;
                }

                Btn[i].IsIdle = true;
            }
        }
    } while(osDelay(BTN_POLL_DELAY) == osOK);

    osThreadExit();
}


bool RegButtonEvent(IdButtonT Btn, BtnEventKindT EvtKind, BtnEventCallbackT EvtCallback, void *Params)
{
    if(EvtKind >= EvtButtonEmpty) { return false; }

    if(Btn >= NUM_BUTTONS) { Btn = NUM_BUTTONS - 1; }

    BtnCallbacks[Btn][EvtKind] =
    (BtnEvtCallbackT){
        .Callback = EvtCallback,
        .Params = Params
    };

    return true;
}


bool UnRegButtonEvent(IdButtonT Btn, BtnEventKindT EvtKind)
{
    if(EvtKind >= EvtButtonEmpty) { return false; }

    if(Btn >= NUM_BUTTONS) { Btn = NUM_BUTTONS - 1; }

    BtnCallbacks[Btn][EvtKind].Callback = NULL;
    BtnCallbacks[Btn][EvtKind].Params = NULL;

    return true;
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/