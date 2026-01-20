#include "tft.h"
#include "spi.h"
#include "gpio.h"
#include <string.h>

#define FONT_SCALE 2

#define CS_LOW()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define DC_LOW()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET)
#define DC_HIGH()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET)
#define RST_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define RST_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)

static const uint8_t font5x7[][5] = {
    {0x3E,0x51,0x49,0x45,0x3E},
    {0x00,0x42,0x7F,0x40,0x00},
    {0x62,0x51,0x49,0x49,0x46},
    {0x22,0x49,0x49,0x49,0x36},
    {0x18,0x14,0x12,0x7F,0x10},
    {0x2F,0x49,0x49,0x49,0x31},
    {0x3E,0x49,0x49,0x49,0x30},
    {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},
    {0x06,0x49,0x49,0x49,0x3E},
    {0,0,0,0,0},
    {0x7E,0x11,0x11,0x11,0x7E},
    {0x7F,0x49,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41},
    {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F},
    {0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x04,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F},
    {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E},
    {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01},
    {0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F},
    {0x7F,0x20,0x10,0x20,0x7F},
    {0x63,0x14,0x08,0x14,0x63},
    {0x03,0x04,0x78,0x04,0x03},
    {0x61,0x51,0x49,0x45,0x43},
	{0x02, 0x01, 0x59, 0x06, 0x00},

};

void TFT_FillRect(uint16_t x,
                  uint16_t y,
                  uint16_t w,
                  uint16_t h,
                  uint16_t color)
{
    for (uint16_t yy = 0; yy < h; yy++)
    {
        for (uint16_t xx = 0; xx < w; xx++)
        {
            DrawPixel(x + xx, y + yy, color);
        }
    }
}


static void TFT_Reset(void)
{
    RST_LOW();
    HAL_Delay(20);
    RST_HIGH();
    HAL_Delay(120);
}

static inline uint16_t FIX_COLOR(uint16_t c)
{
    uint16_t r = (c >> 11) & 0x1F;
    uint16_t g = (c >> 5)  & 0x3F;
    uint16_t b = c & 0x1F;
    uint16_t bgr = (b << 11) | (g << 5) | r;
    return (bgr << 8) | (bgr >> 8);
}

static void TFT_Cmd(uint8_t cmd)
{
    DC_LOW(); CS_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    CS_HIGH();
}

static void TFT_Data(uint8_t *d, uint16_t n)
{
    DC_HIGH(); CS_LOW();
    HAL_SPI_Transmit(&hspi1, d, n, HAL_MAX_DELAY);
    CS_HIGH();
}

static void TFT_SetAddr(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
    uint8_t d[4];

    TFT_Cmd(0x2A);
    d[0]=x0>>8; d[1]=x0; d[2]=x1>>8; d[3]=x1;
    TFT_Data(d,4);

    TFT_Cmd(0x2B);
    d[0]=y0>>8; d[1]=y0; d[2]=y1>>8; d[3]=y1;
    TFT_Data(d,4);

    TFT_Cmd(0x2C);
}


void TFT_Init(void)
{
    uint8_t d;
    TFT_Reset();

    TFT_Cmd(0x11);
    HAL_Delay(120);

    TFT_Cmd(0x3A); d=0x55; TFT_Data(&d,1);
    TFT_Cmd(0x36); d=0x20; TFT_Data(&d,1);
    TFT_Cmd(0x29);
}



void TFT_Fill(uint16_t c)
{
    uint16_t cc = FIX_COLOR(c);
    uint8_t d[2] = { cc >> 8, cc & 0xFF };

    TFT_SetAddr(0,0,TFT_W-1,TFT_H-1);

    DC_HIGH(); CS_LOW();
    for(uint32_t i=0;i<TFT_W*TFT_H;i++)
        HAL_SPI_Transmit(&hspi1,d,2,HAL_MAX_DELAY);
    CS_HIGH();
}

void DrawPixel(uint16_t x,uint16_t y,uint16_t color)
{
    if(x>=TFT_W||y>=TFT_H) return;

    uint16_t cc = FIX_COLOR(color);
    uint8_t d[2] = { cc >> 8, cc & 0xFF };

    TFT_SetAddr(x,y,x,y);
    TFT_Data(d,2);
}

void DrawChar(uint16_t x,uint16_t y,char c,uint16_t color)
{
    const uint8_t *g=NULL;

    if(c>='0'&&c<='9') g=font5x7[c-'0'];
    else if(c==' ')   g=font5x7[10];
    else if (c >= 'A' && c <= 'Z')
        g = font5x7[c - 'A' + 11];
    else if (c == '?')
        g = font5x7[11 + 26];   // after A–Z
    else
        return;


    for(uint8_t col=0;col<5;col++)
        for(uint8_t row=0;row<7;row++)
            if(g[col]&(1<<row))
                for(uint8_t dx=0;dx<FONT_SCALE;dx++)
                    for(uint8_t dy=0;dy<FONT_SCALE;dy++)
                        DrawPixel(
                            x + col*FONT_SCALE + dx,
                            y + row*FONT_SCALE + dy,
                            color
                        );
}

void DrawCharScaled(
    uint16_t x,
    uint16_t y,
    char c,
    uint16_t color,
    uint8_t scale)
{
    const uint8_t *g = NULL;

    if (c >= '0' && c <= '9') g = font5x7[c - '0'];
    else if (c == ' ')        g = font5x7[10];
    else if (c >= 'A' && c <= 'Z') g = font5x7[c - 'A' + 11];
    else return;

    for (uint8_t col = 0; col < 5; col++)
        for (uint8_t row = 0; row < 7; row++)
            if (g[col] & (1 << row))
                for (uint8_t dx = 0; dx < scale; dx++)
                    for (uint8_t dy = 0; dy < scale; dy++)
                        DrawPixel(
                            x + col * scale + dx,
                            y + row * scale + dy,
                            color
                        );
}


void DrawString(uint16_t x,uint16_t y,const char *s,uint16_t color)
{
    while(*s)
    {
        DrawChar(x,y,*s,color);
        x += 6 * FONT_SCALE;
        s++;
    }
}
