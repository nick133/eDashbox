
#include "omgui.h"
#include "dispatch.h"
#include "main.h"


typedef enum {
  IdWgtSpeed,
  IdWgtTacho,
  IdWgtVolt,
  IdWgtOdo,
  NumOfMainWidgets
} wgtIdScreenMain;

static void ScreenMainShowCb(omScreenT *);
static void WgtMainSpeedCb(omWidgetT *);
static void WgtMainTachoCb(omWidgetT *);
static void WgtMainVoltCb(omWidgetT *);
static void WgtMainOdoCb(omWidgetT *);


void MainScreenInit(void)
{
  screenMain.Id = IdScreenMain;
  screenMain.Ui = &oledUi;
  screenMain.WidgetsNumOf = NumOfMainWidgets;
  screenMain.ShowCallback = ScreenMainShowCb;
  screenMain.HideCallback = NULL;

  ////
  ///  Widget: Speed
  //
  struct WgtCfgMainSpeed
  {
    float Speed;
    UnitsSpeedT Units;
  } wgtCfgMainSpeed;
  wgtCfgMainSpeed.Speed = 0.0;
  wgtCfgMainSpeed.Units = config.SpeedUnits;

  screenMain.Widgets[IdWgtSpeed].Id = IdWgtSpeed;
  screenMain.Widgets[IdWgtSpeed].Screen = &screenMain;
  screenMain.Widgets[IdWgtSpeed].Config = &wgtCfgMainSpeed;
  screenMain.Widgets[IdWgtSpeed].PosX = 0;
  screenMain.Widgets[IdWgtSpeed].PosX = 0;
  screenMain.Widgets[IdWgtSpeed].PosY = 0;
  screenMain.Widgets[IdWgtSpeed].Width = 16;
  screenMain.Widgets[IdWgtSpeed].Height = 16;
  screenMain.Widgets[IdWgtSpeed].ShowCallback = WgtMainSpeedCb;
  screenMain.Widgets[IdWgtSpeed].UpdateCallback = NULL;

  ////
  ///  Widget: Tacho
  //
  struct WgtCfgMainTacho { uint16_t RPM; } wgtCfgMainTacho;
  wgtCfgMainTacho.RPM = 0;

  screenMain.Widgets[IdWgtTacho].Id = IdWgtTacho;
  screenMain.Widgets[IdWgtTacho].Screen = &screenMain;
  screenMain.Widgets[IdWgtTacho].Config = &wgtCfgMainTacho;
  screenMain.Widgets[IdWgtTacho].PosX = 0;
  screenMain.Widgets[IdWgtTacho].PosY = 0;
  screenMain.Widgets[IdWgtTacho].Width = 16;
  screenMain.Widgets[IdWgtTacho].Height = 16;
  screenMain.Widgets[IdWgtTacho].ShowCallback = WgtMainTachoCb;
  screenMain.Widgets[IdWgtTacho].UpdateCallback = NULL;

  ////
  ///  Widget: Volt
  //
  struct WgtCfgMainVolt { float Volt; } wgtCfgMainVolt;
  wgtCfgMainVolt.Volt = 0.0;

  screenMain.Widgets[IdWgtVolt].Id = IdWgtVolt;
  screenMain.Widgets[IdWgtVolt].Screen = &screenMain;
  screenMain.Widgets[IdWgtVolt].Config = &wgtCfgMainVolt;
  screenMain.Widgets[IdWgtVolt].PosX = 0;
  screenMain.Widgets[IdWgtVolt].PosY = 0;
  screenMain.Widgets[IdWgtVolt].Width = 16;
  screenMain.Widgets[IdWgtVolt].Height = 16;
  screenMain.Widgets[IdWgtVolt].ShowCallback = WgtMainVoltCb;
  screenMain.Widgets[IdWgtVolt].UpdateCallback = NULL;

  ////
  ///  Widget: Odo
  //
  struct WgtCfgMainOdo { uint32_t Kilos; } wgtCfgMainOdo;
  wgtCfgMainOdo.Kilos = 0;

  screenMain.Widgets[IdWgtOdo].Id = IdWgtOdo;
  screenMain.Widgets[IdWgtOdo].Screen = &screenMain;
  screenMain.Widgets[IdWgtOdo].Config = &wgtCfgMainOdo;
  screenMain.Widgets[IdWgtOdo].PosX = 0;
  screenMain.Widgets[IdWgtOdo].PosY = 0;
  screenMain.Widgets[IdWgtOdo].Width = 16;
  screenMain.Widgets[IdWgtOdo].Height = 16;
  screenMain.Widgets[IdWgtOdo].ShowCallback = WgtMainOdoCb;
  screenMain.Widgets[IdWgtOdo].UpdateCallback = NULL;
}

static void ScreenMainShowCb(omScreenT *screen)
{
  return;
}

static void WgtMainSpeedCb(omWidgetT *wgt)
{
  return;
}

static void WgtMainTachoCb(omWidgetT *wgt)
{
  return;
}

static void WgtMainVoltCb(omWidgetT *wgt)
{
  return;
}

static void WgtMainOdoCb(omWidgetT *wgt)
{
  return;
}