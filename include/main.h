/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "board_bsp.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define USE_LCD       /* enable LCD  */
#define USE_DHCP       /* enable DHCP, if disabled static address is used */

#define DEST_IP_ADDR0   ((uint8_t)192)
#define DEST_IP_ADDR1   ((uint8_t)168)
#define DEST_IP_ADDR2   ((uint8_t)0)
#define DEST_IP_ADDR3   ((uint8_t)1)

#define DEST_PORT       ((uint16_t)7)
 
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   ((uint8_t)192)
#define IP_ADDR1   ((uint8_t)168)
#define IP_ADDR2   ((uint8_t)0)
#define IP_ADDR3   ((uint8_t)10)

/*NETMASK*/
#define NETMASK_ADDR0   ((uint8_t)255)
#define NETMASK_ADDR1   ((uint8_t)255)
#define NETMASK_ADDR2   ((uint8_t)255)
#define NETMASK_ADDR3   ((uint8_t)0)

/*Gateway Address*/
#define GW_ADDR0   ((uint8_t)192)
#define GW_ADDR1   ((uint8_t)168)
#define GW_ADDR2   ((uint8_t)0)
#define GW_ADDR3   ((uint8_t)1)

/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */
