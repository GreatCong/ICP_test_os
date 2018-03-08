/**
 *************************************************************************************
 * @file task_utilities.c
 * @author liucongjun
 * @version 0.1
 * @date 2018-3-7
 * @brief FreeRTOS裁剪的函数
 *************************************************************************************
*/
#include "my_task.h"

#include "rw_app.h"
#include "tim.h"

/************************************
// Method:    get_xtask_state
// Date:  	  2016/06/27
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
// Function:  获取每个任务的状态信息
************************************/
void get_xtask_state(void)
{
    char tmp[128] = "";
    int res_getstate = 0;
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, index;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
    TaskStatus_t *pxTaskStatusArray = NULL;

    /*获取任务数目，至少有一个任务（该任务为freertos的IDLE空闲任务）*/
    uxArraySize = uxTaskGetNumberOfTasks(); 
    assert_param(uxArraySize>0);

    /*为任务信息变量分配内存*/
    pxTaskStatusArray = pvPortMalloc(sizeof(TaskStatus_t)*uxArraySize);
    assert_param(pxTaskStatusArray);

    /*获取任务信息*/
    res_getstate = uxTaskGetSystemState(pxTaskStatusArray,uxArraySize,&ulTotalRunTime);
    assert_param(res_getstate);

    //DPRINTF("********************任务信息统计表*********************\r\n\r\n");
    //DPRINTF("任务名    状态    ID  优先级   堆栈  CPU使用率\r\n");
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
    //DPRINTF("任务状态:   r-运行  R-就绪  B-阻塞  S-挂起  D-删除\r\n"); 
    DPRINTF("内存剩余量:%d Byte\r\n\r\n",xPortGetFreeHeapSize());

    /*释放内存*/
    vPortFree(pxTaskStatusArray);
}

/*FreeeRTOS 裁剪*/
void configureTimerForRunTimeStats(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}

unsigned long getRunTimeCounterValue(void)
{
    /*返回定时器3中断处理函数的变量*/
    return Tim3Counter;
}

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
    DPRINTF("%s StackOverFlow!\r\n",pcTaskName);
}

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

/*FreeeRTOS 裁剪 end */
