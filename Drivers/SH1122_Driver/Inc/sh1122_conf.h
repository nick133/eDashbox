/*
 * sh1122_conf.h
 *
 *  Created on: 21 янв. 2021 г.
 *      Author: nick
 */
#ifndef __SH1122_CONF_H
#define __SH1122_CONF_H

#include "main.h"
#include "spi.h"

#define SH1122_OLED_SPI_HANDLE  &hspi1

#define SH1122_OLED_DC_PIN      OLED_DC_Pin
#define SH1122_OLED_DC_PORT     OLED_DC_GPIO_Port
#define SH1122_OLED_CS_PIN      OLED_nCS_Pin
#define SH1122_OLED_CS_PORT     OLED_nCS_GPIO_Port
#define SH1122_OLED_RES_PIN     OLED_RES_Pin
#define SH1122_OLED_RES_PORT    OLED_RES_GPIO_Port

#endif /* __SH1122_CONF_H */