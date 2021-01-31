/*
 * omgui.h
 *
 *  OLED MCU GUI Library
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: nick
 */

#ifndef _OMGUI_H_
#define _OMGUI_H_

#include "stm32l4xx_hal.h"
//#include "omgui_conf.h"

#define OMGUI_USE_FREERTOS // should go to omgui_conf.h

#ifdef OMGUI_USE_FREERTOS
#include "FreeRTOS.h"
#define SYS_SLEEP(ms) vTaskDelay(ms / portTICK_PERIOD_MS)
#else
#define SYS_SLEEP(ms) HAL_Delay(ms)
#endif // OMGUI_USE_FREERTOS

#ifndef OMGUI_MAX_WIDGETS
#define OMGUI_MAX_WIDGETS 16
#endif

#ifndef CAMELCASE_BOOL_T
#define CAMELCASE_BOOL_T
typedef enum { False, True } Bool;
#endif

typedef struct omDisplay omDisplayT;
typedef struct omScreen omScreenT;
typedef struct omWidget omWidgetT;
typedef struct omBitmap omBitmapT;
typedef struct omAnimation omAnimationT;


// NOTE: No need to keep array of all screens, just set active screen!!!
struct omDisplay
{
  uint16_t Id;
  uint16_t ResX;
  uint16_t ResY;
  omScreenT *Screen;

  void (*InitCallback)(omDisplayT *);
  void (*DeInitCallback)(omDisplayT *);
  void (*UpdateCallback)(omDisplayT *);
  void (*DrawPixelCallback)(omDisplayT *, uint32_t x, uint32_t y, uint32_t color);
};

struct omWidget
{
  uint16_t Id;
  omScreenT *Screen;
  void *Config;
  uint16_t PosX, PosY;
  uint16_t Width, Height;

  void (*InitCallback)(omWidgetT *); // Init and show widget (first time)
  void (*UpdateCallback)(omWidgetT *, void *); // Update only changed parts of widget
};

// NOTE: Init is done by setting a struct, no Init func is needed
struct omScreen
{
  uint16_t Id;
  omDisplayT *Display;
  uint16_t WidgetsNumOf;
  omWidgetT Widgets[OMGUI_MAX_WIDGETS];

  void (*ShowCallback)(omScreenT *);
  void (*HideCallback)(omScreenT *);
};

struct omBitmap
{
  uint16_t Width, Height;
  uint32_t *RawData;
  uint16_t ColorsNumOf;
  Bool IsAlpha;
  uint32_t AlphaColor;
  omDisplayT *Display;
};

struct omAnimation
{
  uint16_t PosX, PosY;
  uint16_t FramesNumOf;
  uint16_t Interval; // msec
  omDisplayT *Display;
  omBitmapT *Bitmaps;
};


// Public methods
void omDisplayInit(omDisplayT *);
void omDisplayDeInit(omDisplayT *);
void omDisplayUpdate(omDisplayT *);
Bool omScreenSelect(omScreenT *);
Bool omScreenIsActive(omScreenT *);
void omDrawPixel(omDisplayT *, uint32_t x, uint32_t y, uint32_t color);
void omDrawBitmap(omBitmapT *, uint32_t x, uint32_t y);
void omAnimationStart(omAnimationT *);

#endif /* _OMGUI_H_ */
