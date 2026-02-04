#include "main.h"
#include "stm32f0xx_hal.h"

#include "assert.h"


void SystemClock_Config(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  uint32_t moder_mask;
  uint32_t moder_expect;
  uint32_t otyper_mask;
  uint32_t ospeedr_mask;
  uint32_t pupdr_mask;

  uint32_t old_odr;


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  __HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
  assert((RCC->AHBENR & (1u << 19)) != 0u); // IOPCEN bit-19 in AHBERN (14 offset to AHBERN)

  // Set up a configuration struct to pass to the initialization function
  GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
  HAL_GPIO_Init(GPIOC, &initStr);

  moder_mask = (3u << 16u | 3u << 18u);
  moder_expect = (1u << 16u | 1u << 18u);
  assert((GPIOC->MODER & moder_mask) == moder_expect);

  otyper_mask = ((1u << 8u) | (1u << 9u));
  assert((GPIOC->OTYPER & otyper_mask) == 0u); // 0 for push pull (OUPUT_PP)

  ospeedr_mask = moder_mask; // Same offset from pointer as MODER
  assert((GPIOC->OSPEEDR & ospeedr_mask) == 0u); // 0 for low frequency (SPEED_FREQ_LOW)

  pupdr_mask = moder_mask; // Same offset from pointer as MODER (and OSPEEDR)
  assert((GPIOC->PUPDR & pupdr_mask) == 0u); // 0 for no pull up or pull down
  

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

  assert((GPIOC->ODR & GPIO_PIN_8) != 0u); // Should have been set
  assert((GPIOC->ODR & GPIO_PIN_9) == 0u); // should not have been set

  while (1)
  {
    HAL_Delay(200); // Delay 200ms

    old_odr = GPIOC->ODR;
    // Toggle the output state of both PC8 and PC9
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    assert(((GPIOC->ODR ^ old_odr) & (GPIO_PIN_8 | GPIO_PIN_9)) == (GPIO_PIN_8 | GPIO_PIN_9)); // ensure GPIOC->ODR is toggling as it should
  }
  return -1;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
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
     
  printf("Wrong parameters value: file %s on line %d\r\n", file, line)
}
#endif /* USE_FULL_ASSERT */
