#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t i;
    uint32_t mask;
    uint32_t shift_amt;

    uint32_t pins = GPIO_Init.Pin;

    for (i = 0; i < 16u; i++){
        mask = (1u << i);
        shift_amt = 2u * i;

        if ((pins & mask) == 0u){
            continue;
        }

        GPIOx->MODER &= ~(3u << shift_amt);

        if (GPIO_Init.MODE == GPIO_MODE_OUTPUT_PP || GPIO_Init.MODE == GPIO_MODE_OUTPUT_OD){
            GPIOx->MODER |= (1u << shift_amt);
        }

        if (GPIO_Init.MODE == GPIO_MODE_OUTPUT_OD){
            GPIOx->OTYPER |= mask;
        }
        else {
            GPIOx->OTYPER &= mask;
        }

        GPIOx->OSPEEDR &= ~(3u << shift_amt);
        GPIOx->OSPEEDR |= ((GPIO_Init->Speed & 3u) << shift_amt);
    }

    // uint32_t pin_mask;
    // uint32_t isolate_vals_mask;
    // uint32_t moder_set_mask;

    // if (GPIOx == GPIOC){
    //     pin_mask = (1u << 6u) | (1u << 7u) | (1u << 8u) | (1u << 9u);

    //     isolate_vals_mask = (3u << 12u) | (3u << 14u) | (3u << 16u) | (3u << 18u);

    //     moder_set_mask = (1u << 12u) | (1u << 14u) | (1u << 16u) | (1u << 18u);

    //     GPIOx->MODER = (GPIOx->MODER & ~isolate_vals_mask) | moder_set_mask;
    //     GPIOx->OSPEEDR &= ~isolate_vals_mask;
    //     GPIOx->PUPDR &= ~isolate_vals_mask;
    //     GPIOx->OTYPER &= ~pin_mask;
    // }
    // else if (GPIOx == GPIOA){
    //     isolate_vals_mask = 3u;
        
    //     GPIOx->MODER &= ~isolate_vals_mask;
    //     GPIOx->OSPEEDR &= ~isolate_vals_mask;
    //     GPIOx->PUPDR = (GPIOx->PUPDR & ~isolate_vals_mask) | 2u;
    // }
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


void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (PinState != GPIO_PIN_SET)
    {
        GPIOx->BSRR = ((uint32_t)GPIO_Pin) << 16u;
    }
    else 
    {
        GPIOx->BSRR = (uint32_t)GPIO_Pin;
    }

}



void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    uint32_t pins = (uint32_t)GPIO_Pin;
    uint32_t drive_high_mask = pins & ~(GPIOx->ODR);
    uint32_t drive_low_mask = pins & (GPIOx->ODR);
    GPIOx->BSRR = drive_high_mask | (drive_low_mask << 16u);
}

