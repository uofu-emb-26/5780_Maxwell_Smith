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

#define RX_BUFFER_SIZE 64u
extern volatile uint8_t usart3_rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t usart3_rx_head;
extern volatile uint8_t usart3_rx_tail;
extern volatile uint8_t usart3_rx_overflow;

void USART3_4_IRQHandler(void)
{
  const uint32_t isr = USART3->ISR;
  if (isr & USART_ISR_RXNE)
  {
    const uint8_t c = (uint8_t)(USART3->RDR & 0xFFu);

    uint8_t next = (uint8_t)(usart3_rx_head + 1u);
    if (next >= RX_BUFFER_SIZE)
      next = 0u;
    
    if (next != usart3_rx_tail){
      usart3_rx_buffer[usart3_rx_head] = c;
      usart3_rx_head = next;
    }
    else
    {
      usart3_rx_overflow = 1u;
    }
  }

  uint32_t icr = 0u;
  if (isr & USART_ISR_ORE )
    icr |= USART_ICR_ORECF;
  if (isr & USART_ISR_NE )
    icr |= USART_ICR_NCF;
  if (isr & USART_ISR_FE )
    icr |= USART_ICR_FECF;
  if (isr & USART_ISR_PE )
    icr |= USART_ICR_PECF;
  if (icr)
    USART3->ICR = icr;  
}