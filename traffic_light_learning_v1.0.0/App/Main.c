/***********************************************************************
文件名称：main.C
功    能：UDP客户端模式收发测试，基于UC0S II操作系统
实验平台：基于STM32F407VET6 开发板
库版本  ：V1.0.0 
***********************************************************************/
#include "main.h"

int tafficlight_dbg_level = 1;
int main(void)
{
	//初始化板子
	BSP_Init();
	//初始化系统
	OSInit();
	//创建启动任务
	OSTaskCreate(	Task_StartUp,               		    				//指向任务代码的指针
                  	(void *) 0,												//任务开始执行时，传递给任务的参数的指针
					(OS_STK *)&Stk_TaskStartUp[TASK_STARTUP_STK_SIZE - 1],	//分配给任务的堆栈的栈顶指针   从顶向下递减
					(INT8U) OS_USER_PRIO_LOWEST);							//分配给任务的优先级  

	//节拍计数器清0  
	OSTimeSet(0);

	//启动UCOS-II内核
	OSStart();
	
	return 0;
}
//启动任务堆栈
OS_STK Stk_TaskStartUp[TASK_STARTUP_STK_SIZE];

void Task_StartUp(void *pdata)
{

	//初始化UCOS时钟
	//OS_TICKS_PER_SEC 为 UCOS-II 每秒嘀嗒数
	SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC - 1);

	//优先级说明，使用OS_USER_PRIO_GET(n)宏来获取
	//OS_USER_PRIO_GET(0)最高,OS_USER_PRIO_GET(1)次之，依次类推
	//OS_USER_PRIO_GET(0)：最高的优先级，主要用于在处理紧急事务，需要将优先处理的任务设置为最高这个优先级  

	//OSTaskCreate(	Task_Xxx,               		    					//指向任务代码的指针
    //              (void *)0,												//任务开始执行时，传递给任务的参数的指针
	//				(OS_STK *)&Stk_TaskXxx[TASK_XXX_STK_SIZE - 1],			//分配给任务的堆栈的栈顶指针   从顶向下递减
	//				(INT8U) OS_USER_PRIO_GET(N));							//分配给任务的优先级  
	info_msg("红绿灯学习系统v1.0.0 编译时间:%s %s\r\n", __TIME__,__DATE__);
	OSTaskCreate(Task_UDP_Client, (void *)0, &Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE-1], OS_USER_PRIO_GET(5));
	info_msg("网络传输任务启动...\r\n");
	OSTaskCreate(Task_LED, (void *)0, &Stk_Task_LED[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(6));
	info_msg("模拟红绿灯任务启动...\r\n");
	OSTaskCreate(Task_TRAFFIC, (void *)0, &Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(7));
	info_msg("红绿灯学习任务启动...\r\n");
	while (1)
	{
		
		OSTimeDlyHMSM(0, 0, 1, 0);//1000ms
	}
}
