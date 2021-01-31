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

// NOTE: No need to keep array of all screens, just set active screen!!!
struct omDisplay
{
  uint16_t Id;
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

// Public methods
void omDisplayInit(omDisplayT *);
void omDisplayDeInit(omDisplayT *);
void omDisplayUpdate(omDisplayT *);
Bool omScreenSelect(omScreenT *);
Bool omScreenIsActive(omScreenT *);
void omDrawPixel(omDisplayT *, uint32_t x, uint32_t y, uint32_t color);
void omDrawBitmap(omBitmapT *);

#endif /* _OMGUI_H_ */
