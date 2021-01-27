
#include "omgui.h"
#include "screens.h"
#include "main.h"

#include "ssd1306.h"
#include "stm32l4xx_hal.h"
#include "tim.h"


omDisplayT oled1;
omScreenT screenLogo, screenMain, screenData, screenTemp, screenSetup;

void DisplayInitCb(omDisplayT *);


void OLED_GUI_Init(void)
{
  oled1.Id = 0;
  oled1.Screen = (config.ShowLogo == True) ? &screenLogo : &screenMain;
  oled1.InitCallback = DisplayInitCb;

  LogoScreenInit();
  MainScreenInit();

  omDisplayInit(&oled1);

  HAL_Delay(1000);

  omScreenSelect(&screenMain);
}

void DisplayInitCb(omDisplayT *displ)
{
  /* HAL_TIM_IC_Start_IT is not enabling HAL_TIM_PeriodElapsedCallback,
   * so we need to manually enable it. See:
   * https://community.st.com/s/question/0D50X00009hpBdlSAE/timer3-update-event-interrupt-not-working-properly
   */
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

  ssd1306_Init();
  return;
}
