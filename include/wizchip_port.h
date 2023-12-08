/**
 * @file  wiznet_port.h
 * @brief Header for wizchip porting layer 
 * @date  <Today>
 * @author  GS
 */

#ifndef __WIZCHIP_PORT_H_
#define __WIZCHIP_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

void wiz_lowlevel_setup(void);
void wiz_hwReset(void);
uint8_t Init_W5500(void);
uint8_t wiz_link_state();

//TEMP
//===========================================================================//
void getVersion();
void PrintNetInfo();

#ifdef __cplusplus
}
#endif

#endif /* __WIZCHIP_PORT_H_ */