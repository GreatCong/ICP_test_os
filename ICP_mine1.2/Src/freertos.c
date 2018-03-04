/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "rw_app.h"
#include "tim.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId appTaskHandle;
osThreadId clientTaskHandle;
osThreadId mallocTaskHandle;
osMutexId buf_mutexHandle;

/* USER CODE BEGIN Variables */
#define SIZE 53
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartAppTask(void const * argument);
void StartClientTask(void const * argument);
void StartMallocTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
/************************************
// Method:    platform_init
// Date:  	  2016/06/27
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void
// Function:  ����RAK439�����ͳ�ʼ������ ��ȡRAK439��汾�������ַ 
************************************/
int platform_init(void)
{
    rw_DriverParams_t     params;
    int                   ret =0;
    char                  libVersion[20]="";
    char                  module_mac[6] ="";

    /*RAK439�û���������*/
    wifi_init_params(&params);

    /*RAK439������ʼ��*/
    ret =rw_sysDriverInit(&params);
    DPRINTF("rw_sysDriverInit ret:%d\r\n",ret);
    if(ret != RW_OK)
    {
        DPRINTF("RAK module platform init...failed\r\n");
        while(1);
    }

    /*��ȡRAK439��汾*/
    rw_getLibVersion(libVersion); 
    DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);

    /*��ȡRAK439�����ַ*/
    rw_getMacAddr(module_mac);
    DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);

    return RW_OK;
}

/************************************
// Method:    get_xtask_state
// Date:  	  2016/06/27
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
// Function:  ��ȡÿ�������״̬��Ϣ
************************************/
void get_xtask_state(void)
{
    char tmp[128] = "";
    int res_getstate = 0;
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, index;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
    TaskStatus_t *pxTaskStatusArray = NULL;

    /*��ȡ������Ŀ��������һ�����񣨸�����Ϊfreertos��IDLE��������*/
    uxArraySize = uxTaskGetNumberOfTasks(); 
    assert_param(uxArraySize>0);

    /*Ϊ������Ϣ���������ڴ�*/
    pxTaskStatusArray = pvPortMalloc(sizeof(TaskStatus_t)*uxArraySize);
    assert_param(pxTaskStatusArray);

    /*��ȡ������Ϣ*/
    res_getstate = uxTaskGetSystemState(pxTaskStatusArray,uxArraySize,&ulTotalRunTime);
    assert_param(res_getstate);

    //DPRINTF("********************������Ϣͳ�Ʊ�*********************\r\n\r\n");
    //DPRINTF("������    ״̬    ID  ���ȼ�   ��ջ  CPUʹ����\r\n");
    if (ulTotalRunTime>0)
    {
        for (index=0; index<uxArraySize; index++)
        {
            ulStatsAsPercentage = (uint64_t)(pxTaskStatusArray[index].ulRunTimeCounter)*100 / ulTotalRunTime;
            if (ulStatsAsPercentage > 0UL)
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8d%d%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark,
                    ulStatsAsPercentage);
            }
            else
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8dt<1%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark); 
            }
            //DPRINTF("%s\r\n",tmp);
        }
    }
    //DPRINTF("����״̬:   r-����  R-����  B-����  S-����  D-ɾ��\r\n"); 
    DPRINTF("�ڴ�ʣ����:%d Byte\r\n\r\n",xPortGetFreeHeapSize());

    /*�ͷ��ڴ�*/
    vPortFree(pxTaskStatusArray);
}

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
//__weak void configureTimerForRunTimeStats(void)
//{

//}

//__weak unsigned long getRunTimeCounterValue(void)
//{
//return 0;
//}

//��������ĺ���
void configureTimerForRunTimeStats(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}

unsigned long getRunTimeCounterValue(void)
{
    /*���ض�ʱ��3�жϴ������ı���*/
    return Tim3Counter;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */

//__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
//{
//   /* Run time stack overflow checking is performed if
//   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
//   called if a stack overflow is detected. */
//}

//��������ĺ���
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
    DPRINTF("%s StackOverFlow!\r\n",pcTaskName);
}

/* USER CODE END 4 */

/* USER CODE BEGIN 5 */

//__weak void vApplicationMallocFailedHook(void)
//{
//   /* vApplicationMallocFailedHook() will only be called if
//   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
//   function that will get called if a call to pvPortMalloc() fails.
//   pvPortMalloc() is called internally by the kernel whenever a task, queue,
//   timer or semaphore is created. It is also called by various parts of the
//   demo application. If heap_1.c or heap_2.c are used, then the size of the
//   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
//   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
//   to query the size of free heap space that remains (although it does not
//   provide information on how the remaining heap might be fragmented). */
//}

//��������ĺ���
void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created. It is also called by various parts of the
    demo application. If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    DPRINTF("vApplicationMallocFailedHook!\r\n");
}

