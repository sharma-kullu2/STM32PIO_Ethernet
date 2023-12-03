/**
 * @file  wiznet_port.h
 * @brief Header for wizchip porting layer 
 * @date  8 feb 2020 г.
 * @author  Peter Borisenko
 */

#ifndef __WIZCHIP_PORT_H_
#define __WIZCHIP_PORT_H_

#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

void wiz_lowlevel_setup(void);
void wiz_transmit_pbuf(struct pbuf *buf);
int wiz_read_receive_pbuf(struct pbuf **buf);
void wiz_hwReset(void);

//TEMP
//TEMP
//===========================================================================//
void w5500_ini(void);
void getVersion();

#ifdef __cplusplus
}
#endif

#endif /* __WIZCHIP_PORT_H_ */