
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
//  omWidgetT mainWidgets[MainWidgetsNumOf];

  screenMain.Id = IdScreenMain;
  screenMain.Display = &oled1;
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

  screenMain.Widgets[0].Id = 7;
  screenMain.Widgets[0].Screen = &screenMain;
  screenMain.Widgets[0].Config = &wgtCfgMainSpeed;
  screenMain.Widgets[0].PosX = 0;
  screenMain.Widgets[0].PosY = 0;
  screenMain.Widgets[0].Width = 16;
  screenMain.Widgets[0].Height = 16;
  screenMain.Widgets[0].InitCallback = WgtMainVoltCb;
  screenMain.Widgets[0].UpdateCallback =WgtMainSpeedCb;

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
