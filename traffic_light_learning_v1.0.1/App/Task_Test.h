/***********************************************************************
�ļ����ƣ�
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
#ifndef _APP_TASK_TEST_H_
#define _APP_TASK_TEST_H_

#define TASK_TEST_LED_STK_SIZE	400
#define TASK_UDP_CLIENT_STK_SIZE	2000

extern OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
extern OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
extern OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];

void Task_LED(void *pdata);
void Task_TRAFFIC(void *pdata);
void Task_UDP_Client(void *pdata);

#endif
