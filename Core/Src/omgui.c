/*
 * omgui.h
 *
 *  OLED MCU GUI Library
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: nick
 */

#include "main.h"
#include "omgui.h"


// Private methods
void _omScreenInit(omScreenT *screen);


void omDisplayInit(omDisplayT *displ)
{
  displ->InitCallback(displ); // OLED Driver's Init code goes here

  _omScreenInit(displ->Screen);
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

  if (screen->Display->Screen->HideCallback != NULL)
  {
    screen->Display->Screen->HideCallback(screen);
  }

  _omScreenInit(screen);

  return True;
}


void _omScreenInit(omScreenT *screen)
{
  if (screen->ShowCallback != NULL)
  {
    screen->ShowCallback(screen);
  }

  for (uint16_t i = 0; i < screen->WidgetsNumOf; i++)
  {
    //
    // FROM HERE screen.Widgets corrupted!!!!!!!!!!
    //

    // array[1] is equal to *(array + 1)
    // https://stackoverflow.com/questions/16201607/c-pointer-to-array-of-structs
    omWidgetT *w = screen->Widgets;
    int id1 = w->Id;
    int id2 = w[0].Id;
    (screen->Widgets + i)->InitCallback(screen->Widgets + i);
//    screen->Widgets[i].InitCallback(screen->Widgets + i);
  }

  screen->Display->Screen = screen;
}


Bool omScreenIsActive(omScreenT *screen)
{
  return (screen->Display->Screen->Id == screen->Id) ? True : False;
}


void omBitmapLoad(omBitmapT *bitmap)
{
  return;
}
