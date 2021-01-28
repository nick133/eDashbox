
#include "omgui.h"
#include "screens.h"
#include "main.h"
#include <stdlib.h>


void ScreenMainShowCb(omScreenT *);
void WgtMainSpeedCb(omWidgetT *);
void WgtMainTachoCb(omWidgetT *);
void WgtMainVoltCb(omWidgetT *);
void WgtMainOdoCb(omWidgetT *);


void MainScreenInit(void)
{
  const uint8_t MainWidgetsNumOf = 1;
  omWidgetT mainWidgets[MainWidgetsNumOf];

  screenMain.Id = IdScreenMain;
  screenMain.Display = &oled1;
//  screenMain.Widgets = malloc(sizeof(omWidgetT) * 1);
  screenMain.WidgetsNumOf = MainWidgetsNumOf;
  screenMain.ShowCallback = ScreenMainShowCb;
  screenMain.HideCallback = NULL;

  ////
  ///  Widget: Speed
  //

  struct WgtCfgMainSpeed {
    float Speed;
    UnitsSpeedT Units;
  } wgtCfgMainSpeed;

  wgtCfgMainSpeed.Speed = 0.0;
  wgtCfgMainSpeed.Units = config.SpeedUnits;

  mainWidgets[0].Id = 7;
  mainWidgets[0].Screen = &screenMain;
  mainWidgets[0].Config = &wgtCfgMainSpeed;
  mainWidgets[0].PosX = 0;
  mainWidgets[0].PosY = 0;
  mainWidgets[0].Width = 16;
  mainWidgets[0].Height = 16;
  mainWidgets[0].InitCallback = WgtMainSpeedCb;
  mainWidgets[0].UpdateCallback = NULL;

  ////
  ///  Widget: Tacho
  //
//  omWidgetT wgtMainTacho;
//
//  struct WgtCfgMainTacho { uint16_t RPM; } wgtCfgMainTacho;
//  wgtCfgMainTacho.RPM = 0;
//
//  wgtMainTacho.Id = 1;
//  wgtMainTacho.Config = &wgtCfgMainTacho;
//  wgtMainTacho.InitCallback = WgtMainTachoCb;
//
//  ////
//  ///  Widget: Volt
//  //
//  omWidgetT wgtMainVolt;
//
//  struct WgtCfgMainVolt { float Volt; } wgtCfgMainVolt;
//  wgtCfgMainVolt.Volt = 0.0;
//
//  wgtMainVolt.Id = 2;
//  wgtMainVolt.Config = &wgtCfgMainVolt;
//  wgtMainVolt.InitCallback = WgtMainVoltCb;
//
//  ////
//  ///  Widget: Odo
//  //
//  omWidgetT wgtMainOdo;
//
//  struct WgtCfgMainOdo { uint32_t Kilos; } wgtCfgMainOdo;
//  wgtCfgMainOdo.Kilos = 0;
//
//  wgtMainOdo.Id = 3;
//  wgtMainOdo.Config = &wgtCfgMainOdo;
//  wgtMainOdo.InitCallback = WgtMainOdoCb;

  screenMain.Widgets = &mainWidgets;

  int id=screenMain.Widgets[0].Id;
}

void ScreenMainShowCb(omScreenT *screen)
{
  return;
}

void WgtMainSpeedCb(omWidgetT *wgt)
{
  return;
}

void WgtMainTachoCb(omWidgetT *wgt)
{
  return;
}

void WgtMainVoltCb(omWidgetT *wgt)
{
  return;
}

void WgtMainOdoCb(omWidgetT *wgt)
{
  return;
}
