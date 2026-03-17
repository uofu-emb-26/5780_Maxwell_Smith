#include "main.h"
#include "stm32f0xx_hal.h"

void SystemClock_Config(void);

#define LED_RED_PIN GPIO_PIN_6
#define LED_BLUE_PIN GPIO_PIN_7
#define LED_ORANGE_PIN GPIO_PIN_8
#define LED_GREEN_PIN GPIO_PIN_9
#define LED_ALL_PINS (LED_RED_PIN | LED_BLUE_PIN | LED_ORANGE_PIN | LED_GREEN_PIN)
#define ADC_INPUT_PIN GPIO_PIN_0 //PC0

static void GPIO_Init(void);
static void ADC1_Init(void);
static void Set_Led(uint8_t adc_value);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  GPIO_Init();
  ADC1_Init();

  while (1)
  {
    uint8_t adc_value = (uint8_t)(ADC1->DR & 0xFF);
    Set_Led(adc_value);
  }
  return -1;
}

static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_Init = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_Init.Pin = LED_ALL_PINS;
  GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull = GPIO_NOPULL;
  GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_Init);

  HAL_GPIO_WritePin(GPIOC, LED_ALL_PINS, GPIO_PIN_RESET);

  GPIO_Init.Pin = ADC_INPUT_PIN;
  GPIO_Init.Mode = GPIO_MODE_ANALOG;
  GPIO_Init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_Init);
}

static void ADC1_Init(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();

  ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
  ADC1->CFGR2 |= ADC_CFGR2_CKMODE_0;

  ADC1->CFGR1 &= ~(ADC_CFGR1_RES | ADC_CFGR1_EXTEN | ADC_CFGR1_ALIGN | ADC_CFGR1_SCANDIR | ADC_CFGR1_DISCEN | ADC_CFGR1_AUTOFF | ADC_CFGR1_WAIT);
  ADC1->CFGR1 |= ADC_CFGR1_RES_1 | ADC_CFGR1_CONT;

  ADC1->CHSELR = ADC_CHSELR_CHSEL10;

  ADC1->SMPR |= ADC_SMPR_SMP;

  if ((ADC1->CR & ADC_CR_ADEN) != 0)
  {
    ADC1->CR |= ADC_CR_ADDIS;
    while ((ADC1->CR & ADC_CR_ADEN) != 0)
    {
    }
  }

  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) != 0)
  {
  }

  if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)
  {
    ADC1->ISR |= ADC_ISR_ADRDY;
  }

  ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRDY) == 0)
  {
  }

  ADC1->CR |= ADC_CR_ADSTART;
}

static void Set_Led(uint8_t adc_value)
{
  const uint8_t thresh1 = 51;
  const uint8_t thresh2 = 102;
  const uint8_t thresh3 = 153;
  const uint8_t thresh4 = 204;

  HAL_GPIO_WritePin(GPIOC, LED_RED_PIN, (adc_value >= thresh1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, LED_ORANGE_PIN, (adc_value >= thresh2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, LED_GREEN_PIN, (adc_value >= thresh3) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, LED_BLUE_PIN, (adc_value >= thresh4) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add their own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
