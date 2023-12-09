/**
  ******************************************************************************
  * @file    app_ethernet.c 
  * @author  GS
  * @brief   TBA
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef USE_DHCP
#include "DHCP/dhcp.h"
#endif
#include "httpServer/httpServer.h"
#include "wizchip_port.h"
#include "app_ethernet.h"
#include "http_page.h"
#ifdef USE_LCD
  // #include "lcd_log.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
uint32_t DHCPfineTimer = 0;
#endif

uint8_t ip_assigned;

http_sock sock_list[] = {HTTP_SOCK_1,HTTP_SOCK_2};

http_config config;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * 
*/
void Init_Http_Server(){
  /*Init Flash Based Web Content*/
  reg_httpServer_webContent(name,content);
  /*Init SOCKETS*/
  for(int i=0; i<MAX_SOCK; i++){
    config.sock[i]=sock_list[i];
  }
  httpServer_init(config.txbuf,config.rxbuf,MAX_SOCK,config.sock);
}

/**
 * 
*/
void Run_Http_Server(){
  ENTER();
  if(wiz_link_state()){
    if (ip_assigned)
    {
      httpServer_run(config.sock[0]);
      EXIT();
    }
  }
}
#ifdef USE_DHCP
/*
*
*/
void Init_Dhcp(uint8_t* buf){
  ENTER();
  //can check if link is up or not @Todo
  DHCP_init(DHCP_SOCK, buf);
  ip_assigned=0;
  EXIT();
}

/**
  * @brief  DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void DHCP_Process()
{
  ENTER();
  if(wiz_link_state()){
    uint8_t dhcp_retry = 0;		
    //static 
    uint8_t ret= 0;
    //if (ret==4) return; 
    ret=DHCP_run();
    DEBUG("DHCP_run : %d\n",ret);
    switch(ret)
      {
        case DHCP_IP_ASSIGN:
        case DHCP_IP_CHANGED:
        break;
        case DHCP_IP_LEASED:
          ip_assigned=1;
        break;
        case DHCP_FAILED:
          ip_assigned=0;
          dhcp_retry++;
          if(dhcp_retry > MAX_DHCP_TRIES)
          {
            DEBUG(">> DHCP %d Failed\r\n", dhcp_retry);
            dhcp_retry = 0;
            DHCP_stop();      
            //network_init(); //Assign Static IP 
          }
          break;
        default:
          break;
      }
  }
  else{
    DEBUG("LINK DOWN");
  }
  EXIT();
}

/**
  * @brief  DHCP periodic check
  * @param  localtime the current LocalTime value
  * @retval None
  */
void DHCP_Periodic_Handle()
{  
  /* Fine DHCP periodic process every 500ms */
  if (HAL_GetTick() - DHCPfineTimer >= 500)//DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  HAL_GetTick();
    /* process DHCP state machine */
    DHCP_Process(); 
    PrintNetInfo();
  }
}
#endif
