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

#include "stdbool.h"
#include "stm32l4xx_hal.h"

#ifndef OMGUI_MAX_WIDGETS
#define OMGUI_MAX_WIDGETS 16
#endif

typedef struct omGui omGuiT;
typedef struct omScreen omScreenT;
typedef struct omBitmap omBitmapT;
typedef struct omAnimation omAnimationT;


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

struct omScreen
{
  uint16_t Id;
  omGuiT *Ui;
  void (*ShowCallback)(omScreenT *);
  void (*HideCallback)(omScreenT *);
  void (*UpdateCallback)(omScreenT *);
};

struct omBitmap
{
  uint32_t Width, Height;
  uint32_t ColorsNumOf;
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
bool omScreenSelect(omScreenT *);
void omScreenUpdate(omGuiT *);
bool omScreenIsActive(omScreenT *);
void omDrawPixel(omGuiT *, uint32_t x, uint32_t y, uint8_t color);
void omDrawBitmap(omGuiT *, omBitmapT *, uint32_t x, uint32_t y, bool alpha, bool update);
void omDrawBitmapCropped(omGuiT *, omBitmapT *, uint32_t x, uint32_t y,
        uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, bool alpha, bool update);
void omDrawLine(omGuiT *, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color, bool update);
void omDrawRectangle(omGuiT *,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, bool update);
void omDrawRectangleFilled(omGuiT *,
        int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bColor, uint8_t fColor, bool update);
//void omAnimationStart(omAnimationT *);

#endif /* _OMGUI_H_ */