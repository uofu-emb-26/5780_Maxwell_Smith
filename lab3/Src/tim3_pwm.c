#include "tim3_pwm.h"

void TIM3_PWM_800Hz_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable TIM3 clock

  TIM3->CR1 = 0;
  TIM3->CR2 = 0;
  TIM3->SMCR = 0;
  TIM3->DIER = 0;

  TIM3->PSC = 9; // Prescaler value for 8 MHz / (9 + 1) = 800 kHz timer clock
  TIM3->ARR = 999; // Auto-reload value for 800 Hz

  TIM3->CR1 |= TIM_CR1_ARPE; // Enable auto-reload preload

  TIM3->CCMR1 &= ~((3u << 0) | (7u << 4) 
  | (1u << 3) | (3u << 8) | (7u << 12) 
  | (1u << 11) | (1u << 24));

  TIM3->CCMR1 |= (7u << 4) | (1u << 3);
  TIM3->CCMR1 |= (6u << 12) | (1u << 11);

  TIM3->CCER &= ~((1u << 1) | (1u << 5));
  TIM3->CCER |= (1u << 0) | (1u << 4); // Enable output for both channels

  TIM3_PWM_SetDutyPercent(20); // Set initial duty cycle to 20%

  TIM3->CNT = 0;
  TIM3->EGR = TIM_EGR_UG; // Generate an update event to load the prescaler value immediately
  TIM3->SR = 0; // Clear update interrupt flag
}

void TIM3_PWM_Start(void)
{
  TIM3->CR1 |= TIM_CR1_CEN; // Enable TIM3
}

void TIM3_PWM_SetDutyPercent(uint8_t duty)
{
  if (duty > 100) duty = 100; // Cap duty cycle at 100%
  uint32_t period = (uint32_t)TIM3->ARR + 1; // Calculate the period of the PWM signal
  uint32_t ccr = (period * duty) / 100; // Calculate the CCR value for the desired duty cycle
  TIM3->CCR1 = ccr; // Update duty cycle for channel 1
  TIM3->CCR2 = ccr; // Update duty cycle for channel 2
}

void Red_Blue_LED_Init(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock
    GPIOC->MODER &= ~((3u << (2 * 6)) | (3u << (2 * 7))); // Clear mode bits for PC6 and PC7
    GPIOC->MODER |= ((2u << (2 * 6)) | (2u << (2 * 7))); // Set mode bits for PC6 and PC7 to alternate function

    GPIOC->OTYPER &= ~((1u << 6) | (1u << 7)); // Set output type to push-pull
    GPIOC->PUPDR &= ~((3u << (2 * 6)) | (3u << (2 * 7))); // No pull-up, pull-down

    GPIOC->AFR[0] &= ~((0xFu << (4 * 6)) | (0xFu << (4 * 7))); // Clear alternate function bits for PC6 and PC7
}