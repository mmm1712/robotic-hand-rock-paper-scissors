#include "touch.h"
#include "i2c.h"
#include "gpio.h"

#define FT6236_ADDR   (0x38 << 1)
#define REG_TD_STATUS 0x02
#define REG_XH        0x03

#define INT_PORT GPIOC
#define INT_PIN  GPIO_PIN_6

#define RST_PORT GPIOB
#define RST_PIN  GPIO_PIN_12

void Touch_Reset(void)
{
    HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_SET);
    HAL_Delay(200);
}

bool Touch_Read(uint16_t *x, uint16_t *y)
{
    uint8_t pts = 0;
    uint8_t buf[4];

    if (HAL_I2C_Mem_Read(&hi2c1,
                         FT6236_ADDR,
                         REG_TD_STATUS,
                         1,
                         &pts,
                         1,
                         100) != HAL_OK)
        return false;

    if ((pts & 0x0F) == 0)
        return false;

    if (HAL_I2C_Mem_Read(&hi2c1,
                         FT6236_ADDR,
                         REG_XH,
                         1,
                         buf,
                         4,
                         100) != HAL_OK)
        return false;

    *x = ((buf[0] & 0x0F) << 8) | buf[1];
    *y = ((buf[2] & 0x0F) << 8) | buf[3];

    return true;
}
