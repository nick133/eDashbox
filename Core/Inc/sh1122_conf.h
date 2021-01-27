/*
 * sh1122_conf.h
 *
 *  Created on: 21 янв. 2021 г.
 *      Author: nick
 */

#ifndef INC_SH1122_CONF_H_
#define INC_SH1122_CONF_H_


#include "main.h"
#include "spi.h"


#define OLED_nCS_GPIO_Port SPI1_nCS_GPIO_Port
#define OLED_nCS_Pin SPI1_nCS_Pin

#define OLED_DC_GPIO_Port SPI1_DC_GPIO_Port
#define OLED_DC_Pin SPI1_DC_Pin

#define OLED_RES_GPIO_Port SPI_RES_GPIO_Port
#define OLED_RES_Pin SPI1_RES_Pin


#endif /* INC_SH1122_CONF_H_ */
