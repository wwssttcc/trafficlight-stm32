#include "main.h"  

OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];


typedef struct TrafficLight{
	LightType last_light;			//ǰһ�κ��̵�״̬
	LightType current_light;	//��ǰ���̵�״̬
	uint8_t	last_seconds;			//��ǰ��ʣ��ʱ��
	uint8_t study_flag;				//ѧϰ��־
	uint8_t light_period[3];	//ѧϰ��ó����̵�����
	uint32_t start_ticks;			//ÿһ�κ��̵�״̬�仯ʱ��ticks
	uint32_t current_ticks;		//��ǰticksֵ
	uint32_t total_ticks;			//total_ticks = current_ticks - start_ticks
}TrafficLight;

TrafficLight g_trafficlight[4][3];	//ÿ��·����3����̵ƣ�����ǰ�У���գ��ҹ�

void trafficLightInit()
{
	memset(g_trafficlight, 0, 12 * sizeof(TrafficLight));
	uint8_t bitstatus = 0x00;
	bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].current_light = red;
			LED1_ON;
		}
		else
			LED1_OFF;
		
		bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].current_light = green;
			LED2_ON;
		}
		else
			LED2_OFF;
		
		bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].current_light = yellow;
			LED3_ON;
		}
		else
			LED3_OFF;
		
		g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
		g_trafficlight[0][0].study_flag = 1;
}

void trafficLightStudy()
{
	if(g_trafficlight[0][0].study_flag == 1)
	{
		//���ϴκ��̵�״̬�뵱ǰ���̵�״̬��һ�£�����Ϊ״̬�����ı�
		if(g_trafficlight[0][0].last_light != g_trafficlight[0][0].current_light)
		{
			g_trafficlight[0][0].current_ticks = OSTimeGet();
			//�������״̬�µĺ��̵�start_ticks����Ϊ0
			if(g_trafficlight[0][0].start_ticks != 0)
			{
				//�������ǰ���̵�����
				g_trafficlight[0][0].light_period[g_trafficlight[0][0].last_light] = (g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks) / 100;
				g_trafficlight[0][0].total_ticks = g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks;
			}
			g_trafficlight[0][0].start_ticks = OSTimeGet();
			//ֻ�к��̻����ڶ����������ѧϰ����
			if(g_trafficlight[0][0].light_period[red] && g_trafficlight[0][0].light_period[green] && g_trafficlight[0][0].light_period[yellow])
				g_trafficlight[0][0].study_flag = 0;
		}
	}
}

void trafficLightWork()
{
	//���̵Ʋ�ѧϰʱ��ֻ���¼ÿ��״̬�ı�ʱstart_ticks��ֵ
	if(g_trafficlight[0][0].study_flag == 0)
	{
		if(g_trafficlight[0][0].last_light != g_trafficlight[0][0].current_light)
			g_trafficlight[0][0].start_ticks = OSTimeGet();
	}
}

void trafficLightRead()
{
	uint8_t bitstatus = 0x00;
	bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
			g_trafficlight[0][0].current_light = red;
			LED1_ON;
		}
		else
			LED1_OFF;
		
		bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
			g_trafficlight[0][0].current_light = green;
			LED2_ON;
		}
		else
			LED2_OFF;
		
		bitstatus = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
		if(bitstatus == 0)
		{
			g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
			g_trafficlight[0][0].current_light = yellow;
			LED3_ON;
		}
		else
			LED3_OFF;
}

//LED��˸����
void Task_LED(void *pdata)
{	
	trafficLightInit();
  while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 5);
		trafficLightRead();
		trafficLightStudy();
		trafficLightWork();
	}
}

void Task_TRAFFIC(void *pdata)
{
	uint8_t i = 0;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1000);
		i++;
		if(i == 5)
		{
			TRAFFIC_LED1_OFF;
			TRAFFIC_LED2_ON;
		}
		if(i == 20)
		{
			TRAFFIC_LED2_OFF;
			TRAFFIC_LED3_ON;
		}
		if(i == 40)
		{
			TRAFFIC_LED3_OFF;
			TRAFFIC_LED1_ON;
			i = 0;
		}
	}
}

extern unsigned char UDPData[5];
//UDP�������շ�����
void Task_UDP_Client(void *pdata)
{
	uint32_t last_ticks;
	uint32_t current_ticks;
	uint8_t	current_light;
	uint32_t total_ticks = 0;
	LightInfo light_info;
	//uint8_t send_buf[5];
	__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	
	ETH_BSP_Config();
	LwIP_Init();
	
	/* UDP_client Init */
	UDP_client_init();
	udp_p -> payload = (void *)&light_info;
	light_info.head = 0xA55A;
	light_info.code1 = 0x83;
	light_info.code2 = 0xA0;
	light_info.code3 = 0x00;
	light_info.tail = 0x5AA5;
	while(1)
	{  
		LocalTime += 10;
		LwIP_Periodic_Handle(LocalTime);		
		OSTimeDlyHMSM(0, 0, 0, 500);//����10ms���Ա������߳�����
		light_info.direct = south;
		light_info.to = front;
		light_info.light = g_trafficlight[0][0].current_light;
		total_ticks = OSTimeGet() - g_trafficlight[0][0].start_ticks;
		light_info.last_seconds = g_trafficlight[0][0].light_period[light_info.light] - total_ticks / 100;

		if(g_trafficlight[0][0].study_flag == 1)
			info_msg("����ѧϰ��\r\n");
		else
		{
			if(light_info.light == red)
				info_msg("���ʣ��%02d��\r\n", light_info.last_seconds);
			else if(light_info.light == green)
				info_msg("�̵�ʣ��%02d��\r\n", light_info.last_seconds);
			else
				info_msg("�Ƶ�ʣ��%02d��\r\n", light_info.last_seconds);
		}
		light_info.check = 0xff;
		//sprintf(UDPData, "%d", total_ticks);
		udp_send(udp_pcb,udp_p);//��������
	}
}

