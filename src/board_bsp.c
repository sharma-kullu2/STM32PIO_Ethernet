#include "board_bsp.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/*
* Init LED 
*/

/** @defgroup STM32F1XX_BOARD_Private_Variables STM32F1XX BOARD Private Variables
  * @{
  */ 
GPIO_TypeDef* LED_PORT[LEDn] = {LED2_GPIO_PORT};

const uint16_t LED_PIN[LEDn] = {LED2_PIN};

/**
  * @brief  Helper Fn to enable GPIO | Enable Clock before enabling GPIO Port
  * @param  Port: GPIO Port. 
  *          This parameter can be one of the following values:
  *     @arg GPIOA
  * @param  Pin: GPIO Pin. 
  *          This parameter can be one of the following values:
  *     @arg LED2
  * @param  Mode: Push PUll/ Open Drain etc 
  * @param  Pull: Pull no/up/down mode.
  * @param  Speed: Low,Medium,High Frequency.       
  */
void BSP_GPIO_Init(GPIO_TypeDef* Port, uint32_t Pin, uint32_t Mode, uint32_t Pull, uint32_t Speed)
{
  GPIO_InitTypeDef  gpioinitstruct;
  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin    = Pin;
  gpioinitstruct.Mode   = Mode;
  if(Pull != NOPULL)
    gpioinitstruct.Pull   = Pull;
  if(Speed != NOSPEED)
    gpioinitstruct.Speed  = Speed;
  
  HAL_GPIO_Init(Port, &gpioinitstruct);
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Led to be configured. 
  *          This parameter can be one of the following values:
  *     @arg LED2
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  //GPIO_InitTypeDef  gpioinitstruct;
  
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_LED pin */
  BSP_GPIO_Init(LED_PORT[Led],LED_PIN[Led],GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
  /*gpioinitstruct.Pin    = LED_PIN[Led];
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(LED_PORT[Led], &gpioinitstruct);*/

  /* Reset PIN to switch off the LED */
  HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led], GPIO_PIN_RESET);
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be de-init. 
  *   This parameter can be one of the following values:
  *     @arg  LED2
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led],LED_PIN[Led], GPIO_PIN_RESET);
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_On(Led_TypeDef Led)
{
  //@Todo assert if led is valid before proceeding
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  //@Todo assert if led is valid before proceeding
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *            @arg  LED2
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/*
* Init UART
*/

/* UART handler declaration */
static UART_HandleTypeDef UartHandle;

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;


  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

}

HAL_StatusTypeDef BSP_UART_Init(){
  HAL_StatusTypeDef Status;
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  Status=HAL_UART_Init(&UartHandle);
  return Status;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/*
* Init SPI
*/

#ifdef HAL_SPI_MODULE_ENABLED
static uint32_t SpixTimeout = BOARD_SPIx_TIMEOUT_MAX;        /*<! Value of Timeout when SPI communication fails */

static SPI_HandleTypeDef hf103_Spi;
static void SPIx_Error (void);
/**
  * @brief  Initialize SPI MSP.
  */
static void SPIx_MspInit(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  BOARD_SPIx_SCK_GPIO_CLK_ENABLE();
  BOARD_SPIx_MISO_MOSI_GPIO_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /* Configure SPI SCK */
  gpioinitstruct.Pin        = BOARD_SPIx_SCK_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BOARD_SPIx_SCK_GPIO_PORT, &gpioinitstruct);

  /* configure SPI MISO and MOSI */
  gpioinitstruct.Pin        = (BOARD_SPIx_MISO_PIN | BOARD_SPIx_MOSI_PIN);
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull       = GPIO_NOPULL;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BOARD_SPIx_MISO_MOSI_GPIO_PORT, &gpioinitstruct);
  /*** Configure the SPI peripheral ***/ 
  /* Enable SPI clock */
  BOARD_SPIx_CLK_ENABLE();
}

/**
  * @brief  Initialize SPI HAL.
  */
void BSP_SPI_Init(void)
{
  /* SPI Config */
  hf103_Spi.Instance = BOARD_SPIx;
  HAL_SPI_DeInit(&hf103_Spi);
  /* SPI baudrate is set to 8 MHz maximum (PCLK2/SPI_BaudRatePrescaler = 64/8 = 8 MHz) 
       to verify these constraints:
          - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
            Since the provided driver doesn't use read capability from LCD, only constraint 
            on write baudrate is considered.
          - SD card SPI interface max baudrate is 25MHz for write/read
          - PCLK2 max frequency is 32 MHz 
  */
  hf103_Spi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_4; 
  hf103_Spi.Init.Direction          = SPI_DIRECTION_2LINES;
  hf103_Spi.Init.CLKPhase           = SPI_PHASE_2EDGE;
  hf103_Spi.Init.CLKPolarity        = SPI_POLARITY_HIGH;
  hf103_Spi.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
  hf103_Spi.Init.CRCPolynomial      = 7;
  hf103_Spi.Init.DataSize           = SPI_DATASIZE_8BIT;
  hf103_Spi.Init.FirstBit           = SPI_FIRSTBIT_MSB;
  hf103_Spi.Init.NSS                = SPI_NSS_SOFT;
  hf103_Spi.Init.TIMode             = SPI_TIMODE_DISABLE;
  hf103_Spi.Init.Mode               = SPI_MODE_MASTER;
  SPIx_MspInit();
  if (HAL_SPI_Init(&hf103_Spi) != HAL_OK){
    /* Should not occur */
    while(1) {};
  }
}

/**
  * @brief SPI Read 4 bytes from device
  * @retval Read data
*/
uint32_t BSP_SPI_Read(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t          readvalue = 0;
  uint32_t          writevalue = 0xFFFFFFFF;
  
  status = HAL_SPI_TransmitReceive(&hf103_Spi, (uint8_t*) &writevalue, (uint8_t*) &readvalue, 1, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }

  return readvalue;
}

/**
  * @brief  SPI Write a byte to device
  * @param  Value: value to be written
*/
void BSP_SPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_SPI_TransmitReceive(&hf103_Spi, (uint8_t*) DataIn, DataOut, DataLength, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write an amount of data to device
  * @param  Value: value to be written
  * @param  DataLength: number of bytes to write
  */
void BSP_SPI_WriteData(const uint8_t *DataIn, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit(&hf103_Spi, DataIn, DataLength, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Read Data from device
  * @param  Data: value to be read
  * @param  DataLength: length of data
  */
void BSP_SPI_ReadData(const uint8_t *Data, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Receive(&hf103_Spi, (uint8_t*) Data, DataLength, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write a byte to device
  * @param  Value: value to be written
  */
void BSP_SPI_Write(uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t data;

  status = HAL_SPI_TransmitReceive(&hf103_Spi, (uint8_t*) &Value, &data, 1, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI error treatment function
  */
static void SPIx_Error (void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&hf103_Spi);

  /* Re-Initiaize the SPI communication BUS */
  BSP_SPI_Init();
}


#endif /* HAL_SPI_MODULE_ENABLED */
