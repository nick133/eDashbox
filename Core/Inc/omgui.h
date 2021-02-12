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

typedef struct omGui omGuiT;
typedef struct omScreen omScreenT;
typedef struct omWidget omWidgetT;
typedef struct omBitmap omBitmapT;
typedef struct omAnimation omAnimationT;


// NOTE: No need to keep array of all screens, just set active screen!!!
struct omGui
{
  uint16_t Id;
  uint16_t ResX, ResY;
  omScreenT *ActiveScreen;

  void (*InitCallback)(omGuiT *);
  void (*DeInitCallback)(omGuiT *);
  void (*UpdateCallback)(omGuiT *);
  void (*ClearCallback)(omGuiT *);
  void (*DrawPixelCallback)(omGuiT *, uint32_t x, uint32_t y, uint8_t color);
};

struct omWidget
{
  uint16_t Id;
  omScreenT *Screen;
  void *Config;
  uint16_t PosX, PosY;
  uint16_t Width, Height;

  void (*ShowCallback)(omWidgetT *); // Init and show widget (first time)
  void (*UpdateCallback)(omWidgetT *, void *); // Update only changed parts of widget
};

// NOTE: Init is done by setting a struct, no Init func is needed for screens
struct omScreen
{
  uint16_t Id;
  omGuiT *Ui;
  uint16_t WidgetsNumOf;
  omWidgetT Widgets[OMGUI_MAX_WIDGETS];

  void (*ShowCallback)(omScreenT *);
  void (*HideCallback)(omScreenT *);
};

struct omBitmap
{
  uint32_t Width, Height;
  uint32_t ColorsNumOf;
  Bool IsAlpha;
  uint32_t AlphaColor;
  const uint8_t *RawData;
};

struct omAnimation
{
  uint16_t PosX, PosY;
  uint16_t FramesNumOf;
  uint16_t Interval; // msec
  omGuiT *Ui;
  omBitmapT *Bitmaps;
};

// Public methods
void omGuiInit(omGuiT *);
void omGuiDeInit(omGuiT *);
void omGuiUpdate(omGuiT *);
void omGuiClear(omGuiT *);
Bool omScreenSelect(omScreenT *);
Bool omScreenIsActive(omScreenT *);
void omDrawPixel(omGuiT *, uint32_t x, uint32_t y, uint8_t color);
void omDrawBitmap(omGuiT *, omBitmapT *, uint32_t x, uint32_t y, Bool alpha, Bool update);
void omDrawLine(omGuiT *, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color, Bool update);
//void omAnimationStart(omAnimationT *);

#endif /* _OMGUI_H_ */