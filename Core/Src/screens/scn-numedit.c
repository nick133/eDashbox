#include <string.h>
#include <math.h>
#include "printf.h"

#include "sh1122.h"

#include "main.h"
#include "rtos.h"

#include "settings.h"
#include "bitmaps.h"
#include "omgui.h"
#include "screens.h"


static void ScreenShowCb(omScreenT *);
static void ScreenHideCb(omScreenT *);
static bool ScreenUpdateCb(omScreenT *);

static void NextNumCb(IdButtonT, BtnEventKindT, void *Params);
static void IncNumCb(IdButtonT, BtnEventKindT, void *Params);
static void SaveSettingsCb(IdButtonT, BtnEventKindT, void *Params);


void NumEditScreen_Init(void)
{
    screenMain.Id = IdScreenNumEditor;
    screenMain.Ui = &oledUi;
    screenMain.ShowCallback = ScreenShowCb;
    screenMain.HideCallback = ScreenHideCb;
    screenMain.UpdateCallback = ScreenUpdateCb;
}

/*
    Modes:
    12345
    12345.[123]
    12:23

    Params:
    max val
    min val
*/

static void ScreenShowCb(omScreenT *screen)
{


    RegButtonEvent(IdButton1, EvtButtonPress, NextNumCb, NULL);
    RegButtonEvent(IdButton2, EvtButtonPress, IncNumCb, NULL);

    RegButtonEvent(IdButton1, EvtButtonLongPress, SaveSettingsCb, NULL);
    RegButtonEvent(IdButton2, EvtButtonLongPress, SaveSettingsCb, NULL);
}


static void ScreenHideCb(omScreenT *screen)
{

}


static bool ScreenUpdateCb(omScreenT *screen)
{
    return true;
}


static void NextNumCb(IdButtonT Btn, BtnEventKindT EvtKind, void *Params)
{

}


static void IncNumCb(IdButtonT Btn, BtnEventKindT EvtKind, void *Params)
{

}


static void SaveSettingsCb(IdButtonT Btn, BtnEventKindT EvtKind, void *Params)
{

}