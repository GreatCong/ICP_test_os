#include "ad7606.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stm32f4xx.h"
#include "spi.h"

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

#define OS2(State)	HAL_GPIO_WritePin(AD_OS2_GPIO_Port,AD_OS2_Pin,(GPIO_PinState)State)
#define OS1(State)	HAL_GPIO_WritePin(AD_OS1_GPIO_Port,AD_OS1_Pin,(GPIO_PinState)State)
#define OS0(State)	HAL_GPIO_WritePin(AD_OS0_GPIO_Port,AD_OS0_Pin,(GPIO_PinState)State)
#define SPI_CS(State)	HAL_GPIO_WritePin(AD_CS_GPIO_Port,AD_CS_Pin,(GPIO_PinState)State)

union _AD7606_BUF AD7606_BUF;

void AD7606_Init(void)
{
	AD7606_SetOsRate(AD_OS_NO);
	
	#ifdef AD7606_SOFT_SPI
	AD7606_GPIO_SPI();
	#endif
}

//  @ fuction:  
//  @ description:  过采样位解码
//  @ input:
//  @ output:
//  @ note: 
void AD7606_SetOsRate(AD7606_OS_Rate rate)
{
	switch(rate)
	{
		case AD_OS_NO:{OS2(0);OS1(0);OS0(0);break;}
		case AD_OS_X2:{OS2(0);OS1(0);OS0(1);break;}
		case AD_OS_X4:{OS2(0);OS1(1);OS0(0);break;}
		case AD_OS_X8:{OS2(0);OS1(1);OS0(1);break;}
		case AD_OS_X16:{OS2(1);OS1(0);OS0(0);break;}
		case AD_OS_X32:{OS2(1);OS1(0);OS0(1);break;}
		case AD_OS_X64:{OS2(1);OS1(1);OS0(0);break;}
		default:{OS2(0);OS1(0);OS0(0);break;}//111 invalid
	}
}

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 读取AD7606，兼容硬件和软件SPI
#ifdef AD7606_SOFT_SPI

#define SPI_SCK(State)	HAL_GPIO_WritePin(AD_SPI1_SCK_GPIO_Port,AD_SPI1_SCK_Pin,(GPIO_PinState)State)
#define SPI_MISO()	HAL_GPIO_ReadPin(AD_SPI1_MISO_GPIO_Port,(GPIO_PinState)AD_SPI1_MISO_Pin)
void AD7606_Read4CH(void)
{
	uint8_t i,j;
	uint8_t value;
	for(i=0;i<8;i++)
	{
		SPI_CS(0);//chip select
		for(j=0;j<8;j++)
		{
			value <<=1;
			if((AD_SPI1_MISO_GPIO_Port->IDR & AD_SPI1_MISO_Pin) != 0)
				value |= 1;
			AD_SPI1_SCK_GPIO_Port->BSRR = AD_SPI1_SCK_Pin;//set high
			SPI_SCK(0);//set low
		}
		SPI_CS(1);//chip deselect
		AD7606_BUF.bytebuf[i] = value;
	}
}
#else
void AD7606_Read4CH(void){
//  uint8_t 						dummy;
//  uint8_t 						recv;
//	uint8_t 						i;
	HAL_StatusTypeDef status = HAL_OK;
	
	SPI_CS(0);//chip select
//	for(i=0;i<8;i++){
//	  SPI_CS(0);//chip select
//		while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX){};//等待发送缓冲区空
//		
//	  status = HAL_SPI_TransmitReceive(&hspi1,&dummy,&recv,1,5000);
//	  assert_param(status == HAL_OK);		
//		
//		SPI_CS(1);//chip deselect
//		AD7606_BUF.bytebuf[i] = recv;
//	}
	
	status = HAL_SPI_Receive_DMA(&hspi1,AD7606_BUF.bytebuf,8);//HAL_SPI_Receive自带向从机发送空字节
  assert_param(status == HAL_OK);	//硬件SPI没有调通？？注意OS情况下，DMA如果被OS管理了，会有问题
	SPI_CS(1);//chip deselect
	
}
#endif

//  @ fuction:  
//  @ description:  AD_CONVEST_PWM
//  @ input:
//  @ output:
//  @ note: TIM2 channal 4 --> output: f_khz Hz
void AD_CONVEST_PWM_Init(uint8_t f_khz){
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 70-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1200/f_khz-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 600/f_khz;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim2);
}

#ifdef AD7606_SOFT_SPI

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 软件模拟SPI
void AD7606_GPIO_SPI(void){
  GPIO_InitTypeDef GPIO_InitStruct;
	
	  /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */

  HAL_SPI_MspDeInit(&hspi1);//关闭硬件SPI
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AD_SPI1_SCK_Pin|AD_SPI1_MOSI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = AD_SPI1_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD_SPI1_SCK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = AD_SPI1_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AD_SPI1_MISO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = AD_SPI1_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD_SPI1_MOSI_GPIO_Port, &GPIO_InitStruct);

}
#endif

//  @ fuction:  
//  @ description:  AD7606_EXTI_Handle
//  @ input:
//  @ output:
//  @ note: 在rw_lib_platform.c中，wifi模块的中断中调用
void AD7606_handle(void){
	uint8_t i = 0;
	
  AD7606_Read4CH();
	
	for(i=0;i<4;i++) 
	{
		printf("AD7606 = 0x%x,i=%d\r\n",AD7606_BUF.shortbuf[0+i],i);//会造成阻塞，Wifi无法初始化
		
	}
	//i = AD7606_BUF.bytebuf[0];
}
