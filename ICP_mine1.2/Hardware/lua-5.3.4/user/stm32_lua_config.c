/**
  ******************************************************************************
  * @file   
  * @author  liucongjun
  * @version 
  * @date    
  * @brief ����MicroLibû�еĺ���  
	         lua.c      ---> PC��lua��main����
					 luac.c     ---> lua�Ľ�����main����
					 lint.c     ---> lua��ӵĿ�
					 loslib.c   --> os ���õĿ⣨���ã�
					 liolib.c   --> io�ļ����õĿ�
					 lauxlib.c  ---> 1008�� l_alloc���� �޸�myfree��myrealloc�ڴ���� �����޸������ļ�heap��stack�㹻��
					 Lua print��ʵ����lbaselib.c��luaB_print���� --> �޸���luaB_print������ʹ���ӡ������ʱ�򲻻��һ��'\t'
					 lauxlib.h  ---> �����˴����ض����Ĭ��ֵ,������luaconf.h������޸� lua�����ļ���·��Ҳ��luaconf.h���޸�
					 lua.h -->�����xprintf��֧�֣�������xprintf�滻fprintf���
	--> 2017.11.23
					 luaconf.h -->�޸���lua�ļ���Ĭ��·��,����stdout��stderr�Ѿ��ض��򣬿��Բ��ø���lua��string���
					 bug:
					    1.file����֧��һ���ļ��Ķ�д(Ĭ��handles=0)
							2.����ض�����_sys_write��_sys_read,��Ҫ�����㹻���heap����Ȼ�޷�������д
							  ���������������ض���sys_read��ִ��2�Σ�����fread�����⣬fwriteû������
******************************************************************************
*/

#include <stdio.h>
#include <time.h>
#include <rt_sys.h>
#include "mmcfs.h"
#include "usart.h"

typedef int FILEHANDLE;

#pragma import(__use_no_semihosting_swi)
#pragma import(_main_redirection)
const char __stdin_name[150];
const char __stdout_name[150];
const char __stderr_name[150];

//#if   defined ( __CC_ARM ) /* ARM Compiler */
////#if   defined (__ICCARM__)    /* IAR Compiler */
//#ifdef __GNUC__
///* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
///**
//* @brief  Retargets the C library printf function to the USART.
//* @param  None
//* @retval None
//*/
////   PUTCHAR_PROTOTYPE
////   {
////     /* Place your implementation of fputc here */
////     /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
////     USART_SendData(PRINT_USART, (uint16_t) ch);
////           
////     /* Loop until the end of transmission */
////     while (USART_GetFlagStatus(PRINT_USART, USART_FLAG_TC) == RESET)
//// 		
////     {};

////     return ch;
////   }

//PUTCHAR_PROTOTYPE
//{
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������
//	
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (uint8_t) ch;      
//	return ch;

//}

//#endif

////�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#if 1
////#pragma import(__use_no_semihosting)             
////��׼����Ҫ��֧�ֺ���                 
//struct __FILE 
//{
//	int handle; 
//}; 

//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
// //�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{ 	
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������
//	
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (uint8_t) ch;      
//	return ch;
//}
//
//#endif

int fputc(int ch, FILE *f){
	if(f==stdout || f==stderr){
   //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������	
		while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
		USART6->DR = (uint8_t) ch;      
		return ch;
	}
	else{
		int handles=0;//Ĭ��Ϊ0	
		if(mmc_write(handles, &ch, 1))
		 return ch;
		else
		return EOF;
  }
}

int fgetc(FILE *stream){
  int handles=0;//Ĭ��Ϊ0	
	int ch=0;
  if (mmc_read(handles, &ch, 1))
		return ch;
	else
	return EOF;
}

int feof(FILE * stream){
  int handles=0;//Ĭ��Ϊ0	
  return mmcfs_feof(handles);
}

FILEHANDLE _sys_open(const char *name, int openmode)
{


	return mmc_open(name, openmode);
}

int _sys_close(FILEHANDLE fh)
{
	 return mmc_close(fh);
}

//due to use fputc, not use the function
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
	xprintf("_sys_write=%d ",len);
	return mmc_write(fh, buf, len);
}
//due to use fgetc, not use the function
int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
	xprintf("_sys_read=%d ",len);
	return mmc_read(fh, buf, len);
}
//������Ƿ�Ϊ�ն�
int _sys_istty(FILEHANDLE fh)
{
	return 0;
}
int _sys_seek(FILEHANDLE fh, long pos)
{
	return mmc_lseek(fh, pos, SEEK_SET);
}
//ˢ�¾�������Ļ�����
int _sys_ensure(FILEHANDLE fh)
{
	return 0;
}

//�����ļ���ǰ����
long _sys_flen(FILEHANDLE fh)
{
	return mmc_file_len(fh);
}
void _sys_exit(int status)
{
	//while(1);

}
int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
{
	return 0;
}
//��һ���ַ�д�����̨
void _ttywrch(int ch)
{
	//��д֮����޷���ʼ��
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (uint8_t) ch;      
}
int remove(const char *filename)
{
	return 0;
}
char *_sys_command_string(char *cmd, int len)
{
 return NULL;
}

clock_t clock(void){
  return 0;
}

/* ����lua_os lib�еĲ��ֺ��� */

//time������loslib.c��lstate.c�е���,���ڲ��������
time_t time(time_t * time) {     
  return 0; 
}  

//exit()��system()������loslib.c�е���
void exit(int status) {      

}  

int system(const char * string) {     
  return 0; 
}
