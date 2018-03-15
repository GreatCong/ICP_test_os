#include "my_fatfs.h"

#include "fatfs.h"


//需要在sd_diskio.c中加入下面两个回调，否则DMA+FATFS有问题
//使用fatfs+FreeRTOS时,另外需要在stm32f405xx.h中__NVIC_PRIO_BITS的4U改成4，否则会出问题
//void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
//{
//  osMessagePut(SDQueueID, WRITE_CPLT_MSG, osWaitForever);
//}


//void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
//{

//  osMessagePut(SDQueueID, READ_CPLT_MSG, osWaitForever);
//}

// @ function: fat_test
// @ description:测试fatfs
// @ input:
// @ note:在os调用过程中,堆栈要大一些，512不够,需要1024
void fat_test(void){
  FATFS fs;                 // Work area (file system object) for logical drive
	FIL fil;                  // file objects
	 
	uint32_t byteswritten;                /* File write counts */
	uint32_t bytesread;                   /* File read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                     /* File read buffers */
	char filename[] = "STM32cube.txt";
//	char filename[] = "S.txt";
	
    printf("\r\n ****** FatFs Example ******\r\n\r\n");
 
    /*##-1- Register the file system object to the FatFs module ##############*/
    retSD = f_mount(&fs, "", 0);
    if(retSD)
    {
        printf(" mount error : %d \r\n",retSD);
        Error_Handler();
    }
    else
        printf(" mount sucess!!! \r\n");
     
    /*##-2- Create and Open new text file objects with write access ######*/
    retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(retSD)
        //xprintf(" open file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" open file sucess!!! \r\n");
     
    /*##-3- Write data to the text files ###############################*/
    retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
    if(retSD)
        //xprintf(" write file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
    {
        printf(" write file sucess!!! \r\n");
        printf(" write Data : %s\r\n",wtext);
    }
     
    /*##-4- Close the open text files ################################*/
    retSD = f_close(&fil);
    if(retSD)
        //xprintf(" close error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" close sucess!!! \r\n");
     
    /*##-5- Open the text files object with read access ##############*/
    retSD = f_open(&fil, filename, FA_READ);
    if(retSD)
        //xprintf(" open file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" open file sucess!!! \r\n");
     
    /*##-6- Read data from the text files ##########################*/
    retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
    if(retSD)
        //xprintf(" read error!!! %d\r\n",retSD);
		fat_error_test(retSD);
    else
    {
        printf(" read sucess!!! \r\n");
        printf(" read Data : %s\r\n",rtext);
    }
     
    /*##-7- Close the open text files ############################*/
    retSD = f_close(&fil);
    if(retSD)  
        //xprintf(" close error!!! %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" close sucess!!! \r\n");
     
    /*##-8- Compare read data with the expected data ############*/
    if(bytesread == byteswritten)
    { 
        printf(" FatFs is working well!!!\r\n");
    }
}

// @ function: fat_error_test
// @ description:
// @ input:
// @ note:
void fat_error_test(uint8_t ret){
  switch(ret){
		case FR_OK :
			printf("磁盘正常\r\n");
			break;
		case FR_INVALID_DRIVE:
			printf("驱动器无效\r\n");
			break;
		case FR_NOT_READY:
			printf("磁盘驱动器无法工作\r\n");
			break;
		case FR_DISK_ERR:
			printf("底层磁盘IO错误\r\n");
			break;
		case FR_NO_PATH:
			printf("找不到路径\r\n");
			break;
		case FR_INVALID_NAME:
			printf("文件名无效\r\n");
			break;
		case FR_NO_FILE:
			printf("找不到该文件\r\n");
			break;
		case FR_EXIST:
			printf("文件已存在\r\n");
			break;
		default:
			printf("其他错误%d\r\n",ret);
			break;
	}
		
}
