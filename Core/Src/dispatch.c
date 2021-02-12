
#include "omgui.h"
#include "dispatch.h"
#include "main.h"
#include "bitmaps.h"

/* <<<< FreeRTOS >>>> */
#include "FreeRTOS.h"
#include <queue.h>
#include <task.h>
#include <timers.h>

#include "sh1122.h"
//#include "stm32l4xx_hal.h"
#include "tim.h"
#include "onewire.h"
#include "ds18b20.h"
#include "tim.h"

#define SSR_QUEUE_LENGTH  10
#define SSR_MSG_SIZE      sizeof(SensorMessageT)

#ifdef DEBUG
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#endif



omGuiT oledUi;
omScreenT screenMain, screenData, screenTemp, screenSetup;

static StaticQueue_t SensorMessageQ;
static QueueHandle_t SensorsQueue;

static void DisplayInitCb(omGuiT *);
static void DisplayUpdateCb(omGuiT *);
static void DisplayClearCb(omGuiT *);
static void DisplayDrawPixelCb(omGuiT *, uint32_t x, uint32_t y, uint8_t color);

static void TaskDispatch(void *);
static void TaskTemperaturePoll(void *);


void Dispatch_Init(void)
{
    oledUi.Id = 0;
    oledUi.ResX = SH1122_OLED_WIDTH;
    oledUi.ResY = SH1122_OLED_HEIGHT;
    oledUi.InitCallback = DisplayInitCb;
    oledUi.DeInitCallback = NULL;
    oledUi.UpdateCallback = DisplayUpdateCb;
    oledUi.ClearCallback = DisplayClearCb;
    oledUi.DrawPixelCallback = DisplayDrawPixelCb;

    TaskHandle_t taskDispatch = NULL;
    /* Default MAX_TASK_NAME_LEN = 16 */
    BaseType_t taskDispatchRet = xTaskCreate(
        TaskDispatch,
        "GuiDispatcher",          /* Text name for the task. */
        configMINIMAL_STACK_SIZE,  /* Stack size in words, not bytes. */
        NULL,                      /* Parameter passed into the task. */
        configMAX_PRIORITIES / 2,  /* Priority of the task created. */
        &taskDispatch);
}


static void DisplayInitCb(omGuiT *ui)
{
    SH1122_DisplayInit();
}


static void DisplayUpdateCb(omGuiT *ui)
{
    SH1122_DisplayUpdate();
}


static void DisplayClearCb(omGuiT *ui)
{
    SH1122_ClearRAM();
}


static void DisplayDrawPixelCb(omGuiT *ui, uint32_t x, uint32_t y, uint8_t color)
{
    SH1122_DrawPixel(x, y, color);
}

static void TaskDispatch(void *pvParams)
{
    //f_mount(&fs, "", 1);

    /* ((( Hardware related init code
     * Must be called from inside the first main task for interrupt callbacks
     * and other tasks being able to commmunicate with each other correctly.
     */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

    TaskHandle_t taskTemperPoll = NULL;
    BaseType_t taskTempPollRet = xTaskCreate(
        TaskTemperaturePoll,
        "TempSensorPoll",
        configMINIMAL_STACK_SIZE,
        NULL,
        configMAX_PRIORITIES / 2,
        &taskTemperPoll);
    /* ))) Hardware related init code */

    uint8_t SensorMessageBufQ[ SSR_QUEUE_LENGTH * SSR_MSG_SIZE ];
    QueueHandle_t SensorsQueue = xQueueCreateStatic(
    SSR_QUEUE_LENGTH, SSR_MSG_SIZE, SensorMessageBufQ, &SensorMessageQ );

    // configASSERT(SensorsQueue);

    //omGuiInit(&oledUi);

    //Bitmaps_Init();
    MainScreenInit();

    // Show logo
    //omDrawBitmap(&oledUi, &AssetBitmaps.Logo, 0, 0, False, True);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
SEGGER_RTT_printf(0, "Hello1\n");
    //SH1122_ClearRAM();
    //omGuiClear(&oledUi);
    //omScreenSelect(&screenMain);

    while(1)
    {
        HAL_Delay(2000);
        SEGGER_RTT_printf(0, "Hello\n");

        continue;

    }
}


static void TaskTemperaturePoll(void *pvParams)
{
    char message[64];

    while(1)
    {
        uint8_t ROM_tmp[8];

        SensorMessageT SsrMsg;
        SsrMsg.Kind = SsrMsgTmp1;

        DS18B20_ReadAll();
        DS18B20_StartAll();

        for(uint8_t i = 0; i < DS18B20_Quantity(); i++)
        {
            if(DS18B20_GetTemperature(i, &SsrMsg.Data.Temperature1))
            {
                DS18B20_GetROM(i, ROM_tmp);

                if(xQueueSendToBack(SensorsQueue, (void *)&SsrMsg,
                    (TickType_t)(20 / portTICK_PERIOD_MS)) != pdPASS)
                {
                    /* Failed to post the message, even after 10 ticks. */
                }
            }   
        }

        HAL_Delay(600);
    }
}