#include "main.h"
#include "stm32f0xx_hal.h"
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

// void HAL_USART_MspInit()
// {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
// __HAL_RCC_GPIOC_CLK_ENABLE();

//     GPIO_InitStruct.Pin = GPIO_PIN_4;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF1_USART3;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = GPIO_PIN_5;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_PULLUP;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     GPIO_InitStruct.Alternate = GPIO_AF1_USART3;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
// }
