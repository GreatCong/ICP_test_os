/**
 *************************************************************************************
 * @file bluetooth.c
 * @author
 * @version
 * @date
 * @brief
 *************************************************************************************
*/

#include "bluetooth.h"
#include "usart.h"

uint8_t bt_res[10],i;
const uint32_t SPP_BAUD_LIST[14]={0,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800,921600,1382400};

// @ function: 
// @ description:硬件初始化串口
// @ input:
// @ note:
void UsartBound_init(uint32_t baudrate)
{
  huart6.Instance = USART6;
  huart6.Init.BaudRate = baudrate;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

// @ function: SetBluetoothBaud
// @ description:设置蓝牙的波特率
// @ input:
// @ note:
void SetBluetoothBaud(uint32_t baudrate)
{
	uint8_t j,index=1;
	char str[10];
	
	while((SPP_BAUD_LIST[index]!=baudrate)&&(index<14)) index++;
	if(index==14)	return;
	UsartBound_init(baudrate);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart6,(uint8_t*)"AT\r\n",4,10);HAL_Delay(100);
	HAL_UART_Receive(&huart6,bt_res,10,100);
	if(bt_res[0]=='O')	return;

	for(i=1;i<13;i++)
	{
		__HAL_UART_CLEAR_FLAG(&huart6,UART_FLAG_TC);
		__HAL_UART_CLEAR_FLAG(&huart6,UART_FLAG_RXNE);
		UsartBound_init(SPP_BAUD_LIST[i]);
		HAL_Delay(500);
		for(j=0;j<2;j++)
		{
			HAL_UART_Transmit(&huart6,(uint8_t*)"AT\r\n",4,10);
			HAL_Delay(100);
		}
		HAL_UART_Receive(&huart6,bt_res,10,100);
		HAL_Delay(100);
		if(bt_res[0]=='O')
			break;
		else
			HAL_Delay(500);
	}
	for(i=0;i<10;i++)	bt_res[i]=0;
	HAL_UART_Transmit(&huart6,(uint8_t*)"AT+BAUD\r\n",9,10);
	HAL_UART_Receive(&huart6,bt_res,10,100);
	for(i=0;i<10;i++)	bt_res[i]=0;
	HAL_UART_Transmit(&huart6,(uint8_t*)"AT\r\n",4,10);
	HAL_UART_Receive(&huart6,bt_res,2,100);
	sprintf(str,"AT+BAUD%X\r\n",4);
	HAL_UART_Transmit(&huart6,(uint8_t*)str,10,10);
	__HAL_UART_CLEAR_FLAG(&huart6,UART_FLAG_TC);
	__HAL_UART_CLEAR_FLAG(&huart6,UART_FLAG_RXNE);
	UsartBound_init(baudrate);
	HAL_UART_Transmit(&huart6,(uint8_t*)"AT\r\n",index,10);
	for(i=0;i<10;i++)	bt_res[i]=0;
	HAL_UART_Receive(&huart6,bt_res,10,100);
	HAL_Delay(500);
}
