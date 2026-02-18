#ifndef TIM3_PWM_H
#define TIM3_PWM_H

#include "stm32f0xx.h"

void TIM3_PWM_800Hz_Init(void);
void TIM3_PWM_Start(void);
void TIM3_PWM_SetDutyPercent(uint8_t duty);
void Red_Blue_LED_Init(void);

#endif /* TIM3_PWM_H */