/* USER CODE END 5 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of buf_mutex */
  osMutexDef(buf_mutex);
  buf_mutexHandle = osMutexCreate(osMutex(buf_mutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of appTask */
  osThreadDef(appTask, StartAppTask, osPriorityAboveNormal, 0, 256);
  appTaskHandle = osThreadCreate(osThread(appTask), NULL);

  /* definition and creation of clientTask */
  osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
  clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

  /* definition and creation of mallocTask */
  osThreadDef(mallocTask, StartMallocTask, osPriorityNormal, 0, 128);
  mallocTaskHandle = osThreadCreate(osThread(mallocTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		get_xtask_state();//��ȡ�����״̬
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
		//HAL_Delay(1000);
    osDelay(5000);//ÿ��5���ȡһ��ÿ�������״̬��Ϣ
  }
  /* USER CODE END StartDefaultTask */
}

/* StartAppTask function */
void StartAppTask(void const * argument)
{
  /* USER CODE BEGIN StartAppTask */
//  int ret = RW_OK;
//    /*RAK439������ʼ��*/
//    platform_init();

//    /*��ʼ��app����*/
//    rw_appdemo_context_init();

//    /*����·����*/ 
//    //�˹��ܺ���������һ��ʱ��
//    ret = rw_network_startSTA();
//    osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 2048);
//    clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//    /* Infinite loop */
//    for(;;)
//    {
//        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
//            DPRINTF("reconnect and ipquery...\r\n");
//            /*ɾ��clientTask*/
//            //osThreadTerminate(clientTaskHandle);
//            //rw_appdemo_context_init();   
//            //rw_sysDriverReset();
//            //Ҳ����ֱ�ӵ���rw_network_startSTA()ʵ��·������������
//            //rw_network_startSTA();
//            //osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
//            //clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//        }
//        rw_sysSleep(100);
//			}
test_netWorkTask();
  /* USER CODE END StartAppTask */
}

/* StartClientTask function */
void StartClientTask(void const * argument)
{
  /* USER CODE BEGIN StartClientTask */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
	//���Task������
	int      ret = 0;
    char str[SIZE];
		memset(str, 0, sizeof(str));
		int tmpIndex = 0;
		for (int i = 0; i < SIZE; ++i)
		{
			str[i] = ('0' + tmpIndex);
			++tmpIndex;
			if (tmpIndex == 10)
			{
				tmpIndex = 0;
			}
		}
		str[SIZE - 3] = '\r';
		str[SIZE - 2] = '\n';
		str[SIZE - 1] = '\0';
	  DPRINTF("str:%s\r\n",str);
		char revBuff[SIZE];
		memset(revBuff,0,SIZE);
    /* Infinite loop */
    for(;;)
    {      
reconnect: 
        //
        if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
        {
            DPRINTF("δ���ӷ�����\r\n");
            rw_sysSleep(1000);
            goto reconnect;      
        }


        if(app_demo_ctx.rw_connect_status == STATUS_OK && app_demo_ctx.rw_ipquery_status == STATUS_OK) 
        {
            if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
            {
                //if((ret =RAK_TcpClient(8000, 0xC0A80469)) >= 0)//192.168.04.105
									if((ret =RAK_TcpClient(6001, 0xC0A81701)) >= 0)//192.168.23.1
                {
                    app_demo_ctx.tcp_cloud_sockfd = ret;
                    app_demo_ctx.tcp_cloud_status = STATUS_OK;
                    DPRINTF("RAK_TcpClient sockfd = %u creat\r\n",app_demo_ctx.tcp_cloud_sockfd);
										DPRINTF("SEND START***************************************\r\n\r\n");
                    ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("SEND End***************************************\r\n\r\n");
										DPRINTF("send ret1:%d\r\n",ret);
									
										ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("send ret2:%d\r\n",ret);
										
									  recv(app_demo_ctx.tcp_cloud_sockfd,revBuff,sizeof(revBuff),0);
										DPRINTF("revFromServer:%s\r\n",revBuff);
									  
									  close(app_demo_ctx.tcp_cloud_sockfd);//���Task�����⣬�Ῠ��������
                    app_demo_ctx.tcp_cloud_status = STATUS_INIT;
                    app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
										break;

                }
                else
                {
                    if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
                    { 
                        DPRINTF("RAK_TcpClient creat failed code=%d\r\n", ret);
                        rw_sysSleep(100);
                        goto reconnect;
                    }
                }    
            }
        }
        rw_sysSleep(1000);
    }
  /* USER CODE END StartClientTask */
}

/* StartMallocTask function */
void StartMallocTask(void const * argument)
{
  /* USER CODE BEGIN StartMallocTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMallocTask */
}

/* USER CODE BEGIN Application */

   void test_netWorkTask(void)//����Wifi����
	 {
	     platform_init(); 
    rw_appdemo_context_init();

    rw_network_startSTA();
		#define TCPC_TEST//����TCP clint
		 
		#ifdef  TCPS_TEST
				creat_tcpsTask();
		#endif  
		#ifdef  TCPC_TEST
				creat_tcpcTask();
		#endif
		#ifdef  UDPS_TEST
				 creat_udpsTask();
		#endif
		#ifdef  UDPC_TEST
				 creat_udpcTask();
		#endif 
    
    while(1) {
      
        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
          DPRINTF("reconnect and ipquery...\r\n");
          rw_appdemo_context_init();   
          rw_sysDriverReset();
          rw_network_init(&conn, DHCP_CLIENT, NULL);
        }
        rw_sysSleep(100);
    }
		
	 }
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
