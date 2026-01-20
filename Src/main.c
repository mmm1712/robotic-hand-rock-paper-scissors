/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : TFT + Touch + PCA9685 Servo test
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include "usart.h"

#include "tft.h"
#include "touch.h"
#include "pca9685.h"
#include "hand.h"
#include "ui_game.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "ina219.h"


volatile uint8_t externalGestureReady = 0;
volatile uint8_t externalGesture = 0;
volatile uint8_t cameraGesture = 255;

void UART_Print(char *msg)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}



/* touch */

#define CTP_INT_PORT   GPIOC
#define CTP_INT_PIN    GPIO_PIN_6

#define CTP_RST_PORT   GPIOB
#define CTP_RST_PIN    GPIO_PIN_12

/* prototypes */

void SystemClock_Config(void);
void Error_Handler(void);

/* clock */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_HCLK   |
        RCC_CLOCKTYPE_PCLK1  |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        Error_Handler();
}

uint8_t uart_rx;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2)
    {
        uint8_t c = uart_rx;

        if (c == 'R' || c == 'r')
        {
            externalGesture = 0;  // ROCK
            externalGestureReady = 1;
        }
        else if (c == 'P' || c == 'p')
        {
            externalGesture = 1;  // PAPER
            externalGestureReady = 1;
        }
        else if (c == 'S' || c == 's')
        {
            externalGesture = 2;  // SCISSORS
            externalGestureReady = 1;
        }
        else
        {
            externalGesture = 255;  // INVALID
            externalGestureReady = 0;
        }


        HAL_UART_Receive_IT(&huart2, &uart_rx, 1);
    }
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    MX_USART2_UART_Init();


    HAL_UART_Receive_IT(&huart2, &uart_rx, 1);

    HAL_Delay(50);

    Touch_Reset();
    TFT_Init();

    PCA9685_Init();
    PCA9685_SetPWMFreq(50);
    INA219_Init();
    UI_GameInit();

    while (1)
    {
        UI_GameUpdate();
        HAL_Delay(5);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1);
}
