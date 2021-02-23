/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"
#include "stdbool.h"
#include "ds18b20.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct SensorsData {
    float HallRpm;
    float Temperature[_DS18B20_MAX_SENSORS]; // depends on Config.TempUnits
    float Volt;
    float Ampere;
    float Odo;

    bool Button1Pressed;
    bool Button1Released;
    bool Button2Pressed;
    bool Button2Released;
 } SensorsDataT;

extern SensorsDataT Sensors;

extern osThreadId_t SensorsQueue;
extern osEventFlagsId_t SensorEvent;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define DS18B20_POLL_DELAY 600U // msec
#define EVENT_SENSOR_UPDATE 0x00000001U
#define RPM_IDLE_TIME 1.1 // (float) seconds

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void *malloc(size_t xBytes);
void free(void *pvBuffer);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HALL_SPEED_Pin GPIO_PIN_0
#define HALL_SPEED_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define BTN1_Pin GPIO_PIN_3
#define BTN1_GPIO_Port GPIOA
#define BTN1_EXTI_IRQn EXTI3_IRQn
#define BTN2_Pin GPIO_PIN_4
#define BTN2_GPIO_Port GPIOA
#define BTN2_EXTI_IRQn EXTI4_IRQn
#define OLED_RES_Pin GPIO_PIN_5
#define OLED_RES_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_7
#define DS18B20_GPIO_Port GPIOA
#define OLED_nCS_Pin GPIO_PIN_8
#define OLED_nCS_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_6
#define SD_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
