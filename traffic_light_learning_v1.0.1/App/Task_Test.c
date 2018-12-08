#include "main.h"  

OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];

extern uint8_t aShowTime[50];
extern uint8_t aShowDate[50];

typedef struct TrafficLight{
	LightType last_light;			//ǰһ�κ��̵�״̬
	LightType current_light;	//��ǰ���̵�״̬
	uint8_t	last_seconds;			//��ǰ��ʣ��ʱ��
	uint8_t study_flag;				//ѧϰ��־
	uint8_t light_period[4];	//ѧϰ��ó����̵�����
	uint32_t start_ticks;			//ÿһ�κ��̵�״̬�仯ʱ��ticks
	uint32_t current_ticks;		//��ǰticksֵ
	uint32_t total_ticks;			//total_ticks = current_ticks - start_ticks
}TrafficLight;

TrafficLight g_trafficlight[4][3];	//ÿ��·����3����̵ƣ�����ǰ�У���գ��ҹ�

void trafficLightInit()
{
	memset(g_trafficlight, 0, 12 * sizeof(TrafficLight));
	uint8_t i, zero_num = 0, ret = 1;
	uint32_t ret1 = 0, status = 0;
	g_trafficlight[0][0].current_light = unknown;
	while(ret)
	{
		OSTimeDlyHMSM(0, 0, 0, 10);
		zero_num = 0;
		ret1 = HC165D1_Read() & 0x00ffffff;
		for(i = 3; i < 6/*led_num * 3*/; i++)
		{
			status = ret1 & (0x01 << i);

			if(status == 0)
			{
				zero_num++;
				//g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
				g_trafficlight[0][0].current_light = i % 3;
			}
			
		}
		
		if(g_trafficlight[0][0].current_light != unknown)
			ret = 0;
		//���̵Ʋ����ܳ���ͬʱ��������
		if(zero_num > 1)
		{
			ret = 1;
			info_msg("���̵�״̬����,���³�ʼ�� %d %x \r\n", zero_num, ret1);
		}
	}
	
		g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
		g_trafficlight[0][0].study_flag = 1;
		info_msg("���̵Ƴ�ʼ����� %d\r\n", g_trafficlight[0][0].current_light);
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
				g_trafficlight[0][0].light_period[g_trafficlight[0][0].last_light] = (g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks + 50) / 100;
				g_trafficlight[0][0].total_ticks = g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks;
				info_msg("light %d total_ticks %d\r\n", g_trafficlight[0][0].current_light, g_trafficlight[0][0].total_ticks);
			}
			g_trafficlight[0][0].start_ticks = OSTimeGet();
			//ֻ�к��̻����ڶ����������ѧϰ����
			if(g_trafficlight[0][0].light_period[red] && g_trafficlight[0][0].light_period[green] && g_trafficlight[0][0].light_period[yellow])
			{
				printf("study is ok\r\n");
				printf("red %d green %d yellow %d\r\n", g_trafficlight[0][0].light_period[red], g_trafficlight[0][0].light_period[green], g_trafficlight[0][0].light_period[yellow]);
				g_trafficlight[0][0].study_flag = 0;
			}
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

void trafficLightRead(uint8_t led_num)
{
	uint8_t i = 0, zero_num = 0;
	uint32_t ret1 = 0, ret2 = 0, status = 0;
	ret1 = HC165D1_Read() & 0x00ffffff;
	//info_msg("HC165D1 %x\r\n", ret1);
	
	
	for(i = 3; i < 6/*led_num * 3*/; i++)
	{
		status = ret1 & (0x01 << i);

		if(status == 0)
		{
			zero_num++;
			g_trafficlight[0][0].last_light = g_trafficlight[0][0].current_light;
			g_trafficlight[0][0].current_light = i % 3;
		}
		//���̵Ʋ����ܳ���ͬʱ��������
		if(zero_num > 1)
		{
			info_msg("���̵�״̬����,���³�ʼ�� %d\r\n", zero_num);
			trafficLightInit();
			return;
		}
	}
	
	ret2 = HC165D2_Read() & 0x00ffffff;
	//info_msg("HC165D2 %x\r\n", ret2);
}

//LED��˸����
void Task_LED(void *pdata)
{	
	trafficLightInit();
  while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 20);
		trafficLightRead(1);
		trafficLightStudy();
		trafficLightWork();
	}
}

extern unsigned char UDPData[5];
//UDP�������շ�����
void Task_UDP_Client(void *pdata)
{
	uint32_t current_ticks;
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
	//STMFLASH_Test();
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
		//RS485_test();
		//can1_test();
		//RTC_TimeShow();
		//RTC_DateShow();
		#if 1
		if(g_trafficlight[0][0].study_flag == 1)
			;//info_msg("����ѧϰ��\r\n");
		else
		{
			if(light_info.light == red)
				info_msg("���ʣ��%02d��\r\n", light_info.last_seconds);
			else if(light_info.light == green)
				info_msg("�̵�ʣ��%02d��\r\n", light_info.last_seconds);
			else
				info_msg("�Ƶ�ʣ��%02d��\r\n", light_info.last_seconds);
		}
		//printf("%s   ", aShowDate);
		//printf("%s\r\n", aShowTime);
		#endif
		light_info.check = 0xff;
		//sprintf(UDPData, "%d", total_ticks);
		udp_send(udp_pcb,udp_p);//��������	
	}
}

