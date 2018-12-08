#include "main.h"  

OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];


typedef struct TrafficLight{
	LightType last_light;			//前一次红绿灯状态
	LightType current_light;	//当前红绿灯状态
	uint8_t	last_seconds;			//当前灯剩余时间
	uint8_t study_flag;				//学习标志
	uint8_t light_period[3];	//学习后得出红绿灯周期
	uint32_t start_ticks;			//每一次红绿灯状态变化时的ticks
	uint32_t current_ticks;		//当前ticks值
	uint32_t total_ticks;			//total_ticks = current_ticks - start_ticks
}TrafficLight;

TrafficLight g_trafficlight[4][3];	//每个路口有3组红绿灯，代表前行，左拐，右拐

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
		//若上次红绿灯状态与当前红绿灯状态不一致，则认为状态发生改变
		if(g_trafficlight[0][0].last_light != g_trafficlight[0][0].current_light)
		{
			g_trafficlight[0][0].current_ticks = OSTimeGet();
			//测得正常状态下的红绿的start_ticks不能为0
			if(g_trafficlight[0][0].start_ticks != 0)
			{
				//计算出当前红绿灯周期
				g_trafficlight[0][0].light_period[g_trafficlight[0][0].last_light] = (g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks) / 100;
				g_trafficlight[0][0].total_ticks = g_trafficlight[0][0].current_ticks - g_trafficlight[0][0].start_ticks;
			}
			g_trafficlight[0][0].start_ticks = OSTimeGet();
			//只有红绿黄周期都测输出来，学习结束
			if(g_trafficlight[0][0].light_period[red] && g_trafficlight[0][0].light_period[green] && g_trafficlight[0][0].light_period[yellow])
				g_trafficlight[0][0].study_flag = 0;
		}
	}
}

void trafficLightWork()
{
	//红绿灯不学习时，只需记录每个状态改变时start_ticks的值
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

//LED闪烁任务
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
//UDP服务器收发任务
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
		OSTimeDlyHMSM(0, 0, 0, 500);//挂起10ms，以便其他线程运行
		light_info.direct = south;
		light_info.to = front;
		light_info.light = g_trafficlight[0][0].current_light;
		total_ticks = OSTimeGet() - g_trafficlight[0][0].start_ticks;
		light_info.last_seconds = g_trafficlight[0][0].light_period[light_info.light] - total_ticks / 100;

		if(g_trafficlight[0][0].study_flag == 1)
			info_msg("正在学习中\r\n");
		else
		{
			if(light_info.light == red)
				info_msg("红灯剩余%02d秒\r\n", light_info.last_seconds);
			else if(light_info.light == green)
				info_msg("绿灯剩余%02d秒\r\n", light_info.last_seconds);
			else
				info_msg("黄灯剩余%02d秒\r\n", light_info.last_seconds);
		}
		light_info.check = 0xff;
		//sprintf(UDPData, "%d", total_ticks);
		udp_send(udp_pcb,udp_p);//发送数据
	}
}

