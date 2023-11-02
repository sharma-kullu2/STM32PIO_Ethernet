/**
 * @file  wiznet_port.c
 * @brief Wizchip porting layer
 *        Contains wizchip binds to spi api and to pbuf lwip api
 * @note Platform dependent methods from wizchip_conf.c were redeclared here.
 *       Original methods were declared as weak.  
 * @date  8 feb 2020 г.
 * @author  Peter Borisenko
 */

#include <stdint.h>
#include <stdlib.h>
#include <machine/endian.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
//#include "stm32f1xx_hal_exti.h"
#include "wizchip_conf.h"
#include "lwip/pbuf.h"

#include "stm32f1xx_hal_spi.h" //!< @todo replace with your SPI header

#ifdef __cplusplus
extern "C" {
#endif

SPI_HandleTypeDef SpiHandle;
SPI_TypeDef* wiz_spi;
GPIO_TypeDef* wiz_gpio_port;
uint32_t wiz_cs_pin;
GPIO_TypeDef* wiz_rst_gpio_port;
uint32_t wiz_rst_pin;

// Setup SPI1 pins A4/SS, A5/SCK, A6/MISO, A7/MOSI on port A
// Hardware slave select is not used. That is indicated in the settings
static void PeripheralInit_SPI1_Master(SPI_TypeDef* wiz_spi, GPIO_TypeDef* wiz_gpio_port, uint32_t wiz_cs_pin, GPIO_TypeDef* wiz_rst_gpio_port, uint32_t wiz_rst_pin)
{
  GPIO_InitTypeDef GPIO_InitDef;
  // initialize clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
    
  // do not initialize A4/SS because a software SS will be used
 
  // initialize A5/SCK alternate function push-pull (50 MHz)
    GPIO_InitDef.Pin = wiz_cs_pin;
    GPIO_InitDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM; //25MHz
    HAL_GPIO_Init(GPIOA, &GPIO_InitDef);
 
    // initialize A6/MISO input pull-up (50MHz)
    GPIO_InitDef.Pin = GPIO_PIN_6;
    GPIO_InitDef.Mode = GPIO_MODE_INPUT;
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitDef);
 
    // initialize A7/MOSI alternate function push-pull (50 MHz)
    GPIO_InitDef.Pin = GPIO_PIN_7;
    GPIO_InitDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitDef);

    // RST Pin
    GPIO_InitDef.Pin = wiz_rst_pin;
    GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP; //@check
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(wiz_rst_gpio_port, &GPIO_InitDef);

    // initialize SPI master
  SpiHandle.Instance               = wiz_spi;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

  HAL_SPI_Init(&SpiHandle); //add error check

  __HAL_SPI_ENABLE(&SpiHandle);

}

void wiz_lowlevel_setup(void)
{
  /**
   * @note This is example low level init
   * @todo replace with your own init
   */
  //EXTI_InitTypeDef extiInit;
  //NVIC_InitTypeDef NVIC_InitStructure;
  
  wiz_spi= SPI1;

  wiz_cs_pin= GPIO_PIN_5;
  wiz_gpio_port= GPIOA;

  wiz_rst_pin= GPIO_PIN_13;//@Todo Add rst pin
  wiz_rst_gpio_port= GPIOD;

  PeripheralInit_SPI1_Master(wiz_spi,wiz_gpio_port,wiz_cs_pin,wiz_rst_gpio_port,wiz_rst_pin); // Located in spi.h. Replace with your own

  // CS Pin
  /*gpioInit.GPIO_Pin = wiz_cs_pin;
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_Speed = GPIO_Speed_25MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(wiz_cs_gpio_port, &gpioInit);*/

  HAL_GPIO_WritePin(wiz_gpio_port, wiz_cs_pin, GPIO_PIN_SET);

  // RST Pin
  /*gpioInit.GPIO_Pin = wiz_rst_pin;
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(wiz_rst_gpio_port, &gpioInit);*/

  HAL_GPIO_WritePin(wiz_rst_gpio_port, wiz_rst_pin, GPIO_PIN_SET);

  //@Todo
  // INT Pin
  /*gpioInit.GPIO_Pin = GPIO_Pin_6;
  gpioInit.GPIO_Mode = GPIO_Mode_IN;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &gpioInit);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource6);

  extiInit.EXTI_Line = EXTI_Line6;
  extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
  extiInit.EXTI_Trigger = EXTI_Trigger_Falling;
  extiInit.EXTI_LineCmd = SPL_ENABLE;
  EXTI_Init(&extiInit);

  EXTI_ClearITPendingBit(EXTI_Line6);*/

  //NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

  /* Enable Ethernet global Interrupt */
  /*NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = SPL_ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/
}

void wiz_transmit_pbuf(struct pbuf *p)
{
  uint16_t freeSize= getSn_TX_FSR(0);
  uint16_t length = p->tot_len;

  if (freeSize < length) {
    /* TODO: Handle insufficent space in buffer */
  }
  while(1) {
    wiz_send_data(0, (uint8_t*)p->payload, p->len);
    if (p->len == p->tot_len)
      break;
    p = p->next;
  }
  setSn_CR(0, Sn_CR_SEND);
}

