
#include "omgui.h"
#include "screens.h"
#include "main.h"


void ScreenMainShowCb(omScreenT *);
void WgtMainSpeedCb(omWidgetT *);
void WgtMainTachoCb(omWidgetT *);
void WgtMainVoltCb(omWidgetT *);
void WgtMainOdoCb(omWidgetT *);


void MainScreenInit(void)
{
  const uint8_t MainWidgetsNumOf = 4;
  omWidgetT mainWidgets[MainWidgetsNumOf];

  screenMain.Id = IdScreenMain;
  screenMain.Display = &oled1;
  screenMain.Widgets = &mainWidgets[0];
  screenMain.WidgetsNumOf = MainWidgetsNumOf;
  screenMain.ShowCallback = ScreenMainShowCb;
  screenMain.HideCallback = NULL;

  ////
  ///  Widget: Speed
  //
  omWidgetT wgtMainSpeed;
     struct WgtCfgMainSpeed {
    float Speed;
    UnitsSpeedT Units;
  } wgtCfgMainSpeed;

  wgtCfgMainSpeed.Speed = 0.0;
  wgtCfgMainSpeed.Units = config.SpeedUnits;

  wgtMainSpeed.Id = 0;
  wgtMainSpeed.Config = &wgtCfgMainSpeed;
  wgtMainSpeed.InitCallback = WgtMainSpeedCb; /// EMPTY !!!!!!!!!

  ////
  ///  Widget: Tacho
  //
  omWidgetT wgtMainTacho;

  struct WgtCfgMainTacho { uint16_t Tacho; } wgtCfgMainTacho;
  wgtMainTacho.Id = 1;
  wgtMainTacho.Config = &wgtCfgMainTacho;
  wgtMainTacho.InitCallback = WgtMainTachoCb; /// EMPTY !!!!!!!!!

  ////
  ///  Widget: Volt
  //
  omWidgetT wgtMainVolt;

  struct WgtCfgMainVolt { float Volt; } wgtCfgMainVolt;
  wgtMainVolt.Id = 2;
  wgtMainVolt.Config = &wgtCfgMainVolt;
  wgtMainVolt.InitCallback = WgtMainVoltCb; /// EMPTY !!!!!!!!!

  ////
  ///  Widget: Odo
  //
  omWidgetT wgtMainOdo;

  struct WgtCfgMainOdo { uint32_t Odo; } wgtCfgMainOdo;
  wgtMainOdo.Id = 3;
  wgtMainOdo.Config = &wgtCfgMainOdo;
  wgtMainOdo.InitCallback = WgtMainOdoCb; /// EMPTY !!!!!!!!!


  mainWidgets[0] = wgtMainSpeed;
  mainWidgets[1] = wgtMainTacho;
  mainWidgets[2] = wgtMainVolt;
  mainWidgets[3] = wgtMainOdo;
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
