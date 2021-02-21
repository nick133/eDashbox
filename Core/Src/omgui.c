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


void omGuiInit(omGuiT *ui)
{
    ui->InitCallback(ui); // OLED Driver's Init code goes here
    ui->ActiveScreen=NULL; // Discarded if set, use omScreenSelect()
}


void omGuiDeInit(omGuiT *ui)
{
    ui->DeInitCallback(ui); // OLED Driver's DeInit code goes here
}

void omScreenUpdate(omGuiT *ui)
{
    if(ui->ActiveScreen->UpdateCallback != NULL)
    {
        ui->ActiveScreen->UpdateCallback(ui->ActiveScreen);
    }

    omGuiUpdate(ui);
}


bool omScreenSelect(omScreenT *screen)
{
    if(omScreenIsActive(screen) == true) { return false; }

    if(screen->Ui->ActiveScreen != NULL && screen->Ui->ActiveScreen->HideCallback != NULL)
        { screen->Ui->ActiveScreen->HideCallback(screen); }

    omGuiClear(screen->Ui);

    if(screen->ShowCallback != NULL)
        { screen->ShowCallback(screen); }

    screen->Ui->ActiveScreen = screen;
    omGuiUpdate(screen->Ui);

    return true;
}


bool omScreenIsActive(omScreenT *screen)
{
    return (screen->Ui->ActiveScreen == NULL
        || screen->Ui->ActiveScreen->Id != screen->Id) ? false : true;
}


void omGuiUpdate(omGuiT *ui)
{
    ui->UpdateCallback(ui);
}


void omGuiClear(omGuiT *ui)
{
    ui->ClearCallback(ui);
}


void omDrawPixel(omGuiT *ui, uint32_t x, uint32_t y, uint8_t color)
{
    ui->DrawPixelCallback(ui, x, y, color);
}


/* Store bitmap RawData in FLASH memory, not RAM, image buffer is corrupted otherwise.
 * Data array must be declared as global 'const'. Further reading:
 * https://electronics.stackexchange.com/questions/74589/how-to-stock-variables-in-flash-memory
 * https://forum.arduino.cc/index.php?topic=461487.0
 */
void omDrawBitmap(omGuiT *ui, omBitmapT *bitmap, uint32_t x, uint32_t y, bool alpha, bool update)
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
                    { omDrawPixel(ui, xto, yto, color1); }

                is_color1 = false;
            }
            else
            {
                if(!(alpha == true && color2 == 0x0))
                    { omDrawPixel(ui, xto, yto, color2); }

                idx++;
                is_color1 = true;
            }
        }
    }

    if(update) { omGuiUpdate(ui); }
}


/*
 * x1, x2, y1, y2 = 0  croped area is 1x1 pixel, not 0!
 */
void omDrawBitmapCropped(omGuiT *ui, omBitmapT *bitmap,
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
                    { omDrawPixel(ui, xto, yto, color1); }

                is_color1 = false;
            }
            else
            {
                if(!(alpha == true && color2 == 0x0)
                        && xto >= x1+x && xto <= x2+x && yto >= y1+y && yto <= y2+y)
                    { omDrawPixel(ui, xto, yto, color2); }

                idx++;
                is_color1 = true;
            }
        }
    }

    if(update) { omGuiUpdate(ui); }
}


void omDrawLine(omGuiT *ui, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color, bool update)
{
    const int16_t deltaX = abs(x2 - x1);
    const int16_t deltaY = abs(y2 - y1);
    const int16_t signX = x1 < x2 ? 1 : -1;
    const int16_t signY = y1 < y2 ? 1 : -1;

    int16_t error = deltaX - deltaY;

    omDrawPixel(ui, x2, y2, color);

    while(x1 != x2 || y1 != y2)
    {
        omDrawPixel(ui, x1, y1, color);
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

    if(update) { omGuiUpdate(ui); }
}


// Draw rectangle
void omDrawRectangle(omGuiT *ui, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, bool update)
{
    omDrawLine(ui, x1, y1, x1, y2, color, false);
    omDrawLine(ui, x2, y1, x2, y2, color, false);
    omDrawLine(ui, x1, y1, x2, y1, color, false);
    omDrawLine(ui, x1, y2, x2, y2, color, false);

    if(update) { omGuiUpdate(ui); }
}


// Draw rectangle filled
void omDrawRectangleFilled(omGuiT *ui, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bColor, uint8_t fColor, bool update)
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
        omDrawLine(ui, x1, y_start, x2, y_start, fColor, false);
        y_start++;
    }

    // Border
    omDrawLine(ui, x1, y1, x1, y2, bColor, false);
    omDrawLine(ui, x2, y1, x2, y2, bColor, false);
    omDrawLine(ui, x1, y1, x2, y1, bColor, false);
    omDrawLine(ui, x1, y2, x2, y2, bColor, false);

    if(update) { omGuiUpdate(ui); }
}


/* void omAnimationStart(omAnimationT *anim)
{
    omBitmapT *bitmap = anim->Bitmaps; // Don't increment original pointer

    for(uint16_t i = 0; i < anim->FramesNumOf; i++)
    {
        omDrawBitmap(bitmap, anim->PosX, anim->PosY);
        omGuiUpdate(anim->Ui);
        SYS_SLEEP(anim->Interval);

        bitmap++;
    }

    return;
} */
