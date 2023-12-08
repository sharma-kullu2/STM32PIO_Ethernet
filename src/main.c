/*******************************************************************************
* @file    Main.c
* @author  GS
* @brief   This file contains the entry point
******************************************************************************/

#include "main.h"
#include "app_ethernet.h"
#include "wizchip_port.h" 

/* Private function prototypes -----------------------------------------------*/
;
static void Error_Handler(void);
static void SystemClock_Config(void);

uint8_t DHCP_Buf[512];
/*Entry Point-----------------------------------------------------------------*/
int main(void) {
  HAL_Init();
  SystemClock_Config();
  BSP_LED_Init(LED2);
  if(BSP_UART_Init()!=HAL_OK){
    Error_Handler();
  }
  BSP_LED_On(LED2);
  HAL_Delay(5*1000); ///Temp
  /* Output a message on Hyperterminal using printf function */
  printf("\n\r********************************************\n\r");
  printf("\n\r*STM32 F103C8 W5500 SPIEthernet Device v0.1*\n\r");
  printf("\n\r********************************************\n\r");

  if (!Init_W5500()){
    DEBUG("Init W5500 Failed\n");
    Error_Handler();
  }
 
#ifdef USE_DHCP
  /*Init DHCP*/
  Init_Dhcp(DHCP_Buf);
#endif
  Init_Http_Server();
  /* Infinite loop */
  while (1)
  {  
    //DEBUG("DEBUG : inside while\n");
    BSP_LED_Toggle(LED2);
  #if 0
    //getVersion();
    BSP_LED_Off(LED2);
    printf("LED OFF \n\r");
    HAL_Delay(500);
    BSP_LED_On(LED2);
    printf("LED ON \n\r");
    HAL_Delay(500);
  #else
    Run_Http_Server();
#ifdef USE_DHCP
    /* handle periodic timers for DHCP */
    DHCP_Periodic_Handle();
#endif 
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
  if (GPIO_Pin == GPIO_PIN_1)
  {
    //@Todo
    DEBUG("Got Interrupt!!!\n");
  }
}


/*Private Function definitions-----------------------------------------------*/

/**
  * @brief  Error_Handler : indicates UART could not start hence turns the LED 2 on
  * @param  void
  * @retval None
  */
static void Error_Handler(void){
  while(1){
    BSP_LED_Toggle(LED2);
    HAL_Delay(100);
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




