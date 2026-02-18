#include "tim3_pwm.h"

void TIM3_PWM_800Hz_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable TIM3 clock

  TIM3->CR1 = 0;
  TIM3->CR2 = 0;
  TIM3->PSC = 9; // Prescaler value for 8 MHz / (9 + 1) = 800 kHz timer clock
  TIM3->ARR = 999; // Auto-reload value for 800 Hz
  TIM3->SMCR = 0;
  TIM3->DIER = 0;

  TIM3->CR1 |= TIM_CR1_ARPE; // Enable auto-reload preload

  TIM3->CCMR1 &= ~((3u << 0) | (7u << 4) 
  | (1u << 3) | (3u << 8) | (7u << 12) 
  | (1u << 11) | (1u << 24));

  TIM3->CCMR1 |= (7u << 12) | (1u << 3);
  TIM3->CCMR1 |= (6u << 12) | (1u << 11);

  TIM3->CCER &= ~((1u << 1) | (1u << 5));
  TIM3->CCER |= (1u << 0) | (1u << 4); // Enable output for both channels

  uint32_t ccr = (TIM3->ARR * 20u) / 100u; // 20% duty cycle
  TIM3->CCR1 = ccr; // Set duty cycle for channel 1
  TIM3->CCR2 = ccr; // Set duty cycle for channel 2

  TIM3->CNT = 0;
  TIM3->EGR = TIM_EGR_UG; // Generate an update event to load the prescaler value immediately
  TIM3->SR = 0; // Clear update interrupt flag
}