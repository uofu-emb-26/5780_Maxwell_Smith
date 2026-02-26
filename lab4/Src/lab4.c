#include "main.h"
#include "stm32f0xx_hal.h"

static void MX_USART3_Init(void);
int __io_putchar(int ch);

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void usart3_write_char(char c);
static void usart3_write_string(const char* str);


static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

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

  MX_GPIO_Init();
  MX_USART3_Init();

 //HAL_USART_Transmit(&USART3, "USART3 ready. Type r/o/g/b to toggle LEDs.\r\n", strlen("USART3 ready. Type r/o/g/b to toggle LEDs.\r\n"), 100);
 usart3_write_string("USART3 ready. Type r/o/g/b to toggle LEDs.\r\n");

while (1)
{
  while ((USART3->ISR & (1u << 5u)) == 0)
  {
    // empty loop (blocking wait)
  }

  char c = (char)(USART3->RDR & 0xFF);

  if (c == '\r' || c == '\n')
    continue;

  switch (c)
  {
    case 'r':
    case 'R':
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // red
      break;

    case 'b':
    case 'B':
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // blue
      break;

    case 'o':
    case 'O':
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); // orange
      break;

    case 'g':
    case 'G':
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); // green
      break;

    default:
      usart3_write_char("Error: invalid key (use r/o/g/b)\r\n");
      break;
  }
}
    
  return -1;
}

static void usart3_write_char(char c)
{
  while ((USART3->ISR & USART_ISR_TXE) == 0)
  {
    // empty loop (blocking wait)
  }
  USART3->TDR = (uint8_t)c;
}

static void usart3_write_string(const char* str)
{
  while (*str)
  {
    usart3_write_char(*str++);
  }
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

static void MX_USART3_Init(void)
{
  __HAL_RCC_USART3_CLK_ENABLE();
  //__HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // GPIOC->MODER |= (2 < 2*4);
  // GPIOC->AFR[0] |= (1 << 4*4);

  // GPIOC->MODER |= (2 < 2*5);
  // GPIOC->AFR[0] |= (1 << 4*5);

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // USART3->CR3 = 0; // no flow control

  //USART3->CR1 &= ~USART_CR1_UE;
  USART3->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_OVER8);
  USART3->CR2 &= ~(USART_CR2_STOP | USART_CR2_CLKEN);

  USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;
  USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE);
  USART3->CR1 |= USART_CR1_UE;

  while ((USART3->ISR & (USART_ISR_TEACK | USART_ISR_REACK)) != (USART_ISR_TEACK | USART_ISR_REACK))
  {
    // empty loop (waiting for TE and RE to be acknowledged by hardware)
  }
}

//int __io_putchar(int ch)
//{
  //uint8_t c = (uint8_t)ch;
  //HAL_USART_Transmit(&USART3, &c, 1, HAL_MAX_DELAY);
  //return ch;
//}

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
