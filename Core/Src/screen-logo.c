
#include "omgui.h"
#include "main.h"
#include "screens.h"


void ScreenLogoShowCb(omScreenT *);


void LogoScreenInit(void)
{

  const uint8_t LogoWidgetsNumOf = 1;
  omWidgetT logoWidgets[LogoWidgetsNumOf];

  screenLogo.Id = IdScreenLogo;
  screenLogo.Display = &oled1;
  screenLogo.Widgets = logoWidgets;
  screenLogo.ShowCallback = ScreenLogoShowCb;
  screenLogo.HideCallback = NULL;

  ////
  ///  Widget: Logo
  //
  omWidgetT wgtLogoImage;
  wgtLogoImage.Id = 0;

  struct WgtCfgLogoImage {
    float Speed;
  } wgtCfgLogoImage;

  wgtLogoImage.Id = 0;
  wgtLogoImage.Config = &wgtCfgLogoImage;


  logoWidgets[0] = wgtLogoImage;
}


void ScreenLogoShowCb(omScreenT *screen)
{
  return;
}
