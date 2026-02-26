#include "main.h"
#include "stm32f0xx_hal.h"

typedef enum{
  PARSE_WAIT_LED = 0,
  PARSE_WAIT_OP = 1,
} parse_state_t;


static void MX_USART3_Init(void);
//int __io_putchar(int ch);

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void usart3_write_char(char c);
static void usart3_write_string(const char* str);

static int usart3_read_char_nonblocking(char* c);
static const char* led_name_from_letter(char led);
static uint16_t led_pin_from_letter(char led);
static char to_lower_ascii(char c);
static void print_prompt_if_needed(uint8_t* prompt_shown);
static void print_error_and_reset(parse_state_t* state, uint8_t* prompt_shown, const char* msg);
static void execute_and_report_command(char led, char op);


#define RX_BUFFER_SIZE 64u
volatile uint8_t usart3_rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t usart3_rx_head = 0u;
volatile uint8_t usart3_rx_tail = 0u;
volatile uint8_t usart3_rx_overflow = 0u;

static int usart3_read_char_nonblocking(char* c)
{
  if (usart3_rx_head == usart3_rx_tail)
  {
    return 0;
  }
  else
  {
    *c = (char)usart3_rx_buffer[usart3_rx_tail];
    uint8_t next = (uint8_t)(usart3_rx_tail + 1u);
    if (next >= RX_BUFFER_SIZE)
      next = 0u;
    usart3_rx_tail = next;
    return 1;
  }
}

static uint16_t led_pin_from_letter(char led)
{
  switch (led)
  {
    case 'r':
    case 'R':
      return GPIO_PIN_6; // red

    case 'b':
    case 'B':
      return GPIO_PIN_7; // blue

    case 'o':
    case 'O':
      return GPIO_PIN_8; // orange

    case 'g':
    case 'G':
      return GPIO_PIN_9; // green

    default:
      return 0;
  }
}

static const char* led_name_from_letter(char led)
{
  switch (led)
  {
    case 'r':
    case 'R':
      return "red";

    case 'b':
    case 'B':
      return "blue";

    case 'o':
    case 'O':
      return "orange";

    case 'g':
    case 'G':
      return "green";

    default:
      return "?";
  }
}

static char to_lower_ascii(char c)
{
  if (c >= 'A' && c <= 'Z')
    return (char)(c - 'A' + 'a');
  else
    return c;
}

static void print_prompt_if_needed(uint8_t* prompt_shown)
{
  if (*prompt_shown)
    return;
  
  usart3_write_string("CMD? ");
  *prompt_shown = 1u;
}

static void print_error_and_reset(parse_state_t* state, uint8_t* prompt_shown, const char* msg)
{
  usart3_write_string("\r\n");
  usart3_write_string(msg);
  usart3_write_string("\r\n");
  *state = PARSE_WAIT_LED;
  *prompt_shown = 0u;
}

static void execute_and_report_command(char led, char op)
{
  const uint16_t pin = led_pin_from_letter(led);
  const char* led_name = led_name_from_letter(led);
  if (pin == 0u)
  {
    return;
  }

  const char* action = "";
  switch(op){
    case '0':
      HAL_GPIO_WritePin(GPIOC, pin, GPIO_PIN_RESET);
      action = "OFF";
      break;
    case '1':
      HAL_GPIO_WritePin(GPIOC, pin, GPIO_PIN_SET);
      action = "ON";
      break;
    case '2':
      HAL_GPIO_TogglePin(GPIOC, pin);
      action = "TOGGLED";
      break;
    default:
      return;
  }
  usart3_write_string("\r\nCommand Recognized: ");
  usart3_write_char((char)(led - 'a' + 'A')); // uppercase letter
  usart3_write_char(op);
  usart3_write_string(" -> ");
  usart3_write_string(led_name);
  usart3_write_string(" is now ");
  usart3_write_string(action);
  usart3_write_string("\r\n");
}

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
 //usart3_write_string("USART3 ready. Type r/o/g/b to toggle LEDs.\r\n");
 usart3_write_string("USART3 ready. Commands: LED 0/1/2 (0 = off, 1 = on, 2 = toggle).\r\n");
 parse_state_t state = PARSE_WAIT_LED;
 char led = 0;
 uint8_t prompt_shown = 0u;

while (1)
{
  if (usart3_rx_overflow){
    usart3_rx_overflow = 0u;
    usart3_write_string("\r\nError: input buffer overflow. Some characters may have been lost.\r\n");
    state = PARSE_WAIT_LED;
    prompt_shown = 0u;
  }

  if (state == PARSE_WAIT_LED)
    print_prompt_if_needed(&prompt_shown);

  char c;
  if (!usart3_read_char_nonblocking(&c))
    continue;
  
  if (c == '\r' || c == '\n')
  {
    continue;
  }

  c = to_lower_ascii(c);

  if (state == PARSE_WAIT_LED)
  {
    if (led_pin_from_letter(c) == 0u)
    {
      print_error_and_reset(&state, &prompt_shown, "Error: expected LED letter (r/o/g/b)");
      continue;
    }
    else
    {
      led = c;
      state = PARSE_WAIT_OP;
    }
  }
  else {
    if (!(c == '0' || c == '1' || c == '2'))
    {
      print_error_and_reset(&state, &prompt_shown, "Error: expected operation (0 = off, 1 = on, 2 = toggle)");
      continue;
    }
    execute_and_report_command(led, c);
    state = PARSE_WAIT_LED;
    prompt_shown = 0u;
  }
}
//   while ((USART3->ISR & (1u << 5u)) == 0)
//   {
//     // empty loop (blocking wait)
//   }

//   char c = (char)(USART3->RDR & 0xFF);

//   if (c == '\r' || c == '\n')
//     continue;

//   switch (c)
//   {
//     case 'r':
//     case 'R':
//       HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); // red
//       break;

//     case 'b':
//     case 'B':
//       HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); // blue
//       break;

//     case 'o':
//     case 'O':
//       HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); // orange
//       break;

//     case 'g':
//     case 'G':
//       HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); // green
//       break;

//     default:
//       usart3_write_char("Error: invalid key (use r/o/g/b)\r\n");
//       break;
//   }
// }
    
  return 0;
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
  for(int i = 0; str[i] != '\0'; i++)
    usart3_write_char(str[i]);
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
  __HAL_RCC_GPIOC_CLK_ENABLE();

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

  USART3->CR1 |= USART_CR1_RXNEIE; // enable RX interrupt
  HAL_NVIC_SetPriority(USART3_4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_4_IRQn);
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
