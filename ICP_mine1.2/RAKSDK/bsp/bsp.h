#ifndef __BSP_H
#define __BSP_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>


#define WIFI_SPI                             SPI3

#if 0//在标准库中才会有下面的定义
//#define USE_WIFI_POWER_FET

#if defined (USE_WIFI_POWER_FET)//应该是SDK中测试用的，用户没有用到
#define WIFI_FET_GPIO_PORT             			 GPIOB
#define WIFI_FET_PIN                   			 GPIO_Pin_10
#define WIFI_FET_GPIO_CLK              			 RCC_AHB1Periph_GPIOB
#endif

#define WIFI_PWD_GPIO_PORT             			 GPIOC
#define WIFI_PWD_PIN                   			 GPIO_Pin_3
#define WIFI_PWD_GPIO_CLK              			 RCC_AHB1Periph_GPIOC

#define WIFI_INT_GPIO_PORT             			 GPIOB
#define WIFI_INT_PIN                   			 GPIO_Pin_6
#define WIFI_INT_GPIO_EXTI_PORT        			 EXTI_PortSourceGPIOB
#define WIFI_INT_EXTI_PIN_SOURCE       			 EXTI_PinSource6
#define WIFI_INT_EXTI_LINE             			 EXTI_Line6
#define WIFI_INT_EXTI_IRQN             			 EXTI9_5_IRQn

//WIFI SPI接口配置
/* WIFI SPI Interface pins  */  
#define WIFI_SPI                             SPI3
#define WIFI_SPI_CLK                         RCC_APB2Periph_SPI3
#define WIFI_SPI_CLK_INIT                    RCC_APB2PeriphClockCmd

#define WIFI_SPI_SCK_PIN                     GPIO_Pin_3
#define WIFI_SPI_SCK_GPIO_PORT               GPIOB
#define WIFI_SPI_SCK_GPIO_CLK                RCC_AHB1Periph_GPIOB
#define WIFI_SPI_SCK_SOURCE                  GPIO_PinSource3
#define WIFI_SPI_SCK_AF                      GPIO_AF_SPI3

#define WIFI_SPI_MISO_PIN                    GPIO_Pin_4
#define WIFI_SPI_MISO_GPIO_PORT              GPIOB
#define WIFI_SPI_MISO_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define WIFI_SPI_MISO_SOURCE                 GPIO_PinSource4
#define WIFI_SPI_MISO_AF                     GPIO_AF_SPI3

#define WIFI_SPI_MOSI_PIN                    GPIO_Pin_5
#define WIFI_SPI_MOSI_GPIO_PORT              GPIOB
#define WIFI_SPI_MOSI_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define WIFI_SPI_MOSI_SOURCE                 GPIO_PinSource5
#define WIFI_SPI_MOSI_AF                   	 GPIO_AF_SPI3

#define WIFI_CS_PIN                          GPIO_Pin_7
#define WIFI_CS_GPIO_PORT                    GPIOB
#define WIFI_CS_GPIO_CLK                     RCC_AHB1Periph_GPIOB



#define PRINT_USART                          USART6 
#define PRINT_USART_CLK                      RCC_APB2Periph_USART6//UASRT1和USART6均挂在AP2总线上
#define PRINT_USART_INIT                     RCC_APB2PeriphClockCmd

#define PRINT_USART_TX_PIN                   GPIO_Pin_6
#define PRINT_USART_TX_GPIO_PORT             GPIOC
#define PRINT_USART_TX_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define PRINT_USART_TX_SOURCE                GPIO_PinSource6
#define PRINT_USART_TX_AF                    GPIO_AF_USART6

#define PRINT_USART_RX_PIN                   GPIO_Pin_7
#define PRINT_USART_RX_GPIO_PORT             GPIOC
#define PRINT_USART_RX_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define PRINT_USART_RX_SOURCE                GPIO_PinSource7
#define PRINT_USART_RX_AF                    GPIO_AF_USART6

