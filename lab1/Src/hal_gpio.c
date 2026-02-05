#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t pin_mask;
    uint32_t isolate_vals_mask;
    uint32_t moder_set_mask;

    if (GPIOx == GPIOC){
        pin_mask = (1u << 6u) | (1u << 7u) | (1u << 8u) | (1u << 9u);

        isolate_vals_mask = (3u << 12u) | (3u << 14u) | (3u << 16u) | (3u << 18u);

        moder_set_mask = (1u << 12u) | (1u << 14u) | (1u << 16u) | (1u << 18u);

        GPIOx->MODER = (GPIOx->MODER & ~isolate_vals_mask) | moder_set_mask;
        GPIOx->OSPEEDR &= ~isolate_vals_mask;
        GPIOx->PUPDR &= ~isolate_vals_mask;
        GPIOx->OTYPER &= ~pin_mask;
    }
    else if (GPIOx == GPIOA){
        isolate_vals_mask = 3u;
        
        GPIOx->MODER &= ~isolate_vals_mask;
        GPIOx->OSPEEDR &= ~isolate_vals_mask;
        GPIOx->PUPDR = (GPIOx->PUPDR & ~isolate_vals_mask) | 2u;
    }
}


/*
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}
*/

/*
GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return -1;
}
*/

/*
void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
}
*/

/*
void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
}
*/
