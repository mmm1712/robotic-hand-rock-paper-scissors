#include "hand.h"
#include "pca9685.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>

/* ================= GESTURES ================= */
void Thumb_SetSide(uint8_t side)
{
    if (side == 0)
    {
        // open / rock position
        PCA9685_SetPWM(5, 0, THUMB_LEFT);
    }
    else
    {
        // closed / grip position
        PCA9685_SetPWM(5, 0, THUMB_RIGHT);
    }
}
void Hand_Rock(void)
{
    Thumb_SetSide(THUMB_SIDE_IN);

    PCA9685_SetServoAngle(SERVO_THUMB,  THUMB_CLOSED);
    PCA9685_SetServoAngle(SERVO_INDEX,  INDEX_CLOSED);
    PCA9685_SetServoAngle(SERVO_MIDDLE, MIDDLE_CLOSED);
    PCA9685_SetServoAngle(SERVO_RINGP,  RINGP_CLOSED);
}



void Hand_Paper(void)
{
    Thumb_SetSide(THUMB_SIDE_OUT);

    PCA9685_SetServoAngle(SERVO_THUMB,  THUMB_OPEN);
    PCA9685_SetServoAngle(SERVO_INDEX,  FingerAngle(SERVO_INDEX, 0));
    PCA9685_SetServoAngle(SERVO_MIDDLE, FingerAngle(SERVO_MIDDLE, 0));
    PCA9685_SetServoAngle(SERVO_RINGP,  RINGP_OPEN);
}

void Hand_Scissors(void)
{
    Thumb_SetSide(THUMB_SIDE_IN);

    PCA9685_SetServoAngle(SERVO_THUMB,  THUMB_CLOSED);
    PCA9685_SetServoAngle(SERVO_INDEX,  INDEX_OPEN);
    PCA9685_SetServoAngle(SERVO_MIDDLE, MIDDLE_OPEN);
    PCA9685_SetServoAngle(SERVO_RINGP,  RINGP_CLOSED);
}

/* ================= THINKING ================= */

void Hand_Thinking(void)
{
    static uint32_t last = 0;
    static uint8_t open = 0;

    if (HAL_GetTick() - last < 700)
        return;

    last = HAL_GetTick();

    if (open)
    {
        PCA9685_SetServoAngle(SERVO_INDEX,  INDEX_OPEN);
        PCA9685_SetServoAngle(SERVO_MIDDLE, MIDDLE_OPEN);
        PCA9685_SetServoAngle(SERVO_RINGP,  RINGP_OPEN);
    }
    else
    {
        PCA9685_SetServoAngle(SERVO_INDEX,  INDEX_CLOSED);
        PCA9685_SetServoAngle(SERVO_MIDDLE, MIDDLE_CLOSED);
        PCA9685_SetServoAngle(SERVO_RINGP,  RINGP_CLOSED);
    }

    open ^= 1;
}


void Hand_TestLoop(void)
{

	Hand_Paper();
	HAL_Delay(2500);
    // fully open hand
    Hand_Rock();
    HAL_Delay(2500);

    Hand_Scissors();
    HAL_Delay(2500);
}

void PCA9685_RawTest(uint8_t ch)
{
    while (1)
    {
        PCA9685_SetPWM(ch, 0, 150);
        HAL_Delay(1500);

        PCA9685_SetPWM(ch, 0, 250);
        HAL_Delay(1500);

        PCA9685_SetPWM(ch, 0, 350);
        HAL_Delay(1500);

        PCA9685_SetPWM(ch, 0, 450);
        HAL_Delay(1500);

        PCA9685_SetPWM(ch, 0, 550);
        HAL_Delay(1500);

        PCA9685_SetPWM(ch, 0, 650);
        HAL_Delay(1500);
    }
}



float FingerAngle(uint8_t finger, float t)
{
    // t = 0 → open
    // t = 1 → closed

    switch (finger)
    {
        case SERVO_INDEX:
            return INDEX_OPEN +
                   t * (INDEX_CLOSED - INDEX_OPEN);

        case SERVO_MIDDLE:
            return MIDDLE_OPEN +
                   t * (MIDDLE_CLOSED - MIDDLE_OPEN);

        default:
            return 90;
    }
}




/* ================= RANDOM ================= */

void Hand_RandomGesture(void)
{
    static uint32_t lastChange = 0;
    static uint8_t current = 0;

    if (HAL_GetTick() - lastChange < 3000)
        return;

    lastChange = HAL_GetTick();

    current = rand() % 3;

    switch (current)
    {
        case 0:
            Hand_Rock();
            break;

        case 1:
            Hand_Paper();
            break;

        case 2:
            Hand_Scissors();
            break;
    }
}

