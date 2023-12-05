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


#define WIZ_CS_PIN                                GPIO_PIN_4
#define WIZ_CS_GPIO_PORT                          GPIOA
#define WIZ_CS_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define WIZ_RST_PIN                               GPIO_PIN_1
#define WIZ_RST_GPIO_PORT                         GPIOB
#define WIZ_RST_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define WIZ_INT_PIN                               GPIO_PIN_1 
#define WIZ_INT_GPIO_PORT                         GPIOA

/**
  * @brief  SD Control Lines management
  */  
#define WIZ_CS_LOW()       HAL_GPIO_WritePin(WIZ_CS_GPIO_PORT, WIZ_CS_PIN, GPIO_PIN_RESET);//GPIO_Off(SD_CS_GPIO_PORT, SD_CS_PIN)
#define WIZ_CS_HIGH()      HAL_GPIO_WritePin(WIZ_CS_GPIO_PORT, WIZ_CS_PIN, GPIO_PIN_SET);//GPIO_On(SD_CS_GPIO_PORT, SD_CS_PIN)



uint8_t wizchip_spi_readbyte(void)        {
  //ENTER();
  uint8_t rx=0;
  BSP_SPI_ReadData( &rx, 1);
  DEBUG("readbyte : 0x%X\n",rx);
  //EXIT();
  return rx;
}

void  wizchip_spi_writebyte(uint8_t wb) {
  //ENTER();
  //DEBUG("writebyte : 0x%X\n",wb);
  //BSP_SPI_Write((uint8_t)wb);
  BSP_SPI_WriteData(&wb, 1);
  //EXIT();
}

void  wizchip_spi_readburst(uint8_t* pBuf, uint16_t len)  {
  //ENTER();
  BSP_SPI_ReadData(pBuf, len);
  //EXIT();
}

void  wizchip_spi_writeburst(uint8_t* pBuf, uint16_t len) {
  //ENTER();
  BSP_SPI_WriteData(pBuf, len);
  //EXIT();
}



void wiz_lowlevel_setup(void)
{
  ENTER();
  /* SD_CS_GPIO Periph clock enable */
  GPIO_InitTypeDef  gpioinitstruct = {0};
  WIZ_CS_GPIO_CLK_ENABLE();

  /* Configure SD_CS_PIN pin: SD Card CS pin */
  gpioinitstruct.Pin    = WIZ_CS_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIZ_CS_GPIO_PORT, &gpioinitstruct);

  BSP_SPI_Init();

  // RST Pin
  //WIZ_RST_GPIO_CLK_ENABLE(); 
  //BSP_GPIO_Init(WIZ_RST_GPIO_PORT,WIZ_RST_PIN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);
  //HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_SET);

  /* EXTI interrupt init*/
  /*BSP_GPIO_Init(WIZ_INT_GPIO_PORT,WIZ_INT_PIN,GPIO_MODE_IT_RISING_FALLING,GPIO_NOPULL,NOSPEED);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);*/
  //
  
  reg_wizchip_spi_cbfunc(&wizchip_spi_readbyte,&wizchip_spi_writebyte);
  reg_wizchip_spiburst_cbfunc(&wizchip_spi_readburst,&wizchip_spi_writeburst);
  EXIT();
}

void wiz_transmit_pbuf(struct pbuf *p)
{
  ENTER();
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
  EXIT();
}

int wiz_read_receive_pbuf(struct pbuf **buf)
{
  ENTER();
  //uint8_t header[6];
  uint16_t readlen;
  //uint16_t framelen;
  //struct pbuf * p;

  if (*buf != NULL)
    return 1;

  //read length of RX buffer
  readlen = getSn_RX_RSR(0);
  if(readlen == 0) 
    return 1;

  //uint16_t rxRd= 
  //getSn_RX_RD(0);
  if (readlen < 4) {
    /* This could be indicative of a crashed (brown-outed?) controller */
    goto end;
  }
  /* workaround for https://savannah.nongnu.org/bugs/index.php?50040 */
  if (readlen > 32000) {
    wiz_recv_ignore(0, readlen);
    setSn_CR(0, Sn_CR_RECV);
    goto end;
  }

  *buf = pbuf_alloc(PBUF_RAW, (readlen), PBUF_RAM);

  if (*buf == NULL) {
    goto end;
  }

  wiz_recv_data(0, (uint8_t*)(*buf)->payload, readlen);
  setSn_CR(0, Sn_CR_RECV);

  
  EXIT();
end:
  return (*buf == NULL) ? 2 : 0;
}

