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

/* Private */
static const uint8_t colorMap8bit[16] = {
  // As defined in sh1122.c
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};

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


void omDrawPixel(omDisplayT *displ, uint32_t x, uint32_t y, uint8_t color)
{
  displ->DrawPixelCallback(displ, x, y, color);
  return;
}


/* Store bitmap RawData in FLASH memory, not RAM, image buffer is corrupted otherwise.
 * Data array must be declared as global 'const'. Further readings:
 * https://electronics.stackexchange.com/questions/74589/how-to-stock-variables-in-flash-memory
 * https://forum.arduino.cc/index.php?topic=461487.0
 */
void omDrawBitmap(omDisplayT *displ, omBitmapT *bitmap, uint32_t x, uint32_t y)
{
  uint8_t *data = bitmap->RawData; // (!) IMPORTANT (!) Don't increment original pointer
  uint8_t color1, color2;
  Bool is_color1 = True;

  for (uint8_t yto = y; yto < bitmap->Height + y; yto++)
  {
    for (uint8_t xto = x; xto < bitmap->Width + x; xto++)
    {
      if (is_color1)
      {
        color1 = colorMap8bit[*data & 0x0f];
        color2 = colorMap8bit[*data >> 4];

        if (!(bitmap->IsAlpha == True && bitmap->AlphaColor == color1))
        {
          omDrawPixel(displ, xto, yto, color1);
        }

        is_color1 = False;
      }
      else
      {
        if (!(bitmap->IsAlpha == True && bitmap->AlphaColor == color2))
        {
          omDrawPixel(displ, xto, yto, color2);
        }

        data++;
        is_color1 = True;
      }
    }
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
