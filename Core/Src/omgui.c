/*
 * omgui.h
 *
 *  OLED MCU GUI Library
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: nick
 */

#include "omgui.h"
#include "main.h"


/* Private methods */
static void _omScreenInit(omScreenT *);


void omDisplayInit(omDisplayT *displ)
{
  displ->InitCallback(displ); // OLED Driver's Init code goes here
  displ->ActiveScreen=NULL; // Discarded if set, use omScreenSelect()
}


void omDisplayDeInit(omDisplayT *displ)
{
  displ->DeInitCallback(displ); // OLED Driver's DeInit code goes here
}


Bool omScreenSelect(omScreenT *screen)
{
  if (omScreenIsActive(screen) == True)
  {
    return False;
  }

  if (screen->Display->ActiveScreen != NULL && screen->Display->ActiveScreen->HideCallback != NULL)
  {
    screen->Display->ActiveScreen->HideCallback(screen);
  }

  if (screen->ShowCallback != NULL)
  {
    screen->ShowCallback(screen);
  }

  for (uint16_t i = 0; i < screen->WidgetsNumOf; i++)
  {
    // array[1] equals to *(array + 1)
    // https://stackoverflow.com/questions/16201607/c-pointer-to-array-of-structs
    screen->Widgets[i].ShowCallback(screen->Widgets + i);
  }

  screen->Display->ActiveScreen = screen;

  return True;
}


Bool omScreenIsActive(omScreenT *screen)
{
  return (screen->Display->ActiveScreen == NULL || screen->Display->ActiveScreen->Id != screen->Id) ? False : True;
}


void omDisplayUpdate(omDisplayT *displ)
{
  displ->UpdateCallback(displ);
  return;
}


void omDrawPixel(omDisplayT *displ, uint32_t x, uint32_t y, uint32_t color)
{
  displ->DrawPixelCallback(displ, x, y, color);
  return;
}


void omDrawBitmap(omDisplayT *displ, omBitmapT *bitmap, uint32_t x, uint32_t y)
{
  uint32_t *data = bitmap->RawData; // Don't increment original pointer

  for (uint32_t yto = y; yto < bitmap->Height + y; yto++)
    for (uint32_t xto = x; xto < bitmap->Width + x; xto++)
    {
      if (!(bitmap->IsAlpha == True && bitmap->AlphaColor == *data))
      {
        omDrawPixel(displ, xto, yto, *data);
      }

      data++;
    }

  return;
}

/* void omAnimationStart(omAnimationT *anim)
{
  omBitmapT *bitmap = anim->Bitmaps; // Don't increment original pointer

  for (uint16_t i = 0; i < anim->FramesNumOf; i++)
  {
    omDrawBitmap(bitmap, anim->PosX, anim->PosY);
    omDisplayUpdate(anim->Display);
    SYS_SLEEP(anim->Interval);

    bitmap++;
  }

  return;
} */
