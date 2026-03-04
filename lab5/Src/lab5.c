#include "main.h"
#include "stm32f0xx_hal.h"

#define L3GD20_ADDR_7BIT 0x69
#define L3GD20_WHOAMI_REG 0x0F
#define L3GD20_WHOAMI_EXPECTED 0xD3

static void gpio_for_i2c2_and_gyro_init(void);
static void i2c2_init_100kHz(void);
static uint8_t i3g4250d_read_reg(uint8_t reg);
static int i2c2_wait_txis_or_nack(void);
static int i2c2_wait_rxne_or_nack(void);

void SystemClock_Config(void);

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

  gpio_for_i2c2_and_gyro_init();
  i2c2_init_100kHz();

  volatile uint8_t whoami = i3g4250d_read_reg(L3GD20_WHOAMI_REG);

  if (whoami == L3GD20_WHOAMI_EXPECTED)
  {
    
  }

  while (1)
  {
 
  }
  return -1;
}

static void gpio_for_i2c2_and_gyro_init(void)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

  //PB11 SDA, alternate function, open drain, AF1
  GPIOB->MODER &= ~(3u << (11 * 2));
  GPIOB->MODER |= (2u << (11 * 2));
  GPIOB->OTYPER |= (1u << 11);
  GPIOB->PUPDR &= ~(3u << (11 * 2));
  GPIOB->AFR[1] &= ~(0xFu << ((11 - 8) * 4));
  GPIOB->AFR[1] |= (1u << ((11 - 8) * 4));

  //PB13 SCL, alternate function, open drain, AF5
  GPIOB->MODER &= ~(3u << (13 * 2));
  GPIOB->MODER |= (2u << (13 * 2));
  GPIOB->OTYPER |= (1u << 13);
  GPIOB->PUPDR &= ~(3u << (13 * 2));
  GPIOB->AFR[1] &= ~(0xFu << ((13 - 8) * 4));
  GPIOB->AFR[1] |= (5u << ((13 - 8) * 4));

  //PB14 output, push-pull, drive high
  GPIOB->MODER &= ~(3u << (14 * 2));
  GPIOB->MODER |= (1u << (14 * 2));
  GPIOB->OTYPER &= ~(1u << 14);
  GPIOB->ODR |= (1u << 14);

  //PC0 output, push-pull, drive high
  GPIOC->MODER &= ~(3u << (0 * 2));
  GPIOC->MODER |= (1u << (0 * 2));
  GPIOC->OTYPER &= ~(1u << 0);
  GPIOC->ODR |= (1u << 0);

  //PB15 input
  GPIOB->MODER &= ~(3u << (15 * 2));
}

static void i2c2_init_100kHz(void)
{
  // enable clock to I2C2
  RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

  //reset I2C2
  RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;

  // disable peripheral before configuring
  I2C2->CR1 &= ~I2C_CR1_PE;

  // (PRESC = 1, SCLDEL = 4, SDADEL = 2, SCLH = 0x0F or 13, SCLL = 0x13 or 19)
  I2C2->TIMINGR = 0x10320F13; // 100 kHz I2C clock, 8 MHz peripheral clock

  // enable peripheral after configuring
  I2C2->CR1 |= I2C_CR1_PE;
}

static uint8_t i3g4250d_read_reg(uint8_t reg)
{
  while (I2C2->ISR & I2C_ISR_BUSY) {} // wait until I2C2 is not busy

  I2C2->CR2 &= ~((0x3FFu << 0) | (0xFFu << 16) | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
  I2C2->CR2 |= ((uint32_t)L3GD20_ADDR_7BIT << 1) | (1u << 16) | I2C_CR2_START;

  if (i2c2_wait_txis_or_nack() != 0)
  {
    I2C2->ICR = I2C_ICR_NACKCF;
    I2C2->CR2 |= I2C_CR2_STOP;
    return 0xFF;
  }

  I2C2->TXDR = reg;

  while ((I2C2->ISR & I2C_ISR_TC) == 0) {} // wait until transfer complete

  I2C2->CR2 &= ~((0x3FFu << 0) | (0xFFu << 16) | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
  I2C2->CR2 |= ((uint32_t)L3GD20_ADDR_7BIT << 1) | (1u << 16) | I2C_CR2_RD_WRN | I2C_CR2_START;

  if (i2c2_wait_rxne_or_nack() != 0)
  {
    I2C2->ICR = I2C_ICR_NACKCF;
    I2C2->CR2 |= I2C_CR2_STOP;
    return 0xFF;
  }

  uint8_t val = (uint8_t)I2C2->RXDR;

  while ((I2C2->ISR & I2C_ISR_TC) == 0) {} // wait until transfer complete
  I2C2->CR2 |= I2C_CR2_STOP;

  while ((I2C2->ISR & I2C_ISR_STOPF) == 0) {} // wait until stop flag is set
  I2C2->ICR = I2C_ICR_STOPCF; // clear

  return val;
}

static int i2c2_wait_txis_or_nack(void)
{
  while(1)
  {
    uint32_t isr = I2C2->ISR;
    if (isr & I2C_ISR_NACKF) return -1;
    if (isr & I2C_ISR_TXIS) return 0;
  }
}

static int i2c2_wait_rxne_or_nack(void)
{
  while(1)
  {
    uint32_t isr = I2C2->ISR;
    if (isr & I2C_ISR_NACKF) return -1;
    if (isr & I2C_ISR_RXNE) return 0;
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
