#include "ui.h"
#include "tft.h"
#include <string.h>

uint16_t CenterX(const char *text)
{
    return (TFT_W - strlen(text) * 12) / 2;
}

void DrawRoundedRect(uint16_t x, uint16_t y,
                     uint16_t w, uint16_t h,
                     uint16_t r, uint16_t color)
{
    for (uint16_t yy = y + r; yy < y + h - r; yy++)
        for (uint16_t xx = x; xx < x + w; xx++)
            DrawPixel(xx, yy, color);

    for (uint16_t yy = y; yy < y + r; yy++)
        for (uint16_t xx = x + r; xx < x + w - r; xx++)
            DrawPixel(xx, yy, color);

    for (uint16_t yy = y + h - r; yy < y + h; yy++)
        for (uint16_t xx = x + r; xx < x + w - r; xx++)
            DrawPixel(xx, yy, color);

    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx * dx + dy * dy <= r * r)
            {
                DrawPixel(x + r + dx,     y + r + dy,     color);
                DrawPixel(x + w - r + dx, y + r + dy,     color);
                DrawPixel(x + r + dx,     y + h - r + dy, color);
                DrawPixel(x + w - r + dx, y + h - r + dy, color);
            }
}

void DrawCenteredButton(uint16_t y, const char *text)
{
    uint16_t w = 200;
    uint16_t h = 42;
    uint16_t x = (TFT_W - w) / 2;

    DrawRoundedRect(x, y, w, h, 14, UI_PRIMARY);
    DrawString(CenterX(text), y + 13, text, UI_TEXT_INV);
}

void DrawTextWithShadow(uint16_t x, uint16_t y,
                        const char *text,
                        uint16_t fg,
                        uint16_t shadow)
{
    DrawString(x + 2, y + 2, text, shadow);
    DrawString(x, y, text, fg);
}

void DrawCard(uint16_t y, const char *line1, const char *line2)
{
    uint16_t w = 260;
    uint16_t h = 70;
    uint16_t x = (TFT_W - w) / 2;

    DrawRoundedRect(x, y, w, h, 18, UI_CARD);

    DrawString(CenterX(line1), y + 14, line1, UI_TEXT);
    DrawString(CenterX(line2), y + 38, line2, UI_TEXT);
}

void DrawText(uint16_t x, uint16_t y, const char *txt, uint16_t color)
{
    DrawTextWithShadow(x, y, txt, color, UI_BG);
}

