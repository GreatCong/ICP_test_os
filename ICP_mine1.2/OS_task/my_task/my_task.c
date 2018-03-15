/**
 *************************************************************************************
 * @file my_task.c
 * @author liucongjun
 * @version 1.0
 * @date 2018-3-7
 * @brief 自定义的task
 *************************************************************************************
*/
#include "my_task.h"

#include "rw_app.h"
#include "tim.h"
#include "usbd_cdc_if.h"
#include "fatfs.h"
#include "my_fatfs.h"

#define SIZE 53

//typedef uint8_t Elemtype;
typedef int16_t Elemtype;	

Elemtype myAD_test_data[20] = {50,65,79,90,97,100,97,90,79,65,50,34,20,9,2,0,2,10,21,35};//正弦
	                      //hex={32,41,4f,5A,61,64 ,61,5A,4F,41,32,22,14,9,2,0,2,a ,15,23}
Elemtype myAD_test_data1[60]={0x258,0x28c,0x2bf,0x2f2,0x323,0x351,0x37d,0x3a6,0x3cb,0x3ec,
	                            0x409,0x420,0x433,0x441,0x449,0x44b,0x449,0x441,0x433,0x420,
	                            0x409,0x3ec,0x3cb,0x3a6,0x37d,0x352,0x323,0x2f2,0x2bf,0x28c,
	                            0x258,0x223,0x1f0,0x1bd,0x18c,0x15e,0x132,0x109,0xe4, 0xc3,
	                            0xa6, 0x8f, 0x7c, 0x6e, 0x66, 0x64, 0x66, 0x6e, 0x7c, 0x8f,
	                            0xa6, 0xc3, 0xe4, 0x109,0x132,0x15d,0x18c,0x1bd,0x1f0,0x223,};
#include "my_queue.h"
//#define AD_QUEUE_SIZE 1200
extern QueueArray_type myAD_queue;
Elemtype my_send[AD_QUEUE_SIZE+100] = {0};

/************************************
// Method:    platform_init
// Date:  	  2016/06/27
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void
// Function:  配置RAK439参数和初始化驱动 获取RAK439库版本与物理地址 
************************************/
int platform_init(void)
{
    rw_DriverParams_t     params;
    int                   ret =0;
    char                  libVersion[20]="";
    char                  module_mac[6] ="";

    /*RAK439用户参数配置*/
    wifi_init_params(&params);

    /*RAK439驱动初始化*/
    ret =rw_sysDriverInit(&params);
    DPRINTF("rw_sysDriverInit ret:%d\r\n",ret);
    if(ret != RW_OK)
    {
        DPRINTF("RAK module platform init...failed\r\n");
        while(1);
    }

    /*获取RAK439库版本*/
    rw_getLibVersion(libVersion); 
    DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);

    /*获取RAK439物理地址*/
    rw_getMacAddr(module_mac);
    DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);

    return RW_OK;
}



/*tasks*/

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		get_xtask_state();//获取任务的状态
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
		//HAL_Delay(1000);
    osDelay(5000);//每隔5秒获取一次每个任务的状态信息
  }
  /* USER CODE END StartDefaultTask */
}

/* StartAppTask function */
void StartAppTask(void const * argument)
{
  /* USER CODE BEGIN StartAppTask */
//	int ret = RW_OK;
//    /*RAK439驱动初始化*/
//    platform_init();

//    /*初始化app变量*/
//    rw_appdemo_context_init();

//    /*连接路由器*/ 
//    //此功能函数会阻塞一段时间
//    ret = rw_network_startSTA();
//    osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 2048);
//    clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//    /* Infinite loop */
//    for(;;)
//    {
//        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
//            DPRINTF("reconnect and ipquery...\r\n");
//            /*删除clientTask*/
//            //osThreadTerminate(clientTaskHandle);
//            //rw_appdemo_context_init();   
//            //rw_sysDriverReset();
//            //也可以直接调用rw_network_startSTA()实现路由器重连功能
//            //rw_network_startSTA();
//            //osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
//            //clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//        }
//        rw_sysSleep(100);

//    }
   test_netWorkTask();
  /* USER CODE END StartAppTask */
}

/* StartTest_Task function */
void StartTest_Task(void const * argument)
{
  /* USER CODE BEGIN StartTest_Task */
  /* Infinite loop */
  for(;;)
  {
//		uint8_t xx[] = "abc";
//	  UsbSendData(xx,3);
//		fat_test();//写文件会影响到wifi,添加这个函数偶尔会出现丢包现象
		osDelay(5000);
  }
  /* USER CODE END StartTest_Task */
}

/* StartInquireTask function */
void StartInquireTask(void const * argument)//不断查询的Task,优先级为High
{
  /* USER CODE BEGIN StartInquireTask */
	uint8_t buf;
  /* Infinite loop */
  for(;;)
  {
		//在整合的板卡上出现无法接收的情况，将此任务改成osPriorityHigh，能够解决这个问题		
    UsbReceiveData(&buf);
		if(buf=='Y')
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			buf=0;
		}
		else if(buf=='N') 
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
			buf=0;   
		}
		osDelay(10);
  }
  /* USER CODE END StartInquireTask */
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
	//这个Task有问题
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
            DPRINTF("未连接服务器\r\n");
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
									  
									  close(app_demo_ctx.tcp_cloud_sockfd);//这个Task有问题，会卡死在这里
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

