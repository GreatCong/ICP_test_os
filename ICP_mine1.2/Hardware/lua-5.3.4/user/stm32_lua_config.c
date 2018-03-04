/**
  ******************************************************************************
  * @file   
  * @author  liucongjun
  * @version 
  * @date    
  * @brief 定义MicroLib没有的函数  
	         lua.c      ---> PC中lua的main函数
					 luac.c     ---> lua的解释器main函数
					 lint.c     ---> lua添加的库
					 loslib.c   --> os 调用的库（不用）
					 liolib.c   --> io文件调用的库
					 lauxlib.c  ---> 1008行 l_alloc函数 修改myfree和myrealloc内存管理 否则修改启动文件heap和stack足够大
					 Lua print的实现在lbaselib.c的luaB_print函数 --> 修改了luaB_print函数，使其打印参数的时候不会多一个'\t'
					 lauxlib.h  ---> 包含了串口重定向的默认值,可以在luaconf.h中添加修改 lua查找文件的路径也在luaconf.h中修改
					 lua.h -->添加了xprintf的支持，可以用xprintf替换fprintf输出
	--> 2017.11.23
					 luaconf.h -->修改了lua文件的默认路径,由于stdout和stderr已经重定向，可以不用更改lua的string输出
					 bug:
					    1.file仅仅支持一个文件的读写(默认handles=0)
							2.如果重定向了_sys_write和_sys_read,需要更改足够大的heap，不然无法大量读写
							  若更改了上述的重定向，sys_read会执行2次？？即fread有问题，fwrite没有问题
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
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会
//	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART6->DR = (uint8_t) ch;      
//	return ch;

//}

//#endif

////加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#if 1
////#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{
//	int handle; 
//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
// //重定义fputc函数 
//int fputc(int ch, FILE *f)
//{ 	
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会
//	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART6->DR = (uint8_t) ch;      
//	return ch;
//}
//
//#endif

int fputc(int ch, FILE *f){
	if(f==stdout || f==stderr){
   //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会	
		while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
		USART6->DR = (uint8_t) ch;      
		return ch;
	}
	else{
		int handles=0;//默认为0	
		if(mmc_write(handles, &ch, 1))
		 return ch;
		else
		return EOF;
  }
}

int fgetc(FILE *stream){
  int handles=0;//默认为0	
	int ch=0;
  if (mmc_read(handles, &ch, 1))
		return ch;
	else
	return EOF;
}

int feof(FILE * stream){
  int handles=0;//默认为0	
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
//检查句柄是否为终端
int _sys_istty(FILEHANDLE fh)
{
	return 0;
}
int _sys_seek(FILEHANDLE fh, long pos)
{
	return mmc_lseek(fh, pos, SEEK_SET);
}
//刷新句柄关联的缓冲区
int _sys_ensure(FILEHANDLE fh)
{
	return 0;
}

//返回文件当前长度
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
//将一个字符写入控制台
void _ttywrch(int ch)
{
	//重写之后会无法初始化
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
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

/* 定义lua_os lib中的部分函数 */

//time函数在loslib.c和lstate.c中调用,用于产生随机数
time_t time(time_t * time) {     
  return 0; 
}  

//exit()和system()仅仅在loslib.c中调用
void exit(int status) {      

}  

int system(const char * string) {     
  return 0; 
}
