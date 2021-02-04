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
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* <<<< System >>>> */

/* https://github.com/mpaland/printf
   Tiny printf for embedded systems. Stdlib sprintf corrupts the stack if used
   inside of FreeRTOS vTask */
#include "printf.h"

//#include <stdio.h> // sprintf()
#include "stdlib.h" // itoa(), gcvt()
#include "string.h" // strcpy(), memset()

/* <<<< FreeRTOS >>>> */
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>

/* <<<< Drivers >>>> */
//#include "sd.h"
#include "onewire.h"
#include "ds18b20.h"
#include "sh1122.h"
#include "font.h"
#include "f10x16f.h"

/* <<<< GUI >>>> */
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
SensorsDataT sensor;

static volatile uint8_t u8_RPM_State = IDLE;
static volatile uint32_t u32_RPM_T1 = 0;
static volatile uint32_t u32_RPM_T2 = 0;
static volatile uint32_t u32_RPM_Ticks = 0;
static volatile uint16_t u16_TIM2_OVC = 0;


 FATFS fs;
 FATFS *pfs;
 FIL fil;
 FRESULT fres;
 DWORD fre_clust;
 uint32_t totalSpace, freeSpace;
 char buffer[100];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* void MX_FREERTOS_Init(void); */
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void TaskTemperaturePoll(void *);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void *malloc( size_t xBytes )
{
  return pvPortMalloc( xBytes );
}

void free( void *pvBuffer )
{
  vPortFree( pvBuffer );
}
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
  config.GearRatio = 6;

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
  MX_TIM2_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_FATFS_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  DS18B20_Init(DS18B20_Resolution_12bits);

uint32_t ErrorError=0;

HAL_Delay(1000);

  if(f_mount(&fs, "", 1) != FR_OK)
  {
    ErrorError = 1;
  }
    
       /* Open file to write */
        if(f_open(&fil, "1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
  {
    ErrorError = 2;
  }

        /* Check freeSpace space */
        if(f_getfree("", &fre_clust, &pfs) != FR_OK)
     {
    ErrorError = 3;
  }

        totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
        freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

        /* free space is less than 1kb */
        if(freeSpace < 1)
  {
    ErrorError = 4;
  }

        /* Writing text */
        f_puts("11222333444555666777888999000", &fil);
        //f_puts("Save the world!!!", &fil);

        /* Close file */
        if(f_close(&fil) != FR_OK)
     {
    ErrorError = 5;
  }

        /* Open file to read */
        if(f_open(&fil, "1.txt", FA_READ) != FR_OK)
     {
    ErrorError = 6;
  }

  f_gets(buffer, sizeof(buffer), &fil);
  //      while(f_gets(buffer, sizeof(buffer), &fil))
        // {
        //         /* SWV output */
        //         sprintf("%s", buffer);
        //         fflush(stdout);
        // }

        /* Close file */
        if(f_close(&fil) != FR_OK)
     {
ErrorError = 7;
  }

        /* Unmount SDCARD */
        if(f_mount(NULL, "", 1) != FR_OK)
        {
ErrorError = 8;
  }





  /* HAL_TIM_IC_Start_IT is not enabling HAL_TIM_PeriodElapsedCallback,
   * so we need to manually enable it. See:
   * https://community.st.com/s/question/0D50X00009hpBdlSAE/timer3-update-event-interrupt-not-working-properly
   */
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

/*
  SH1122_Display_T {
    SpiInstance
    SpiPinDC
    SpiPinCS
    SpiPinSCL
    SpiPinRST
    SpiPinSDA
  }

  SH1122_DisplayInit(SH1122_Display_T)
  SH1122_DisplayUpdate

 */

// Display_Init();
// //Display_SetOrienation(OLED_DISP_ROTATE180);
// Frame_DrawPixel(10,10,Display_Color.Gray_01);
// Frame_DrawPixel(10,15,Display_Color.Gray_02);
// Frame_DrawPixel(10,20,Display_Color.Gray_03);
// Frame_DrawRectangle(0, 0, OLED_WIDTH-1, OLED_HEIGHT-1, Display_Color.Gray_08);
// Frame_printf(20,20, FONTID_10X16F, Display_Color.Gray_15, LEFT, TOP, "evAnalyst v0.1");
// Display_SendFrame();
// HAL_Delay(15000);


  OLED_GUI_Init();

//  HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

 TaskHandle_t taskTempPoll = NULL;

 BaseType_t taskTempPollRet = xTaskCreate(
   TaskTemperaturePoll,
   "temp-sensor-poll",        /* Text name for the task. */
   configMINIMAL_STACK_SIZE,  /* Stack size in words, not bytes. */
   (void *) 1,                /* Parameter passed into the task. */
   configMAX_PRIORITIES / 2,  /* Priority of the task created. */
   &taskTempPoll );

  // if (taskTempPollRet == pdPASS)
  // {
  //   ssd1306_WriteString("Task ok!", Font_7x10, White);
  //   omDisplayUpdate(&oled1);
  // }
  // else if (taskTempPollRet == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
  // {
  //   ssd1306_WriteString("Task FAILED!", Font_7x10, White);
  //   omDisplayUpdate(&oled1);
  // }

  vTaskStartScheduler();

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
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
  /* EXTI4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

/* USER CODE BEGIN 4 */
void TaskTemperaturePoll(void *pvParams)
{
  //configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  char message[64];

  while (1)
  {
    DS18B20_ReadAll();
    DS18B20_StartAll();

 		uint8_t ROM_tmp[8];
		uint8_t i;

  	for (i = 0; i < DS18B20_Quantity(); i++)
		{
			if (DS18B20_GetTemperature(i, &sensor.Temperature1))
			{
				DS18B20_GetROM(i, ROM_tmp);
//				memset(message, 0, sizeof(message));
				//sprintf(message, "%d. ROM: %X%X%X%X%X%X%X%X Temp: %f\n\r",i, ROM_tmp[0], ROM_tmp[1], ROM_tmp[2], ROM_tmp[3], ROM_tmp[4], ROM_tmp[5], ROM_tmp[6], ROM_tmp[7], temperature);
        snprintf(message, 64, "Temp: %.2f", sensor.Temperature1);
        //gcvt(sensor.Temperature1, 5, &message);
        // Frame_printf(uint16_t X, uint16_t Y, uint8_t FontID, uint8_t color,
        //     uint8_t hAlign, uint8_t vAlign, const char *args, ...)
        Frame_printf(0,30, FONTID_10X16F, Display_Color.Gray_11, LEFT, TOP, message);

        omDisplayUpdate(&oled1);
			}
		}

    vTaskDelay(600 / portTICK_PERIOD_MS);
  }
}


/*
 * Callbacks for Interupts
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BTN1_Pin)
  {

  }
  else if (GPIO_Pin == BTN2_Pin)
  {

  }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
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
      /* T sec = (Prescaler * Preload) / Frequency MHz
       * Preload = Frequency MHz * T sec / Prescaler
       */
      sensor.MotorRpm = 60 * ((FREQ_CLK/htim->Init.Prescaler) / u32_RPM_Ticks);
      sensor.SpeedKph = ((sensor.MotorRpm * 60) / config.GearRatio) * config.WheelCirc;

      u8_RPM_State = IDLE;
    }
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
      sensor.MotorRpm = 0;
      sensor.SpeedKph = 0;
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
