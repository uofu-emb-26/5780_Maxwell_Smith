#include "main.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
   while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF) // Check if update interrupt flag is set
  {
    TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt flag

    static uint8_t led_state = 0; // 0 for green on, 1 for orange on

    if (led_state == 0)
    {
      GPIOC->BSRR = (1u << 8) | (1u << (9 + 16)); // Set PC8 high and PC9 low
      led_state = 1;
    }
    else
    {
      GPIOC->BSRR = (1u << 9) | (1u << (8 + 16)); // Set PC9 high and PC8 low
      led_state = 0;
    }
  }
}