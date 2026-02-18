#include "tim2_uev.h"


void TIM2_UEV_Init_4Hz(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock

  TIM2->CR1 = 0;
  TIM2->PSC = 7999;
  TIM2->ARR = 249; 
  TIM2->CNT = 0;
  TIM2->EGR = TIM_EGR_UG; // Generate an update event to load the prescaler value immediately
  TIM2->SR = 0; // Clear update interrupt flag

  TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt

  NVIC_SetPriority(TIM2_IRQn, 1); // Set TIM2 interrupt priority
  NVIC_EnableIRQ(TIM2_IRQn); // Enable TIM2 interrupt in NVIC

  TIM2->CR1 |= TIM_CR1_CEN; // Enable TIM2
}