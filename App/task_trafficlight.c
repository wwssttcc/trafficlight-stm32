#include "main.h"  
#include "qijun_parse.h"
#include "rsu_parse.h"

OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];
OS_STK Stk_Task_QIJUN[TASK_TEST_LED_STK_SIZE];
extern uint8_t aShowTime[50];
extern uint8_t aShowDate[50];
extern uint8_t tcp_server_flag;
LightInfo light_info[12];

//LED闪烁任务
void Task_LED(void *pdata)
{
	uint16_t count = 0;
	OSTimeDlyHMSM(0, 0, 5, 0);		//红绿灯上电时，延时等待红绿灯正常工作
	trafficLightInit();
  while(1)
	{
		count++;
		/* 每隔5分钟重新更新红绿灯周期 */
		if(count > 15000)
		{
			count = 0;
			trafficLight_study_once();
		}
		OSTimeDlyHMSM(0, 0, 0, 20);			//20
		trafficLightRead();
		trafficLightStudy();
		trafficLightWork();
	}
}

void if_test()
{
	uint8_t data;
	//RS485_test();
	//can1_test();
	//RTC_TimeShow();
	//RTC_DateShow();
	if(0 == ringBufRead(&data))
	{
		//USART_SendData(USART3, data);
		printf("%x", data);
		//GPIO_ToggleBits(LED1);
	}
	OSTimeDlyHMSM(0, 0, 0, 10);//挂起10ms，以便其他线程运行
}

extern char tcp_server_sendbuf[120];
extern struct tcp_pcb *tcppcbnew;  	//定义一个TCP服务器控制块
extern struct tcp_pcb *tcppcbconn;  	//定义一个TCP服务器控制块
extern char g_send_light_info;
//UDP服务器收发任务

void send_trafficlight_info()
{
	uint8_t i;
	for(i = 0; i < g_online_num; i++)
	{
		trafficLightStatus(&light_info[i], i);
						
		if(i == (g_online_num - 1))
		{
				memcpy(tcp_server_sendbuf, &light_info[0], sizeof(LightInfo) * g_online_num);
				tcp_server_flag|=1<<7;//标记要发送数据
		}
	}
}

void Task_UDP_Client(void *pdata)
{
	uint8_t res=0;
	uint32_t count = 0;
	__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	
	
	ETH_BSP_Config();
	
	LwIP_Init();
	UART3SendByte(0x89);		//用于测试RS232	
	while(1)
	{
		res = tcp_server_init();
		info_msg("tcp server init %d\r\n", res);
		while(res == 0)
		{  
			LocalTime += 10;
			LwIP_Periodic_Handle(LocalTime);		
			OSTimeDlyHMSM(0, 0, 0, 10);
			IWDG_Feed();
			
			if(tcp_server_flag&1<<6)
			{
				if(g_send_light_info)
					send_trafficlight_info();
				info_msg("-------------------------------\r\n");
				count = 0;
				tcp_server_flag&=~(1<<6);//标记数据已经被处理了.
			}
			
			if(tcp_server_flag&1<<5)
			{
			//	info_msg("connect ok \r\n");
				if(count++ > 100*60*3)
				{
					__set_FAULTMASK(1); //关闭所有中断
				NVIC_SystemReset(); //复位
				}
			}
			
		}
		info_msg("tcp close \r\n");
		tcp_server_connection_close(tcppcbnew,0);//关闭TCP Server连接
		tcp_server_connection_close(tcppcbconn,0);//关闭TCP Server连接 
		tcp_server_remove_timewait(); 
		memset(tcppcbnew,0,sizeof(struct tcp_pcb));
		memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
	}
}

extern char tcp_server_sendbuf[120];
extern u32 USART_RX_CNT;
extern u8 USART_RX_BUF[100]; 
extern uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
void Task_QiJun(void *pdata)
{	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 50);
		rsuParse(tcp_server_recvbuf, TCP_SERVER_RX_BUFSIZE);
		rsuParse(USART_RX_BUF, 100);
		USART_RX_CNT = 0;
	}
}

