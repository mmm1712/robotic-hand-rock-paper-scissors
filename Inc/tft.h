#ifndef TFT_DRIVER_H
#define TFT_DRIVER_H

#include "main.h"

#define TFT_W 320
#define TFT_H 240

void TFT_Init(void);
void TFT_Fill(uint16_t color);
void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void DrawChar(uint16_t x, uint16_t y, char c, uint16_t color);
void DrawString(uint16_t x, uint16_t y, const char *s, uint16_t color);
void TFT_FillRect(uint16_t x,
                  uint16_t y,
                  uint16_t w,
                  uint16_t h,
                  uint16_t color);

void DrawCharScaled(
    uint16_t x,
    uint16_t y,
    char c,
    uint16_t color,
    uint8_t scale);


#endif
