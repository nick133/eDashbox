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


static void ScreenShowCb(omScreenT *, void* params);
static void ScreenHideCb(omScreenT *);
static bool ScreenUpdateCb(omScreenT *);

static void NextNumCb(IdButtonT, BtnEventKindT, void *params);
static void IncNumCb(IdButtonT, BtnEventKindT, void *params);
static void SaveSettingsCb(IdButtonT, BtnEventKindT, void *params);


void NumEditScreen_Init(void)
{
    screenNumEdit.Id = IdScreenNumEdit;
    screenNumEdit.Ui = &oledUi;
    screenNumEdit.ShowCallback = ScreenShowCb;
    screenNumEdit.HideCallback = ScreenHideCb;
    screenNumEdit.UpdateCallback = ScreenUpdateCb;
}

/*
    Modes:
    12345
    12345.[123]
    12:23

    Params:
    Title =  Wheel circumference
    Units =  mm
    i_len, f_len, separator
    max val
    min val


    Digit
    x, y
*/

typedef struct NumEditParams
{
    char *Title;
    char *Units;
    char Sep; // '.' | ':'
    uint8_t iLen, fLen;
    float MinVal, MaxVal;
} NumEditParamsT;


static void ScreenShowCb(omScreenT *screen, void* params)
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


static void NextNumCb(IdButtonT btn, BtnEventKindT evtKind, void *params)
{

}


static void IncNumCb(IdButtonT btn, BtnEventKindT evtKind, void *params)
{

}


static void SaveSettingsCb(IdButtonT btn, BtnEventKindT evtKind, void *params)
{

}