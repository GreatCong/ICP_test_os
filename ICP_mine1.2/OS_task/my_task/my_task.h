#ifndef __MY_TASK_H__
#define __MY_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

void get_xtask_state(void);//��ȡÿ�������״̬��Ϣ

void test_netWorkTask(void);
void StartClientTask(void const * argument);

int creat_tcpcTask1(void);

#endif
