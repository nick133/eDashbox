/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* <<<< System >>>> */
#include <string.h>
#include "stm32l4xx_hal.h"

/* https://github.com/mpaland/printf
   Tiny printf for embedded systems. Stdlib sprintf corrupts the stack if used
   inside of FreeRTOS vTask */
#include "printf.h"

//#include <stdio.h> // sprintf()
//#include "stdlib.h" // itoa(), gcvt()
//#include "string.h" // strcpy(), memset()

/* <<<< FreeRTOS >>>> */
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>

/* <<<< GUI >>>> */
#include "screens.h"
#include "settings.h"
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

/* USER CODE BEGIN PV */
SensorsDataT Sensors;

static uint32_t gu32_SysTickFreq;
static uint32_t gu32_SysTickPrev;

static float gf_RpmFactor;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void *malloc(size_t xBytes)
{
    return pvPortMalloc(xBytes);
}

void free(void *pvBuffer)
{
    vPortFree(pvBuffer);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    // Must be read from Config.ini on MicroSD card or EEPROM
    Config.TempUnits = UnitsCelsius;
    Config.SpeedUnits = UnitsKph;
    Config.ShowLogo = true;
    Config.WheelCirc = 1285;
    Config.GearRatio = 6.0;
    Config.MaxRpm = 4500;
    Config.Screen1 = IdScreenMain;
    Config.BatLowV = 57.0;
    Config.BatHighV = 84.0;
    Config.HourFormat24 = true;

    Sensors.HallRpm = 0.0;
    Sensors.Volt = 0.0;
    Sensors.Ampere = 0.0;
    Sensors.Odo = 24589.7;
    memset(Sensors.Temperature, 0.0, sizeof(Sensors.Temperature));
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  MX_TIM1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
    /* Calibrate The ADC On Power-Up For Better Accuracy */
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

    DS18B20_Init(DS18B20_Resolution_12bits);

    gu32_SysTickPrev = 0;
    /* This must be called _AFTER_ SystemClock_Config() for correct frequency value */
    gu32_SysTickFreq = osKernelGetSysTimerFreq();
    gf_RpmFactor = 60.0 * (float)gu32_SysTickFreq;
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    for( ; ; )
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        continue;
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/* USER CODE BEGIN 4 */
/*
 * Callbacks for Interupts
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    // if(GPIO_Pin == BTN1_Pin)
    // {
    //     if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_Pin))
    //     {
    //         if(HAL_GPIO_ReadPin(GPIOB, GPIO_Pin) == GPIO_PIN_SET)
    //         {
    //             debug_printf("BTN1: pressed\n");
    //         }
    //         else
    //         {
    //             debug_printf("BTN1: released\n");
    //         }
    //         __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_Pin);
    //     }
    // }
    // else if (GPIO_Pin == BTN2_Pin)
    // {
    //     if(HAL_GPIO_ReadPin(GPIOB, GPIO_Pin) == GPIO_PIN_SET)
    //     {
    //         debug_printf("BTN2: pressed\n");
    //     }
    //     else
    //     {
    //         debug_printf("BTN2: released\n");
    //     }
    // }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        uint32_t Tick = osKernelGetSysTimerCount();
        float Odo = (float)Config.WheelCirc / 1000000.0;

        if(!Config.HallOnWheel)
            { Odo /= Config.GearRatio; }

        Sensors.Odo = (Sensors.Odo < ODOMETER_MAX) ? (Sensors.Odo + Odo) : 0.0;
        Sensors.HallRpm = gf_RpmFactor / (float)(Tick - gu32_SysTickPrev);

        gu32_SysTickPrev = Tick;
    }
}


/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
    else if(htim->Instance == TIM2) {
        uint32_t Tick = osKernelGetSysTimerCount();

        /* Reset and idle if no input data */
        if(Sensors.HallRpm > 0.0
            && ((float)(Tick - gu32_SysTickPrev) / (float)gu32_SysTickFreq) > RPM_IDLE_TIME)
        {
            Sensors.HallRpm = 0.0;
        }
    }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    debug_printf("Assert failed at: %s line %u\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