void host_platformInit(void);
void WIFI_GPIO_Init(void);
void WIFI_SPI_Init(void);
void WIFI_INT_Init(void);
void WIFI_SPI_Deinit(void);
void HAL_GPIO_EXTI_Callback(void);

void Print_USART_Init(void);
void delay_ms(int count);
uint32_t HAL_GetTick(void);

#else
//HAL库下的定义 在main.h中
#define WIFI_CS_PIN_Pin WIFI_CS_Pin
#define WIFI_CS_PIN_GPIO_Port WIFI_CS_GPIO_Port
#define WIFI_SPI_SCK_PIN_Pin WIFI_SPI3_SCK_Pin
#define WIFI_SPI_SCK_PIN_GPIO_Port WIFI_SPI3_SCK_GPIO_Port
#define WIFI_SPI_MISO_PIN_Pin WIFI_SPI3_MISO_Pin
#define WIFI_SPI_MISO_PIN_GPIO_Port WIFI_SPI3_MISO_GPIO_Port
#define WIFI_SPI_MOSI_PIN_Pin WIFI_SPI3_MOSI_Pin
#define WIFI_SPI_MOSI_PIN_GPIO_Port WIFI_SPI3_MOSI_GPIO_Port
#define WIFI_INT_PIN_Pin WIFI_INT_Pin
#define WIFI_INT_PIN_GPIO_Port WIFI_INT_GPIO_Port
#define WIFI_PWD_PIN_Pin WIFI_PWD_Pin
#define WIFI_PWD_PIN_GPIO_Port WIFI_PWD_GPIO_Port
#define WIFI_INT_EXTI_IRQN          EXTI9_5_IRQn

void WIFI_GPIO_Init(void);
void WIFI_SPI_Init(void);

#endif

#define   DEBUG 

#ifdef DEBUG
static const char* clean_filename(const char* path)
{
    const char* filename = path + strlen(path); 
    while(filename > path)
    {
        if(*filename == '/' || *filename == '\\')
        {
            return filename + 1;
        }
        filename--;
    }
    return path;
}
#endif



/*reference bsp function*/

#define BSP_WIFIPWD_Up() 	HAL_GPIO_WritePin(WIFI_PWD_PIN_GPIO_Port, WIFI_PWD_PIN_Pin, GPIO_PIN_SET) 

#define BSP_WIFIPWD_Down() 	HAL_GPIO_WritePin(WIFI_PWD_PIN_GPIO_Port, WIFI_PWD_PIN_Pin, GPIO_PIN_RESET)

#define BSP_WIFICS_Up()		HAL_GPIO_WritePin(WIFI_CS_PIN_GPIO_Port, WIFI_CS_PIN_Pin, GPIO_PIN_SET)

#define BSP_WIFICS_Down() 	HAL_GPIO_WritePin(WIFI_CS_PIN_GPIO_Port, WIFI_CS_PIN_Pin, GPIO_PIN_RESET)



void SPIx_Error(void);

/*END reference*/

/* extern osMutexId  buf_mutex; */

extern osMutexId buf_mutexHandle;

static inline void p_lock_mutex(osMutexId mutex)
{ 
    osMutexWait(mutex,osWaitForever);
}


static inline void p_unlock_mutex(osMutexId mutex)
{
    osMutexRelease(mutex);
}

#ifdef DEBUG
//#define DPRINTF(fmt, args...) printf(fmt, ##args)
// #define DPRINTF(fmt, args...) do { \
//                                    p_lock_mutex(buf_mutex); \
//                                    printf("%d  ""%s" ":%u  ", get_stamp(), clean_filename(__FILE__), __LINE__); \
//                                    p_unlock_mutex(buf_mutex); printf(fmt, ##args); \
//                                  } while(0)
// #else
#define DPRINTF(fmt, args...) do { \
    p_lock_mutex(buf_mutexHandle); \
    printf("%d  ""%s" ":%u  ", get_stamp(), clean_filename(__FILE__), __LINE__); \
    printf(fmt, ##args); p_unlock_mutex(buf_mutexHandle);  \
} while(0)
#else

#define DPRINTF(fmt, args...)

#endif

#endif
