/***********************************************************************
�ļ����ƣ�main.C
��    �ܣ�UDP�ͻ���ģʽ�շ����ԣ�����UC0S II����ϵͳ
ʵ��ƽ̨������STM32F407VET6 ������
��汾  ��V1.0.0 
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
	SCB->VTOR = FLASH_BASE | 0x10000;//����ƫ����
	#endif
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	#ifdef TRAFFICLIGHT_UPGRADE_FLAG
	data.update_flag = 0;
  STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	#endif
	#ifdef TRAFFICLIGHT_IAP
	if(data.update_flag == 1)
		iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
	#endif
	//��ʼ������
	BSP_Init();
	if(data.update_flag == 0)
		info_msg("TRAFFICLIGHT_IAP\r\n");
	else
		info_msg("TRAFFICLIGHT_APP\r\n");
	#if 1
	//��ʼ��ϵͳ
	OSInit();
	//������������
	OSTaskCreate(	Task_StartUp,               		    				//ָ����������ָ��
                  	(void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
					(OS_STK *)&Stk_TaskStartUp[TASK_STARTUP_STK_SIZE - 1],	//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
					(INT8U) OS_USER_PRIO_LOWEST);							//�������������ȼ�  

	//���ļ�������0  
	OSTimeSet(0);
	
	//����UCOS-II�ں�
	OSStart();
	#endif					
	return 0;
}
//���������ջ
OS_STK Stk_TaskStartUp[TASK_STARTUP_STK_SIZE];

void Task_StartUp(void *pdata)
{

	//��ʼ��UCOSʱ��
	//OS_TICKS_PER_SEC Ϊ UCOS-II ÿ�������
	SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC - 1);
	//���ȼ�˵����ʹ��OS_USER_PRIO_GET(n)������ȡ
	//OS_USER_PRIO_GET(0)���,OS_USER_PRIO_GET(1)��֮����������
	//OS_USER_PRIO_GET(0)����ߵ����ȼ�����Ҫ�����ڴ������������Ҫ�����ȴ������������Ϊ���������ȼ�  

	//OSTaskCreate(	Task_Xxx,               		    					//ָ����������ָ��
    //              (void *)0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
	//				(OS_STK *)&Stk_TaskXxx[TASK_XXX_STK_SIZE - 1],			//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
	//				(INT8U) OS_USER_PRIO_GET(N));							//�������������ȼ�  
			
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
