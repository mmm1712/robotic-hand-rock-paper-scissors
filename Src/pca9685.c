#include "pca9685.h"
#include "i2c.h"
#include <math.h>


#define PCA9685_ADDR   (0x40 << 1)
#define MODE1          0x00
#define MODE2          0x01
#define PRESCALE       0xFE

#define LED0_ON_L      0x06


extern I2C_HandleTypeDef hi2c2;



static void PCA_Write(uint8_t reg, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c2,
                      PCA9685_ADDR,
                      reg,
                      1,
                      &data,
                      1,
                      HAL_MAX_DELAY);
}

static void PCA_Read(uint8_t reg, uint8_t *data)
{
    HAL_I2C_Mem_Read(&hi2c2,
                     PCA9685_ADDR,
                     reg,
                     1,
                     data,
                     1,
                     HAL_MAX_DELAY);
}


void PCA9685_Init(void)
{
    PCA_Write(MODE1, 0x20);
    HAL_Delay(10);

    PCA_Write(MODE2, 0x04);

    PCA9685_SetPWMFreq(50.0f);

    HAL_Delay(10);
}



void PCA9685_SetPWMFreq(float freq)
{
    float prescaleval = 25000000.0f;
    prescaleval /= 4096.0f;
    prescaleval /= freq;
    prescaleval -= 1.0f;

    uint8_t prescale = (uint8_t)(prescaleval + 0.5f);

    uint8_t oldmode;
    PCA_Read(MODE1, &oldmode);

    uint8_t sleep = (oldmode & 0x7F) | 0x10;

    PCA_Write(MODE1, sleep);
    PCA_Write(PRESCALE, prescale);
    PCA_Write(MODE1, oldmode);

    HAL_Delay(5);

    PCA_Write(MODE1, oldmode | 0x80);
}



void PCA9685_SetPWM(uint8_t channel,
                    uint16_t on,
                    uint16_t off)
{
    uint8_t data[4];

    data[0] = on & 0xFF;
    data[1] = on >> 8;
    data[2] = off & 0xFF;
    data[3] = off >> 8;

    HAL_I2C_Mem_Write(&hi2c2,
                      PCA9685_ADDR,
                      LED0_ON_L + 4 * channel,
                      1,
                      data,
                      4,
                      HAL_MAX_DELAY);
}


void PCA9685_SetServoAngle(uint8_t channel, float angle)
{
    if (angle < 0)   angle = 0;
    if (angle > 180) angle = 180;

    static float servo_min[16] =
    {
        90,  // thumb
        90,   // index
        102,  // middle
        90    // ring+pinky
    };

    static float servo_max[16] =
    {
        630,  // thumb
        630,  // index
        630,  // middle
        630
    };

    float pulse =
        servo_min[channel] +
        (angle / 180.0f) *
        (servo_max[channel] - servo_min[channel]);

    PCA9685_SetPWM(channel, 0, (uint16_t)pulse);
}




