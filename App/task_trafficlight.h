/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#ifndef _APP_TASK_TRAFFICLIGHT_H_
#define _APP_TASK_TRAFFICLIGHT_H_

#define TASK_TEST_LED_STK_SIZE	1000
#define TASK_UDP_CLIENT_STK_SIZE	2000

extern OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
extern OS_STK Stk_Task_TRAFFIC[TASK_TEST_LED_STK_SIZE];
extern OS_STK Stk_Task_UDP_Client[TASK_UDP_CLIENT_STK_SIZE];
extern OS_STK Stk_Task_QIJUN[TASK_TEST_LED_STK_SIZE];

void Task_LED(void *pdata);
void Task_TRAFFIC(void *pdata);
void Task_UDP_Client(void *pdata);
void Task_QiJun(void *pdata);
#endif
