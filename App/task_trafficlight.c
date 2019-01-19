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

//LED��˸����
void Task_LED(void *pdata)
{
	uint16_t count = 0;
	OSTimeDlyHMSM(0, 0, 5, 0);		//���̵��ϵ�ʱ����ʱ�ȴ����̵���������
	trafficLightInit();
  while(1)
	{
		count++;
		/* ÿ��5�������¸��º��̵����� */
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
	OSTimeDlyHMSM(0, 0, 0, 10);//����10ms���Ա������߳�����
}

extern char tcp_server_sendbuf[120];
extern struct tcp_pcb *tcppcbnew;  	//����һ��TCP���������ƿ�
extern struct tcp_pcb *tcppcbconn;  	//����һ��TCP���������ƿ�
extern char g_send_light_info;
//UDP�������շ�����

void send_trafficlight_info()
{
	uint8_t i;
	for(i = 0; i < g_online_num; i++)
	{
		trafficLightStatus(&light_info[i], i);
						
		if(i == (g_online_num - 1))
		{
				memcpy(tcp_server_sendbuf, &light_info[0], sizeof(LightInfo) * g_online_num);
				tcp_server_flag|=1<<7;//���Ҫ��������
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
	UART3SendByte(0x89);		//���ڲ���RS232	
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
				tcp_server_flag&=~(1<<6);//��������Ѿ���������.
			}
			
			if(tcp_server_flag&1<<5)
			{
			//	info_msg("connect ok \r\n");
				if(count++ > 100*60*3)
				{
					__set_FAULTMASK(1); //�ر������ж�
				NVIC_SystemReset(); //��λ
				}
			}
			
		}
		info_msg("tcp close \r\n");
		tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
		tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server���� 
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

