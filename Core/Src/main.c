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
/* #include "cmsis_os.h" */
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>

#include "ssd1306.h"
#include <stdio.h> // for sprintf()
//#include "stdlib.h" // for itoa()
//#include "string.h" // for strcpy()

#include "omgui.h"
#include "screens.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IDLE 0
#define DONE 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ConfigSettingsT config;
SensorsDataT sensdata;

volatile uint8_t u8_RPM_State = IDLE;
volatile uint32_t u32_RPM_T1 = 0;
volatile uint32_t u32_RPM_T2 = 0;
volatile uint32_t u32_RPM_Ticks = 0;
volatile uint16_t u16_TIM2_OVC = 0;
volatile uint32_t u32_RPM_Freq = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* void MX_FREERTOS_Init(void); */
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void vTaskCode(void *pvParameters);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//  config.TempUnits = TEMP_CELSIUS;
  config.SpeedUnits = UnitsSpeedKph;
  config.ShowLogo = False;
  config.WheelCirc = 1285;

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_RTC_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  OLED_GUI_Init();

//  ssd1306_SetCursor(0, 0);
//  ssd1306_WriteString("Test: FreeRTOS", Font_7x10, White);
//  ssd1306_UpdateScreen();

//  ssd1306_DrawPixel(12, 30, White);

//  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

//  BaseType_t xReturned;
//  TaskHandle_t xHandle = NULL;
//
//  /* Create the task, storing the handle. */
//  xReturned = xTaskCreate(
//    vTaskCode,       /* Function that implements the task. */
//    "NAME",          /* Text name for the task. */
//    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
//    ( void * ) 1,    /* Parameter passed into the task. */
//    configMAX_PRIORITIES / 2,/* Priority at which the task is created. */
//    &xHandle );      /* Used to pass out the created task's handle. */
//
//  if( xReturned == pdPASS )
//  {
//    /* The task was created.  Use the task's handle to delete the task. */
//    vTaskDelete( xHandle );
//  }

//  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Init scheduler */
/*   osKernelInitialize(); */  /* Call init function for freertos objects (in freertos.c) */
/*   MX_FREERTOS_Init(); */
  /* Start scheduler */
/*   osKernelStart(); */

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
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
  /* EXTI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/* USER CODE BEGIN 4 */
void vTaskCode(void *pvParameters)
{
  return;
}

/*
 * Callbacks for Interupts
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (u8_RPM_State == IDLE)
  {
    u32_RPM_T1 = TIM2->CCR1;
    u16_TIM2_OVC = 0;
    u8_RPM_State = DONE;
  }
  else if (u8_RPM_State == DONE)
  {
    u32_RPM_T2 = TIM2->CCR1;
    u32_RPM_Ticks = (u32_RPM_T2 + (u16_TIM2_OVC * 20000)) - u32_RPM_T1;
    /*
     * T sec = (Prescaler * Preload) / Frequency MHz
     * Preload = Frequency MHz * T sec / Prescaler
     * 80000000*0.25
     */
    // RPM
    u32_RPM_Freq = (uint32_t)(60 * ((FREQ_CLK/htim->Init.Prescaler) / u32_RPM_Ticks));
    u8_RPM_State = IDLE;

    char buf0[16];
//    char buf1[16];
//    char buf2[16];
    sprintf(buf0, "Freq: %d", (int)u32_RPM_Freq);
//    sprintf(buf1, "Tick: %d", (int)u32_RPM_Ticks);
//    sprintf(buf2, "OVC: %d", (int)u16_TIM2_OVC);
    //dbg_print((char*)&buf0);
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
  else if (htim->Instance == TIM2) {
    if (u16_TIM2_OVC > 2)
    {
      u8_RPM_State = IDLE;
      u16_TIM2_OVC = 0;
      u32_RPM_Freq = 0;
    }
    else
      u16_TIM2_OVC++;
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
