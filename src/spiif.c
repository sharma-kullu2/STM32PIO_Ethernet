/**
 * @file 	spiif.c
 * @brief 	LwIp+FreeRTOS binding for Wizchip W5500
 * @date 	8 feb 2020 г.
 * @author 	Peter Borisenko
 */

#include <stdbool.h>
#include <string.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#include "spiif.h"
#include "wizchip_conf.h"
#include "wizchip_port.h"
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
//#include <cstring.h>
extern "C" {
#else
//#include <string.h>
#endif

static uint8_t macaddress[6] ; //{ MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };

static const char IFNAME0= 'e';
static const char IFNAME1= '0';


void spi_if_clr(void) {
  setSn_IR(0, 0x1F);
  setSIR(0);
}

/**
 * Checks PHYCFGR for the hardware link state, and calls lwIP's
 * netif_set_link_up() functions as needed.
 */
void spi_if_set_link_state(struct netif *spi_if_netif)
{
  ENTER();
	bool linkstate;
	// Bit 0 is link status (1 = up), 1 is speed (1 = 100), 2 is duplex (1 = full)
	volatile uint8_t phyReg = getPHYCFGR();
	linkstate = phyReg & PHYCFGR_LNK_ON;
	if (linkstate)
	{
    PrintNetInfo();
    if(!netif_is_link_up(spi_if_netif))
      netif_set_link_up(spi_if_netif);
	}
  if(!linkstate){
    LWIP_DEBUGF(NETIF_DEBUG, ("Link Down\n"));
    netif_set_link_down(spi_if_netif);
    DEBUG("LINK DOWN\n");
  }
  EXIT();
}

/**
 * Checks PHYCFGR for the hardware link state, and calls lwIP's
 * netif_set_link_up() and netif_set_link_down() functions as needed.
 */
static void spi_if_check_link_state(struct  netif * spi_if_netif)
{
	static bool linkstate;

	// Bit 0 is link status (1 = up), 1 is speed (1 = 100), 2 is duplex (1 = full)
	volatile uint8_t phyReg = getPHYCFGR();
	linkstate = phyReg & (1 << 0);
	if (linkstate && !netif_is_link_up(spi_if_netif))
	{
		netif_set_link_up(spi_if_netif);
	}
	else if (!linkstate && netif_is_link_up(spi_if_netif))
	{
		netif_set_link_down(spi_if_netif);
	}
}

void spi_if_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p = NULL;
  uint8_t res= 0;
  //struct pbuf *p;
  //spi_if_clr(); //@Todo Check if interrupt is required
  spi_if_check_link_state(netif);
  if (netif_is_link_up(netif))
	{
				
    /* move received packet into a new pbuf */
    res = wiz_read_receive_pbuf(&p);

    if (res !=0){
      DEBUG("SOCK ERROR\n");
      return;
    }
    /* no packet could be read, silently ignore this */
    if (p == NULL) {
      LWIP_DEBUGF(NETIF_DEBUG, ("didn't receive.\n"));
      return;
    }

    /* entry point to the LwIP stack */
    err = netif->input(p, netif);

    if (err != ERR_OK) {
      LWIP_DEBUGF(NETIF_DEBUG, ("received with result %d\n", err));
      DEBUG("received with result %d\n", err);
      pbuf_free(p);
      p = NULL;
    }
  }
}

static err_t spi_if_linkoutput(struct netif *netif, struct pbuf *p) {
  while (!(getSn_SR(0) & SOCK_MACRAW)); /* TODO: Implement wait timeout */
  wiz_transmit_pbuf(p);
  LWIP_DEBUGF(NETIF_DEBUG, ("sent %d bytes.\n", p->tot_len));
  /* TODO: Set up result value */
  return ERR_OK;
}

err_t spi_if_init(struct netif *netif) 
{
  ENTER();
  //int result;
  netif->hwaddr_len= ETHARP_HWADDR_LEN;
  netif->hwaddr[0]=  0x0C;
  netif->hwaddr[1]=  0x4B;
  netif->hwaddr[2]=  0x0A;
  netif->hwaddr[3]=  0x01;
  netif->hwaddr[4]=  0x0E;
  netif->hwaddr[5]=  0x02;
  netif->name[0]= IFNAME0;
  netif->name[1]= IFNAME1;
  netif->output= etharp_output;
  netif->linkoutput= spi_if_linkoutput;
  netif->mtu= 1500;
  netif->flags|= NETIF_FLAG_ETHARP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHERNET;
  //spi_if_netif= netif;
  wiz_lowlevel_setup();
  //wiz_hwReset();
  wizchip_sw_reset();

  uint8_t reg= getVERSIONR();
  if (reg != 0x04) {
      DEBUG("ERROR: getVersionr | got 0x%X\n",reg);
  }
  
  //setMR(MR_RST);
  setSn_MR(0, Sn_MR_MACRAW | Sn_MR_MIP6B | Sn_MR_MMB);
  setSn_RXBUF_SIZE(0, 16);
  setSn_TXBUF_SIZE(0, 16);
  wiz_PhyConf phy_conf;
  phy_conf.by=PHY_CONFBY_SW; //No reset pin is used
  phy_conf.mode= PHY_MODE_AUTONEGO;
  phy_conf.duplex=PHY_DUPLEX_FULL;
  phy_conf.speed=PHY_SPEED_100;
  wizphy_setphyconf(&phy_conf);
  //setINTLEVEL(1);
  //setSIMR(1);
  //setPHYCFGR(0x58);
  //setPHYCFGR(0xD8);
  //setSn_IMR(0, (Sn_IR_RECV));
  setSn_CR(0, Sn_CR_OPEN); //open socket
#ifdef USE_DHCP
    /* handle periodic timers for LwIP */
  setSHAR(netif->hwaddr);
#endif 
  LWIP_DEBUGF(NETIF_DEBUG, ("Driver initialized.\n"));
  EXIT();
  return ERR_OK;
}

/**
  * @brief  This function set a custom MAC address. This function must be called
  *         before ethernetif_init().
  * @param  mac: mac address
  * @retval None
  */
void spi_if_set_mac_addr(const uint8_t *mac)
{
  if (mac != NULL) {
    memcpy(macaddress, mac, 6);
  }
}

/**
  * @brief Returns the current state
  *
  * @param None
  * @return 0 not initialized else 1
  */
uint8_t spi_if_is_init(void)
{
  return 1; //@Todo
}

/**
 * 
*/
void spi_if_set_net_info(struct netif *netif){
  ENTER();
  //uint8_t mac[6];
  setSHAR(netif->hwaddr);
  //uint8_t tmp[4];
  //tmp[0]=(netif->gw) >>24;
  setGAR(&netif->gw);
  setSUBR(&netif->netmask);
  setSIPR(&netif->ip_addr);
  EXIT();
}
/*
*
*/
void spi_if_update_config(struct netif * netif)
{
  ENTER();
  LWIP_DEBUGF(NETIF_DEBUG, ("netif status changed %s\n",ip4addr_ntoa(netif_ip4_addr(netif))));
  EXIT();
}
/**
  * @brief  Returns the current time in milliseconds
  *         when LWIP_TIMERS == 1 and NO_SYS == 1
  * @param  None
  * @retval Current Time value
  */
u32_t sys_now(void)
{
  return HAL_GetTick();
}

#ifdef __cplusplus
}
#endif