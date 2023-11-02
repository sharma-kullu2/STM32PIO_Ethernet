/*******************************************************************************
* @file    Main.c
* @author  GS
* @brief   This file contains the entry point
******************************************************************************/

#include "main.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "spiif.h"
#include "app_ethernet.h"
#include "tcp_echoserver.h"

/* Private function prototypes -----------------------------------------------*/
struct netif gnetif;
static void Error_Handler(void);
static void SystemClock_Config(void);
static void Netif_Config(void);

/*Entry Point-----------------------------------------------------------------*/
int main(void) {
  HAL_Init();
  SystemClock_Config();
  //LED_Init();
  BSP_LED_Init(LED2);
  if(BSP_UART_Init()!=HAL_OK){
    Error_Handler();
  }
  
  /* Output a message on Hyperterminal using printf function */
  printf("\n\r**********************************\n\r");
  printf("\n\r STM32 F103C8 W5500 SPIEthernet Device\n\r");
  printf("\n\r**********************************\n\r");
  
  /*init lwip stack*/
  lwip_init();
  /* Configure the Network interface */
  Netif_Config();
  
  /* tcp echo server Init */
  tcp_echoserver_init();
  
  /* Notify user about the network interface config */
  User_notification(&gnetif);

  /* Infinite loop */
  while (1)
  {  
    /* Read a received packet from the Ethernet buffers and send it 
       to the lwIP for handling */
    //ethernetif_input(&gnetif);
    BSP_LED_Off(LED2);
    printf("LED OFF \n\r");
    HAL_Delay(500);
    BSP_LED_On(LED2);
    printf("LED ON \n\r");
    HAL_Delay(500);
    spi_if_input(&gnetif);

    /* Handle timeouts */
    sys_check_timeouts();

#ifdef USE_DHCP
    /* handle periodic timers for LwIP */
    //DHCP_Periodic_Handle(&gnetif);
#endif 
  
  }
  return 0;
}

/**
  * @brief  EXTI line detection callbacks
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_14)
  {
    //@Todo
  }
}


/*Private Function definitions-----------------------------------------------*/

/**
  * @brief  Configurates the network interface
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
  
  /* Add the network interface */    
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &spi_if_init, &ethernet_input);
  
  /* Registers the default network interface */
  netif_set_default(&gnetif);
  
  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status */
  //@Todo
  //netif_set_link_callback(&gnetif, ethernetif_update_config);
}

/**
  * @brief  Error_Handler : indicates UART could not start hence turns the LED 2 on
  * @param  void
  * @retval None
  */
static void Error_Handler(void){
  while(1){
    BSP_LED_On(LED2);
  }
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = 16
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Configure PLL ------------------------------------------------------*/
  /* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
  /* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
  /* Enable HSI and activate PLL with HSi_DIV2 as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  oscinitstruct.HSEState        = RCC_HSE_OFF;
  oscinitstruct.LSEState        = RCC_LSE_OFF;
  oscinitstruct.HSIState        = RCC_HSI_ON;
  oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  oscinitstruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI_DIV2;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}




