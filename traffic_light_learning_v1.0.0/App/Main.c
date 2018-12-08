/***********************************************************************
�ļ����ƣ�main.C
��    �ܣ�UDP�ͻ���ģʽ�շ����ԣ�����UC0S II����ϵͳ
ʵ��ƽ̨������STM32F407VET6 ������
��汾  ��V1.0.0 
***********************************************************************/
#include "main.h"

int tafficlight_dbg_level = 1;
int main(void)
{
	//��ʼ������
	BSP_Init();
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
	info_msg("���̵�ѧϰϵͳv1.0.0 ����ʱ��:%s %s\r\n", __TIME__,__DATE__);
	OSTaskCreate(Task_UDP_Client, (void *)0, &Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE-1], OS_USER_PRIO_GET(5));
	info_msg("���紫����������...\r\n");
	OSTaskCreate(Task_LED, (void *)0, &Stk_Task_LED[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(6));
	info_msg("ģ����̵���������...\r\n");
	OSTaskCreate(Task_TRAFFIC, (void *)0, &Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE-1], OS_USER_PRIO_GET(7));
	info_msg("���̵�ѧϰ��������...\r\n");
	while (1)
	{
		
		OSTimeDlyHMSM(0, 0, 1, 0);//1000ms
	}
}
