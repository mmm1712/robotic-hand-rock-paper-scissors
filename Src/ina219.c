#include "ina219.h"

extern I2C_HandleTypeDef hi2c3;

/* ============================= */
/* I2C address                   */
/* ============================= */
#define INA219_ADDR (0x40 << 1)

/* ============================= */
/* Registers                     */
/* ============================= */
#define INA219_REG_CONFIG        0x00
#define INA219_REG_SHUNT_VOLT    0x01
#define INA219_REG_BUS_VOLT      0x02
#define INA219_REG_POWER         0x03
#define INA219_REG_CURRENT       0x04
#define INA219_REG_CALIBRATION   0x05


/* ============================= */
/* Low-level I2C                 */
/* ============================= */

static uint16_t INA219_Read(uint8_t reg)
{
    uint8_t buf[2];

    HAL_I2C_Master_Transmit(&hi2c3, INA219_ADDR, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c3, INA219_ADDR, buf, 2, HAL_MAX_DELAY);

    return (buf[0] << 8) | buf[1];
}

static void INA219_Write(uint8_t reg, uint16_t value)
{
    uint8_t buf[3];

    buf[0] = reg;
    buf[1] = value >> 8;
    buf[2] = value & 0xFF;

    HAL_I2C_Master_Transmit(&hi2c3, INA219_ADDR, buf, 3, HAL_MAX_DELAY);
}


/* ============================= */
/* Init + calibration            */
/* ============================= */

void INA219_Init(void)
{
    // 32V bus, 320mV shunt, 12-bit ADC
    INA219_Write(INA219_REG_CONFIG, 0x019F);

    // calibration for 0.01 ohm shunt
    INA219_Write(INA219_REG_CALIBRATION, 409);
}

float INA219_GetVoltage(void)
{
    uint16_t raw = INA219_Read(INA219_REG_BUS_VOLT);
    raw >>= 3;
    return raw * 0.004f;   // 4 mV per bit
}

float INA219_GetCurrent(void)
{
    int16_t raw = INA219_Read(INA219_REG_CURRENT);
    return raw * 0.0001f;  // 0.1 mA per bit
}

float INA219_GetPower(void)
{
    uint16_t raw = INA219_Read(INA219_REG_POWER);
    return raw * 0.002f;   // W
}

