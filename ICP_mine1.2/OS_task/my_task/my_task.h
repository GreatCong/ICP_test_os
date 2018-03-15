#ifndef __MY_TASK_H__
#define __MY_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/*define*/
#define TX_BUF_LEN_USB 1024*6 //USB缓冲数组大小
#define AD_QUEUE_SIZE 1500 //AD环形队列的大小


/*function*/
void get_xtask_state(void);//获取每个任务的状态信息

void test_netWorkTask(void);
void StartClientTask(void const * argument);

int creat_tcpcTask1(void);

#endif