int wiz_read_receive_pbuf(struct pbuf **buf)
{
  uint8_t header[6];
  uint16_t length, readlen;
  uint16_t framelen;
  struct pbuf * p;

  if (*buf != NULL)
    return 1;

  uint16_t rxRd= getSn_RX_RD(0);

  length= getSn_RX_RSR(0);
  if (length < 4) {
    /* This could be indicative of a crashed (brown-outed?) controller */
    goto end;
  }

  wiz_recv_data(0, (uint8_t *)&framelen, 2);
  setSn_CR(0, Sn_CR_RECV);
  //__bswap16(framelen); //!< didn't work for me
  framelen= (framelen << 8) | (framelen >> 8);

  /* workaround for https://savannah.nongnu.org/bugs/index.php?50040 */
  if (framelen > 32000) {
    wiz_recv_ignore(0, framelen);
    setSn_CR(0, Sn_CR_RECV);
    goto end;
  }

  framelen-= 2;

  *buf = pbuf_alloc(PBUF_RAW, (framelen), PBUF_RAM);

  if (*buf == NULL) {
    goto end;
  }

  wiz_recv_data(0, (uint8_t*)(*buf)->payload, framelen);
  setSn_CR(0, Sn_CR_RECV);

end:
  return (*buf == NULL) ? 2 : 0;
}

void    wizchip_cris_enter(void)           {

}

void    wizchip_cris_exit(void)          {

}

void  wizchip_cs_select(void)            {
  HAL_GPIO_WritePin(wiz_gpio_port, wiz_cs_pin, GPIO_PIN_RESET);
}

void  wizchip_cs_deselect(void)          {
  HAL_GPIO_WritePin(wiz_gpio_port, wiz_cs_pin, GPIO_PIN_SET);
}

void  wizchip_rst_assert(void)            {
    HAL_GPIO_WritePin(wiz_rst_gpio_port, wiz_rst_pin, GPIO_PIN_RESET);
}

void  wizchip_rst_deassert(void)          {
    HAL_GPIO_WritePin(wiz_rst_gpio_port, wiz_rst_pin, GPIO_PIN_SET);
}


void wiz_hwReset(void) {
    uint32_t t= 168000;
    wizchip_rst_assert();
    while(--t);
    wizchip_rst_deassert();
    t= 168000;
    while(--t);
}

#define Timeout 50000

uint8_t wizchip_spi_readbyte(void)        {
  uint8_t rx;
  HAL_SPI_Receive(&SpiHandle, &rx, 1, Timeout);
  return rx;
}

void  wizchip_spi_writebyte(uint8_t wb) {
  HAL_SPI_Transmit(&SpiHandle, &wb, 1, Timeout);
}

void  wizchip_spi_readburst(uint8_t* pBuf, uint16_t len)  {
 HAL_SPI_Receive(&SpiHandle, pBuf, len, Timeout);
}

void  wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {
  HAL_SPI_Transmit(&SpiHandle, pBuf, len, Timeout);
}

#ifdef __cplusplus
}
#endif