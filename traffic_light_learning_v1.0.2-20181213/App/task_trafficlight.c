#include "main.h"  
#include "qijun_parse.h"
OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];
OS_STK Stk_Task_QIJUN[TASK_TEST_LED_STK_SIZE];
extern uint8_t aShowTime[50];
extern uint8_t aShowDate[50];



//LED闪烁任务
void Task_LED(void *pdata)
{	
	OSTimeDlyHMSM(0, 0, 5, 0);		//红绿灯上电时，延时等待红绿灯正常工作
	trafficLightInit();
  while(1)
	{
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

extern unsigned char UDPData[5];
extern struct tcp_pcb *g_pcb[3];
extern char g_accept_num;
extern uint16_t g_tcp_num[3];
void close_tcp_accept()
{
	char i;
	for(i = 0; i < 3; i++)
	{
		if(g_pcb[i] != NULL)
		{
			g_tcp_num[i]++;
			if(g_tcp_num[i] > 30)
			{
				tcp_close(g_pcb[i]); 
				g_pcb[i] = NULL;
				g_tcp_num[i] = 0;
				g_accept_num--;
			}
		}
	}
}


//UDP服务器收发任务
void Task_UDP_Client(void *pdata)
{
	LightInfo light_info;
	uint8_t i, data;
	uint16_t count = 0;
	struct pbuf *p_temp;
	//uint8_t send_buf[5];
	__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	
	ETH_BSP_Config();
	
	LwIP_Init();
	
	TCP_server_init();
	p_temp = pbuf_alloc(PBUF_RAW,sizeof(LightInfo),PBUF_RAM);
	p_temp->payload = (void *)&light_info;
	p_temp->len = sizeof(LightInfo);
	#ifdef UDPC
	/* UDP_client Init */
	UDP_client_init();
	udp_p -> payload = (void *)&light_info;
	#endif
	
	#ifdef USE_STMFLASH
	STMFLASH_Test();
	#endif
	while(1)
	{  
//		if_test();
		LocalTime += 10;
		LwIP_Periodic_Handle(LocalTime);		
		//OSTimeDlyHMSM(0, 0, 0, 30);//挂起10ms，以便其他线程运行  10
		OSTimeDlyHMSM(0, 0, 0, 50);
		#if 0
		close_tcp_accept();
		for(i = 0; i < 3; i++)
		{
			if(g_pcb[i] != NULL)
			{
				tcp_write(g_pcb[i],p_temp->payload,p_temp->len,TCP_WRITE_FLAG_COPY);
				tcp_output(g_pcb[i]);
			}
		}
		#endif
		
		for(i = 0; i < g_online_num; i++)
		{
			trafficLightStatus(&light_info, i);
		//sprintf(UDPData, "%d", total_ticks);
			#ifdef UDPC
			udp_send(udp_pcb,udp_p);//发送数据	
			#endif
			count++;
		}
		info_msg("----发送包count %d----\r\n", count);
	}
}

void Task_QiJun(void *pdata)
{
	uint8_t rcv_buf[512];
  int  rcv_len, i;
	ttf_info pttf_info;
	
	init_qijunReader();
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1000);
		//if_test();
		#if 0
		rcv_len = read_ringbuf(rcv_buf, 256);
//		info_msg("qijun len %d\r\n", rcv_len);
		for(i=0; i<rcv_len; i++)
		{
			//printf("%x ", rcv_buf[i]);
			qijun_reader(rcv_buf[i], &pttf_info);
		}
		#endif
	}
}

