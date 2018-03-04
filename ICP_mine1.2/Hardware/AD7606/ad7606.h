#ifndef __AD7606_DRIVER_H__
#define __AD7606_DRIVER_H__

#include "stm32f4xx_hal.h"

//#define AD7606_SOFT_SPI //Èí¼þÄ£ÄâSPI

/*
PC5 -O- OS 0							
PB0 -O- OS 1
PB1 -O- OS 2
RANGE		0:+/-5V 
PA3 -O- CONVST A&B					rising edge:start to convert
PC4 -O- _CS
PA4 -I- BUSY
PA2 -O- ICP_EN
*/

typedef enum
{
	AD_OS_NO = 0u,
	AD_OS_X2 = 1u,
	AD_OS_X4 = 2u,
	AD_OS_X8 = 3u,
	AD_OS_X16 = 4u,
	AD_OS_X32 = 5u,
	AD_OS_X64 = 6u
}AD7606_OS_Rate;

union _AD7606_BUF
{
	uint16_t shortbuf[4];
	uint8_t bytebuf[8];
};

extern union _AD7606_BUF AD7606_BUF;

void AD7606_Init(void);
void AD7606_SetOsRate(AD7606_OS_Rate rate);
void AD7606_Read4CH(void);
void AD_CONVEST_PWM_Init(uint8_t f_khz);
void AD7606_handle(void);

#ifdef AD7606_SOFT_SPI
void AD7606_GPIO_SPI(void);
#endif

#endif

