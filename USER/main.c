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

//TRAFFICLIGHT����
//�������ȼ�
#define TRAFFICLIGHT_TASK_PRIO		9
//�����ջ��С
#define TRAFFICLIGHT_STK_SIZE		256
//�����ջ
OS_STK	TRAFFICLIGHT_TASK_STK[TRAFFICLIGHT_STK_SIZE];
//������
void trafficlight_task(void *pdata);

//ͨ������
//�������ȼ�
#define COMM_TASK_PRIO	10	
//�����ջ��С
#define COMM_STK_SIZE	1500
//�����ջ
OS_STK	COMM_TASK_STK[COMM_STK_SIZE];
//������
void communication_task(void *pdata);

//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С
#define START_STK_SIZE		128
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 


int main(void)
{
	IPInfo data;
	#ifndef TRAFFICLIGHT_IAP
	SCB->VTOR = FLASH_BASE | 0x40000;//����ƫ����
	#endif
	#ifdef TRAFFICLIGHT_UPGRADE_FLAG
	data.update_flag = 0;
  STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	#endif
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	#ifdef TRAFFICLIGHT_IAP
	if(data.update_flag == 1)
	{
		iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
	}
	#endif

	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	uart_init(115200);   	//���ڲ���������
	delay_ms(200);
	
	LED_Init(); 			//LED��ʼ��
	LED0 = 0;
	HC165D_Config();

	if(data.update_flag == 1)
		printf("TRAFFICLIGHT_APP\r\n");
	else
		printf("TRAFFICLIGHT_IAP\r\n");
	
	#ifndef TRAFFICLIGHT_IAP
	printf("trafficlight app \r\n");
	#endif
	
	mymem_init(SRAMIN);  	//��ʼ���ڲ��ڴ��
	OSInit(); 					//UCOS��ʼ��
	while(lwip_comm_init()) 	//lwip��ʼ��
	{		
		delay_ms(500);
	}
	
	while(tcp_server_init()) 									//��ʼ��tcp_server(����tcp_server�߳�)
	{
		delay_ms(500);
	}

	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //����UCOS
}

//start����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//����DHCP����
#endif
	IWDG_Init(4,2500);
	OSTaskCreate(trafficlight_task,(void*)0,(OS_STK*)&TRAFFICLIGHT_TASK_STK[TRAFFICLIGHT_STK_SIZE-1],TRAFFICLIGHT_TASK_PRIO); 	//����LED����
	OSTaskCreate(communication_task,(void*)0,(OS_STK*)&COMM_TASK_STK[TRAFFICLIGHT_STK_SIZE-1],COMM_TASK_PRIO); 	//����LED����
	OSTaskSuspend(OS_PRIO_SELF); //����start_task����
	OS_EXIT_CRITICAL();  //���ж�
}

//LED��˸����
void trafficlight_task(void *pdata)
{
	uint16_t count = 0;
	IPInfo data;
	//OSTimeDlyHMSM(0, 0, 5, 0);		//���̵��ϵ�ʱ����ʱ�ȴ����̵���������
	trafficLightInit();
	printf("trafficlight_task\r\n");
  while(1)
	{
		count++;
		/* ÿ��5�������¸��º��̵����� */
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
			__set_FAULTMASK(1); //�ر������ж�
			NVIC_SystemReset(); //��λ
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








