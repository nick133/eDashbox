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
#include "omgui.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef enum { UnitsSpeedKph, UnitsSpeedMph } UnitsSpeedT;
typedef enum { UnitsTempCelsius, UnitsTempFahrenheit } UnitsTempT;


typedef struct ConfigSettings {
  UnitsSpeedT SpeedUnits;
  UnitsTempT TempUnits;
  uint16_t WheelCirc; // millimeters
  uint16_t GearRatio; // millimeters
  Bool ShowLogo;

} ConfigSettingsT;

typedef struct SensorsData {
  volatile uint16_t MotorRpm;
  volatile float SpeedKph;
  uint32_t DistanceOdo;
  uint32_t DistanceRide;
  uint32_t Trip1, Trip2;
  uint32_t Power;
  float Temperature1; // Celsius degree
  float Temperature2;
  float BattVoltage;
  float BattCurrent;

} SensorsDataT;


extern ConfigSettingsT config;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define Sleep(ms) vTaskDelay(ms / portTICK_PERIOD_MS)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
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
#define FREQ_CLK 80000000UL
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
