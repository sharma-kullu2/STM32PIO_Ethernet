/**
  ******************************************************************************
  * @file    LwIP/LwIP_TCP_Echo_Server/Inc/app_ethernet.c 
  * @author  MCD Application Team
  * @brief   Header for app_ethernet.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ETHERNET_H
#define __APP_ETHERNET_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* DHCP process states */
#define DHCP_SOCK                   0
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

/* Exported macro ------------------------------------------------------------*/
#define MAX_SOCK          2
typedef enum _http_sock{
  HTTP_SOCK_1=1,
  HTTP_SOCK_2
} http_sock;

typedef struct _http_config{
  uint8_t txbuf[512];
  uint8_t rxbuf[512];
  uint8_t sock[MAX_SOCK];
} http_config;

void Init_Http_Server();
void Run_Http_Server();

/* Exported functions ------------------------------------------------------- */
#ifdef USE_DHCP
void DHCP_Process();
void DHCP_Periodic_Handle();
void Init_Dhcp(uint8_t* buf);
#endif  

#ifdef __cplusplus
}
#endif

#endif /* __APP_ETHERNET_H */
