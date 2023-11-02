/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_BSP_H
#define __BOARD_BSP_H

#include "stm32f1xx_hal.h"

/*Helper Functions------------------------------------------------------------*/
#define NOPULL  1000
#define NOSPEED 1000

void BSP_GPIO_Init(GPIO_TypeDef* Port, uint32_t Pin, uint32_t Mode, uint32_t Pull, uint32_t Speed);

/* UART ----------------------------------------------------------------------*/

/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
/*
* Init UART 1
*/
HAL_StatusTypeDef BSP_UART_Init(void); 

/* LED --------------------------------------------------------------------------*/

/** @defgroup STM32F1XX_BOARD_LED STM32F1XX BOARD LED
  * @{
  */
#define LEDn                             1

#define LED2_PIN                         GPIO_PIN_13
#define LED2_GPIO_PORT                   GPIOC
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()  
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()  

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) LED2_GPIO_CLK_ENABLE();} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  (((__INDEX__) == 0) ? LED2_GPIO_CLK_DISABLE() : 0)
  
/** @defgroup STM32F1XX_BOARD_Exported_Types STM32F1XX BOARD Exported Types
  * @{
  */
typedef enum 
{
  LED2 = 0,
  LED_GREEN = LED2
} Led_TypeDef;

/** @addtogroup STM32F1XX_BOARD_LED_Functions STM32F1XX BOARD LED Functions
  * @{
  */ 

void            BSP_LED_Init(Led_TypeDef Led);
void            BSP_LED_DeInit(Led_TypeDef Led);
void            BSP_LED_On(Led_TypeDef Led);
void            BSP_LED_Off(Led_TypeDef Led);
void            BSP_LED_Toggle(Led_TypeDef Led);

/* SPI----------------------------------------------------------------------------*/

/** @addtogroup STM32F1XX_BOARD_BUS STM32F1XX BOARD BUS
  * @{
  */
/*###################### SPI1 ###################################*/
#ifdef HAL_SPI_MODULE_ENABLED

#define BOARD_SPIx                                 SPI1
#define BOARD_SPIx_CLK_ENABLE()                    __HAL_RCC_SPI1_CLK_ENABLE()

#define BOARD_SPIx_SCK_GPIO_PORT                   GPIOA
#define BOARD_SPIx_SCK_PIN                         GPIO_PIN_5
#define BOARD_SPIx_SCK_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define BOARD_SPIx_SCK_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define BOARD_SPIx_MISO_MOSI_GPIO_PORT             GPIOA
#define BOARD_SPIx_MISO_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define BOARD_SPIx_MISO_MOSI_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define BOARD_SPIx_MISO_PIN                        GPIO_PIN_6
#define BOARD_SPIx_MOSI_PIN                        GPIO_PIN_7
/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define BOARD_SPIx_TIMEOUT_MAX                   1000


void               BSP_SPI_Init(void);
void               BSP_SPI_Write(uint8_t Value);
void               BSP_SPI_WriteData(uint8_t *DataIn, uint16_t DataLength);
void               BSP_SPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);


#endif

#endif