void    wizchip_cris_enter(void)           {
//ENTER();
//EXIT();
}

void    wizchip_cris_exit(void)          {
//ENTER();
//EXIT();
}

void  wizchip_cs_select(void)            {
  //ENTER();
  WIZ_CS_LOW(); 
  //EXIT();
}

void  wizchip_cs_deselect(void)          {
  //ENTER();
  WIZ_CS_HIGH();
  //EXIT();
}

void  wizchip_rst_assert(void)            {
  ENTER();
  HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_RESET);
  EXIT();
}

void  wizchip_rst_deassert(void)          {
  ENTER();
  HAL_GPIO_WritePin(WIZ_RST_GPIO_PORT, WIZ_RST_PIN, GPIO_PIN_SET);
  EXIT();
}    


void wiz_hwReset(void) {
    ENTER();
    uint32_t t= 168000;
    wizchip_rst_assert();
    while(--t);
    wizchip_rst_deassert();
    t= 168000;
    while(--t);
    EXIT();
}

//TEMP
//===========================================================================//
void w5500_ini(void){
  ENTER();
  uint8_t temp;
  uint8_t W5500FifoSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2, }, {2, 2, 2, 2, 2, 2, 2, 2}};

  //wizchip_cs_deselect(); //Chip OFF
  
  wiz_lowlevel_setup();
  uint8_t reg= getVERSIONR();
  DEBUG("VERSION INFO: 0x%X\n",reg);
  if (reg != 0x04) {
      DEBUG("ERROR: getVersionr | got 0x%X\n",reg);
  }
  EXIT();
}

void getVersion()
{
  uint8_t reg= getVERSIONR();
  if (reg != 0x04) {
      DEBUG("ERROR: getVersionr | got 0x%X\n",reg);
  }
}


void w5500_ini_2(void){
  ENTER();
  uint16_t count=0;
  //init SPI and CS
  wiz_lowlevel_setup();

  //WIZ_CS_HIGH();

  //wizchip_sw_reset();
   
  setMR(MR_RST);
  do {
		uint8_t mr = getMR();
    printf("1. MR= 0x%X\n",mr);
		if (mr == 0) break;
		HAL_Delay(10);
	} while (++count < 20);
  setMR(0x08);
	printf("2. MR= 0x%X\n",getMR());
	setMR(0x10);
	printf("3. MR= 0x%X\n",getMR());
	setMR(0x00);
	printf("4. MR= 0x%X\n",getMR());

  uint8_t reg= getVERSIONR();
  DEBUG("VERSION INFO: 0x%X\n",reg);
  if (reg != 0x04) {
      DEBUG("ERROR: getVersionr | got 0x%X\n",reg);
  }

  EXIT();
}

void PrintNetInfo(){
  ENTER();
  wiz_NetInfo netinfo;
  wizchip_getnetinfo(&netinfo);
  DEBUG("IP ADDR : %d.%d.%d.%d\n",netinfo.ip[0],netinfo.ip[1],netinfo.ip[2],netinfo.ip[3]);
  DEBUG("NETMASK : %d.%d.%d.%d\n",netinfo.sn[0],netinfo.sn[1],netinfo.sn[2],netinfo.sn[3]);
  DEBUG("GATEWAY : %d.%d.%d.%d\n",netinfo.gw[0],netinfo.gw[1],netinfo.gw[2],netinfo.gw[3]);
  EXIT();
}
///


#ifdef __cplusplus
}
#endif