// @ function: 
// @ description:测试wifi传输性能的函数
// @ input:
// @ note:
void test_netWorkTask(void)//测试Wifi传输
{
	     platform_init(); 
    rw_appdemo_context_init();

    rw_network_startSTA();
//	rw_network_startAP();
//		#define TCPC_TEST//测试TCP clint
//	#define TCPS_TEST//测试TCP Server
		 
		#ifdef  TCPS_TEST
				creat_tcpsTask();
		#endif  
		#ifdef  TCPC_TEST
//				creat_tcpcTask();
	creat_tcpcTask1();
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

#ifdef  TCPC_TEST 
#define RW_TCPC_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_tcpclient_task1;  
#endif

void RAK_TcpClient_EventHandle1(void *p_arg)
{
	int      ret = 0;
	static int      send_recvCnt = 0;
	int      temp_len;

//	char* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);//RAK439 TCP MAX_RECV_PACKET_LEN=1024
	Elemtype* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);//RAK439 TCP MAX_RECV_PACKET_LEN=1024

	if(NULL == c_buffer){
		DPRINTF("RAK_TcpClient: Allocate buffer failed.\n");
		while(1);
	}  

	while(1)
	{      
reconnect: 
		//连不上WIFI或者分配不到IP地址
		if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
		{
			rw_sysSleep(100);
			goto reconnect;      
		}

		if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
		{
			if((ret =RAK_TcpClient(6001, 0xC0A81701)) >= 0)//192.168.23.1
			//if((ret =RAK_TcpClient(25001, 0xC0A80389)) >= 0)
			{
				app_demo_ctx.tcp_cloud_sockfd = ret;
				app_demo_ctx.tcp_cloud_status = STATUS_OK;
				DPRINTF("RAK_TcpClient sockfd = %u creat \n\r",app_demo_ctx.tcp_cloud_sockfd);
			}
			else
			{
				if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
				{ 
					DPRINTF("RAK_TcpClient creat failed code=%d\n", ret);
					rw_sysSleep(100);
					goto reconnect;
				}
			}    
		}

		if (app_demo_ctx.tcp_cloud_status <= STATUS_INIT)
		{
			if(app_demo_ctx.tcp_cloud_status < STATUS_INIT)
			{
				close(app_demo_ctx.tcp_cloud_sockfd);
				app_demo_ctx.tcp_cloud_status = STATUS_INIT;
				app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
			}
			goto reconnect;
		}

//		ret = recv(app_demo_ctx.tcp_cloud_sockfd, c_buffer, MAX_RECV_PACKET_LEN, 0);//recv返回接受到的数据
//		if (ret <= 0 )
//		{
//			if(ret ==RW_ERR_SOCKET_INVAILD){
//				DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.tcp_cloud_sockfd);
//				app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
//			}

//		}else
//		{
//			//      DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.tcp_cloud_sockfd, ret );              
//			send_recvCnt += ret;
//		}
//   send_recvCnt = 60*2;
//	 c_buffer = myAD_test_data1;
				 send_recvCnt = GetQueue_length(myAD_queue);
//		if(send_recvCnt > 0){ReadQueue_n(&myAD_queue,my_send);}
	 
		if(send_recvCnt > 0)
		{
			temp_len = send_recvCnt >MAX_SEND_PACKET_LEN/2 ? MAX_SEND_PACKET_LEN/2:send_recvCnt;
			ReadQueue_n(&myAD_queue,my_send,temp_len);
			ret = send(app_demo_ctx.tcp_cloud_sockfd, my_send, temp_len*2, 0);//c_buffer是OS分配的内存，大小为1024
//			c_buffer = my_send;
//			ret = send(app_demo_ctx.tcp_cloud_sockfd, c_buffer, temp_len, 0);//c_buffer是OS分配的内存，大小为1024
			//temp_len = send_recvCnt >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:send_recvCnt;
			if (ret <= 0 )
			{
				//        DPRINTF("send errorcode =%d\n\r",ret);
				if(ret ==RW_ERR_SOCKET_INVAILD)
				{
					app_demo_ctx.tcp_cloud_sockfd =INVAILD_SOCK_FD;
//					send_recvCnt = 0;
				}                  
			}else
			{
//				send_recvCnt -= ret;
			}
		}
	}
}

int creat_tcpcTask1(void)
{
	int  xRet = 0;

	xRet =xTaskCreate(RAK_TcpClient_EventHandle1, 
		"TcpClient", 
		configMINIMAL_STACK_SIZE * 2, 
		NULL, 
		RW_TCPC_TASK_PRIO, 
		&g_tcpclient_task1); 

	if(xRet !=pdPASS){
		DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
		while(1);
	}
	
	return 0;

}
