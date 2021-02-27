/*
 * omgui.h
 *
 *  OLED MCU GUI Library
 *
 *  Created on: 23 янв. 2021 г.
 *      Author: nick
 */
#include "stdbool.h"
#include "omgui.h"
#include "main.h"

#include "sh1122.h"
#include "sh1122_conf.h"


void omGui_Init(omGuiT *ui)
{
    ui->InitCallback(ui); // OLED Driver's Init code goes here
    ui->ActiveScreen=NULL; // Discarded if set, use omScreen_Select()
}


void omGui_DeInit(omGuiT *ui)
{
    ui->DeInitCallback(ui); // OLED Driver's DeInit code goes here
}

bool omScreen_Update(omGuiT *ui)
{
    if(ui->ActiveScreen->UpdateCallback != NULL
        && ui->ActiveScreen->UpdateCallback(ui->ActiveScreen))
    {
        omGui_Update(ui);
        return true;
    }

    return false;
}


bool omScreen_Select(omScreenT *screen)
{
    if(omScreen_IsActive(screen) == true) { return false; }

    if(screen->Ui->ActiveScreen != NULL && screen->Ui->ActiveScreen->HideCallback != NULL)
        { screen->Ui->ActiveScreen->HideCallback(screen); }

    omGui_Clear(screen->Ui);

    if(screen->ShowCallback != NULL)
        { screen->ShowCallback(screen); }

    screen->Ui->ActiveScreen = screen;
    omGui_Update(screen->Ui);

    return true;
}


bool omScreen_IsActive(omScreenT *screen)
{
    return (screen->Ui->ActiveScreen == NULL
        || screen->Ui->ActiveScreen->Id != screen->Id) ? false : true;
}


void omGui_Update(omGuiT *ui)
{
    ui->UpdateCallback(ui);
}


void omGui_Clear(omGuiT *ui)
{
    ui->ClearCallback(ui);
}


void omGui_DrawPixel(omGuiT *ui, uint32_t x, uint32_t y, uint8_t color)
{
    ui->DrawPixelCallback(ui, x, y, color);
}


/* Store bitmap RawData in FLASH memory, not RAM, image buffer is corrupted otherwise.
 * Data array must be declared as global 'const'. Further reading:
 * https://electronics.stackexchange.com/questions/74589/how-to-stock-variables-in-flash-memory
 * https://forum.arduino.cc/index.php?topic=461487.0
 */
void omGui_DrawBitmap(omGuiT *ui, const omBitmapT *bitmap, uint32_t x, uint32_t y, bool alpha, bool update)
{
    uint8_t color1, color2;
    uint32_t idx = 0;
    bool is_color1 = true;

    for(uint16_t yto = y; yto < bitmap->Height + y; yto++)
    {
        for(uint16_t xto = x; xto < bitmap->Width + x; xto++)
        {
            if(is_color1)
            {
                color1 = bitmap->RawData[idx] >> 4;
                color2 = bitmap->RawData[idx] & 0x0f;

                if(!(alpha == true && color1 == 0x0))
                    { omGui_DrawPixel(ui, xto, yto, color1); }

                is_color1 = false;
            }
            else
            {
                if(!(alpha == true && color2 == 0x0))
                    { omGui_DrawPixel(ui, xto, yto, color2); }

                idx++;
                is_color1 = true;
            }
        }
    }

    if(update) { omGui_Update(ui); }
}


/*
 * x1, x2, y1, y2 = 0  croped area is 1x1 pixel, not 0!
 */
/* void omGui_DrawBitmapCropped(omGuiT *ui, omBitmapT *bitmap,
        uint32_t x, uint32_t y,
        uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2,
        bool alpha, bool update)
{
    uint8_t color1, color2;
    uint32_t idx = 0;
    bool is_color1 = true;

    for(uint16_t yto = y; yto < bitmap->Height + y; yto++)
    {
        for(uint16_t xto = x; xto < bitmap->Width + x; xto++)
        {
            if(is_color1)
            {
                color1 = bitmap->RawData[idx] >> 4;
                color2 = bitmap->RawData[idx] & 0x0f;

                if(!(alpha == true && color1 == 0x0)
                        && xto >= x1+x && xto <= x2+x && yto >= y1+y && yto <= y2+y)
                    { omGui_DrawPixel(ui, xto, yto, color1); }

                is_color1 = false;
            }
            else
            {
                if(!(alpha == true && color2 == 0x0)
                        && xto >= x1+x && xto <= x2+x && yto >= y1+y && yto <= y2+y)
                    { omGui_DrawPixel(ui, xto, yto, color2); }

                idx++;
                is_color1 = true;
            }
        }
    }

    if(update) { omGui_Update(ui); }
} */


void omGui_DrawLine(omGuiT *ui, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color, bool update)
{
    const int16_t deltaX = abs(x2 - x1);
    const int16_t deltaY = abs(y2 - y1);
    const int16_t signX = x1 < x2 ? 1 : -1;
    const int16_t signY = y1 < y2 ? 1 : -1;

    int16_t error = deltaX - deltaY;

    omGui_DrawPixel(ui, x2, y2, color);

    while(x1 != x2 || y1 != y2)
    {
        omGui_DrawPixel(ui, x1, y1, color);
        const int16_t error2 = error * 2;

        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }

    if(update) { omGui_Update(ui); }
}


// Draw rectangle
void omGui_DrawRectangle(omGuiT *ui, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, bool update)
{
    omGui_DrawLine(ui, x1, y1, x1, y2, color, false);
    omGui_DrawLine(ui, x2, y1, x2, y2, color, false);
    omGui_DrawLine(ui, x1, y1, x2, y1, color, false);
    omGui_DrawLine(ui, x1, y2, x2, y2, color, false);

    if(update) { omGui_Update(ui); }
}


// Draw rectangle filled
void omGui_DrawRectangleFilled(omGuiT *ui, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bColor, uint8_t fColor, bool update)
{
    if(x1 > x2)
    {
        int16_t Temp = x1;
        x1 = x2;
        x2 = Temp;
    }
    
    if(y1 > y2)
    {
        int16_t Temp = y1;
        y1 = y2;
        y2 = Temp;
    }

    // Fill
    int16_t y_start = y1;

    while(y_start <= y2)
    {
        omGui_DrawLine(ui, x1, y_start, x2, y_start, fColor, false);
        y_start++;
    }

    // Border
    omGui_DrawLine(ui, x1, y1, x1, y2, bColor, false);
    omGui_DrawLine(ui, x2, y1, x2, y2, bColor, false);
    omGui_DrawLine(ui, x1, y1, x2, y1, bColor, false);
    omGui_DrawLine(ui, x1, y2, x2, y2, bColor, false);

    if(update) { omGui_Update(ui); }
}


/* void omAnimationStart(omAnimationT *anim)
{
    omBitmapT *bitmap = anim->Bitmaps; // Don't increment original pointer

    for(uint16_t i = 0; i < anim->FramesNumOf; i++)
    {
        omGui_DrawBitmap(bitmap, anim->PosX, anim->PosY);
        omGui_Update(anim->Ui);
        SYS_SLEEP(anim->Interval);

        bitmap++;
    }

    return;
} */
