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
#include "main.h"
#include "wizchip_port.h"
#include "wizchip_conf.h"


#ifdef __cplusplus
extern "C" {
#endif

//@TODO check pin port mappings
#define WIZ_CS_PIN                   GPIO_PIN_8
#define WIZ_CS_GPIO_PORT             GPIOA
#define WIZ_RST_PIN                  GPIO_PIN_9
#define WIZ_RST_GPIO_PORT            GPIOA

void wiz_lowlevel_setup(void)
{
  BSP_SPI_Init();
  // CS Pin
  /*gpioInit.GPIO_Pin = wiz_cs_pin;
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_Speed = GPIO_Speed_25MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(wiz_cs_gpio_port, &gpioInit);*/
  BSP_GPIO_Init(WIZ_CS_GPIO_PORT,WIZ_CS_PIN,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
  HAL_GPIO_WritePin(WIZ_CS_GPIO_PORT, WIZ_CS_PIN, GPIO_PIN_SET);

  // RST Pin
  /*gpioInit.GPIO_Pin = wiz_rst_pin;
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(wiz_rst_gpio_port, &gpioInit);*/
  BSP_GPIO_Init(WIZ_RST_GPIO_PORT,WIZ_RST_PIN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
  HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_SET);

  //@Todo
  // INT Pin
  /*gpioInit.GPIO_Pin = GPIO_Pin_6;
  gpioInit.GPIO_Mode = GPIO_Mode_IN;
  gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &gpioInit);*/
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
  HAL_GPIO_WritePin(WIZ_CS_GPIO_PORT, WIZ_CS_PIN, GPIO_PIN_RESET);
}

void  wizchip_cs_deselect(void)          {
  HAL_GPIO_WritePin(WIZ_CS_GPIO_PORT, WIZ_CS_PIN, GPIO_PIN_SET);
}

void  wizchip_rst_assert(void)            {
  HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_RESET);
}

void  wizchip_rst_deassert(void)          {
  HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_SET);
}    


void wiz_hwReset(void) {
    uint32_t t= 168000;
    wizchip_rst_assert();
    while(--t);
    wizchip_rst_deassert();
    t= 168000;
    while(--t);
}


uint8_t wizchip_spi_readbyte(void)        {
  uint8_t rx;
  BSP_SPI_WriteReadData(NULL,&rx,1);
  return rx;
}

void  wizchip_spi_writebyte(uint8_t wb) {
  BSP_SPI_WriteReadData(&wb,NULL,1);
}

void  wizchip_spi_readburst(uint8_t* pBuf, uint16_t len)  {
  BSP_SPI_WriteReadData(NULL, pBuf, len);
}

void  wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {
  BSP_SPI_WriteReadData(pBuf, NULL, len);
}

#ifdef __cplusplus
}
#endif