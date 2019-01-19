/***********************************************************************
文件名称：main.C
功    能：UDP客户端模式收发测试，基于UC0S II操作系统
实验平台：基于STM32F407VET6 开发板
库版本  ：V1.0.0 
***********************************************************************/
#include "main.h"
#include "iap.h"
#include "rsu_parse.h"

#define	TRAFFICLIGHT_IAP
//#define TRAFFICLIGHT_UPGRADE_FLAG
int tafficlight_dbg_level = 1;

int main(void)
{
	IPInfo data;
	#ifndef TRAFFICLIGHT_IAP
	SCB->VTOR = FLASH_BASE | 0x10000;//设置偏移量
	#endif
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	#ifdef TRAFFICLIGHT_UPGRADE_FLAG
	data.update_flag = 0;
  STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	#endif
	#ifdef TRAFFICLIGHT_IAP
	if(data.update_flag == 1)
		iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
	#endif
	//初始化板子
	BSP_Init();
	if(data.update_flag == 0)
		info_msg("TRAFFICLIGHT_IAP\r\n");
	else
		info_msg("TRAFFICLIGHT_APP\r\n");
	#if 1
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
	#endif					
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
			
	info_msg("TrafficLight v1.0.0 build time:%s %s\r\n", __TIME__,__DATE__);
	#if 1
	OSTaskCreate(Task_UDP_Client, (void *)0, &Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE-1], OS_USER_PRIO_GET(5));
	
	info_msg("network task start...\r\n");
	OSTaskCreate(Task_LED, (void *)0, &Stk_Task_LED[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(6));
	#endif
	info_msg("TrafficLight task start...\r\n");
	//#ifdef QIJUN
	OSTaskCreate(Task_QiJun, (void *)0, &Stk_Task_QIJUN[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(7));
	//#endif
	IWDG_Init(4,2500);
	while (1)
	{
		OSTimeDlyHMSM(0, 0, 1, 0);//1000ms
		GPIO_ToggleBits(LED1);
		//IWDG_Feed();
	}
}
