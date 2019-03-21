#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "timer.h"
#include "led.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"
#include "tcp_server.h"
#include "trafficlight.h"
#include "rsu_parse.h"
#include "iwdg.h"
#include "iap.h"
#include "drv_trafficlight.h"
#define	TRAFFICLIGHT_IAP
//#define TRAFFICLIGHT_UPGRADE_FLAG

//TRAFFICLIGHT任务
//任务优先级
#define TRAFFICLIGHT_TASK_PRIO		9
//任务堆栈大小
#define TRAFFICLIGHT_STK_SIZE		256
//任务堆栈
OS_STK	TRAFFICLIGHT_TASK_STK[TRAFFICLIGHT_STK_SIZE];
//任务函数
void trafficlight_task(void *pdata);

//通信任务
//任务优先级
#define COMM_TASK_PRIO	10	
//任务堆栈大小
#define COMM_STK_SIZE	1500
//任务堆栈
OS_STK	COMM_TASK_STK[COMM_STK_SIZE];
//任务函数
void communication_task(void *pdata);

//START任务
//任务优先级
#define START_TASK_PRIO		11
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 


int main(void)
{
	IPInfo data;
	#ifndef TRAFFICLIGHT_IAP
	SCB->VTOR = FLASH_BASE | 0x40000;//设置偏移量
	#endif
	#ifdef TRAFFICLIGHT_UPGRADE_FLAG
	data.update_flag = 0;
  STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	#endif
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	#ifdef TRAFFICLIGHT_IAP
	if(data.update_flag == 1)
	{
		iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
	}
	#endif

	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	uart_init(115200);   	//串口波特率设置
	delay_ms(200);
	
	LED_Init(); 			//LED初始化
	LED0 = 0;
	HC165D_Config();

	if(data.update_flag == 1)
		printf("TRAFFICLIGHT_APP\r\n");
	else
		printf("TRAFFICLIGHT_IAP\r\n");
	
	#ifndef TRAFFICLIGHT_IAP
	printf("trafficlight app \r\n");
	#endif
	
	mymem_init(SRAMIN);  	//初始化内部内存池
	OSInit(); 					//UCOS初始化
	while(lwip_comm_init()) 	//lwip初始化
	{		
		delay_ms(500);
	}
	
	while(tcp_server_init()) 									//初始化tcp_server(创建tcp_server线程)
	{
		delay_ms(500);
	}

	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//创建DHCP任务
#endif
	IWDG_Init(4,2500);
	OSTaskCreate(trafficlight_task,(void*)0,(OS_STK*)&TRAFFICLIGHT_TASK_STK[TRAFFICLIGHT_STK_SIZE-1],TRAFFICLIGHT_TASK_PRIO); 	//创建LED任务
	OSTaskCreate(communication_task,(void*)0,(OS_STK*)&COMM_TASK_STK[TRAFFICLIGHT_STK_SIZE-1],COMM_TASK_PRIO); 	//创建LED任务
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  //开中断
}

//LED闪烁任务
void trafficlight_task(void *pdata)
{
	uint16_t count = 0;
	IPInfo data;
	//OSTimeDlyHMSM(0, 0, 5, 0);		//红绿灯上电时，延时等待红绿灯正常工作
	trafficLightInit();
	printf("trafficlight_task\r\n");
  while(1)
	{
		count++;
		/* 每隔5分钟重新更新红绿灯周期 */
		if(count > 30000)
		{
			count = 0;
			trafficLight_study_once();
		}
		
		if((count % 50) == 0)
		{
			LED0 = !LED0;
			IWDG_Feed();
		}
		
		if(g_network_count++ > 30000)
		{
			STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
			data.reset_num++;
			printf("network is not work, restart %d.\r\n", data.reset_num);
			STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
			__set_FAULTMASK(1); //关闭所有中断
			NVIC_SystemReset(); //复位
		}
		
		OSTimeDlyHMSM(0, 0, 0, 20);			//20
		trafficLightRead();
		trafficLightStudy();
		trafficLightWork();
	}
}

void communication_task(void *pdata)
{	
	u8 buf[100];
	u8 len;
	printf("communication_task\r\n");
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 40);
		RS485_Receive_Data(buf, &len);
		rsuParse(tcp_server_recvbuf, TCP_SERVER_RX_BUFSIZE);
		rsuParse(buf, len);

	}
}








