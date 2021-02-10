#ifndef __SH1122_H
#define __SH1122_H

#include <stdio.h> 
#include "font.h"

// Display size
#define SH1122_OLED_WIDTH 256
#define SH1122_OLED_HEIGHT 64
#define SH1122_OLED_RAM_SIZE (SH1122_OLED_WIDTH * SH1122_OLED_HEIGHT / 2)

#define OLED_POWER_OFF 0
#define OLED_POWER_ON 1

#define OLED_ENTIRE_OFF 0
#define OLED_ENTIRE_ON 1

#define OLED_MODE_NORMAL 0
#define OLED_MODE_REVERSE 1

#define OLED_SCAN_NORMAL 0
#define OLED_SCAN_REVERSE 1

#define OLED_DISP_NORMAL 0
#define OLED_DISP_ROTATE180 1

#define LEFT 0
#define RIGHT 1
#define CENTER 2
#define TOP 3
#define BOTTOM 4

#define OLED_GRAY_00 0x00
#define OLED_GRAY_01 0x01
#define OLED_GRAY_02 0x02
#define OLED_GRAY_03 0x03
#define OLED_GRAY_04 0x04
#define OLED_GRAY_05 0x05
#define OLED_GRAY_06 0x06
#define OLED_GRAY_07 0x07
#define OLED_GRAY_08 0x08
#define OLED_GRAY_09 0x09
#define OLED_GRAY_10 0x0a
#define OLED_GRAY_11 0x0b
#define OLED_GRAY_12 0x0c
#define OLED_GRAY_13 0x0d
#define OLED_GRAY_14 0x0e
#define OLED_GRAY_15 0x0f

void SH1122_DrawPixel(uint16_t x, uint16_t y, uint8_t color);
void SH1122_DisplayUpdate(void);

//----------------------------------------------------------------------------------------
// SH1122 Control Functions
//----------------------------------------------------------------------------------------

// 1 and 2: Set Lower (00H - 0FH) and Higher (10H - 17H) Column Address of display RAM
void SH1122_SetColumnAddress(uint8_t Value);
// 6: Set Display Start Line: (40H - 7FH)
void SH1122_SetDisplayStartLine(uint8_t Value);
// 7: Set Contrast Control Register
void SH1122_SetContrast(uint8_t Value);
// 9: Set Entire Display OFF/ON: (A4H - A5H)
void SH1122_EntireDisplay(uint8_t State);
// 10: Set Normal/Reverse Display: (A6H - A7H)
void SH1122_DisplayMode(uint8_t State);
// 11: Set Multiplex Ration
void SH1122_SetMultiplexRation(uint8_t Value);
// 13: Display Power Control
void SH1122_DisplayPower(uint8_t State);
// 16: Set Display Offset
void SH1122_SetDisplayOffset(uint8_t Value);
// 25: Write Display Data
void SH1122_WriteData(uint8_t *pData, uint32_t DataLen);
// Clear display RAM
void SH1122_ClearRAM(void);

//----------------------------------------------------------------------------------------
// High Level Display Functions
//----------------------------------------------------------------------------------------

// Rotate 180 degrees
void SH1122_DisplaySetOrienation(uint8_t State);
// Init display
void SH1122_DisplayInit(void);
// Update display
void SH1122_DisplayUpdate(void);
// Draw a pixel in (x, y) coordinates
void SH1122_DrawPixel(uint16_t x, uint16_t y, uint8_t color);
// Draw a line from (x1, y1) to (x2, y2)
void SH1122_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
/*
// Draw rectangle
void Frame_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
// Draw rectangle filled
void Frame_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t bColor, uint8_t fColor);
// Draw circle
void Frame_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint8_t color);
// Draw symbol
uint8_t Frame_DrawChar(uint16_t X, uint16_t Y, uint8_t FontID, uint8_t Char, uint8_t color);
// Draw string
int16_t Frame_DrawString(uint16_t X, uint16_t Y, uint8_t FontID, uint8_t *Str, uint8_t hAlign, uint8_t color);
// Draw formatted string
int16_t Frame_printf(uint16_t X, uint16_t Y, uint8_t FontID, uint8_t color, uint8_t hAlign, uint8_t vAlign, const char *args, ...);
*/
#endif