#pragma once
#include <stdint.h>

#define TFT_W 320
#define TFT_H 240


#define UI_BG        0xFFFF
#define UI_CARD      0xE7FF
#define UI_PRIMARY   0x051F
#define UI_ACCENT    0xF81F
#define UI_TEXT      0x2104
#define UI_TEXT_INV  0xFFFF


uint16_t CenterX(const char *text);

void DrawRoundedRect(uint16_t x, uint16_t y,
                     uint16_t w, uint16_t h,
                     uint16_t r, uint16_t color);

void DrawCenteredButton(uint16_t y, const char *text);

void DrawTextWithShadow(uint16_t x, uint16_t y,
                        const char *text,
                        uint16_t fg,
                        uint16_t shadow);

void DrawCard(uint16_t y,
              const char *line1,
              const char *line2);
