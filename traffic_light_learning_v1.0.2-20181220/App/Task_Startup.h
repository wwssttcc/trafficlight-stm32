/***********************************************************************
�ļ����ƣ�
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
#ifndef _APP_TASK_STARTUP_H_
#define _APP_TASK_STARTUP_H_


//���������ջ��С
#define TASK_STARTUP_STK_SIZE  100


extern OS_STK Stk_TaskStartUp[TASK_STARTUP_STK_SIZE];

void Task_StartUp(void *pdata);


#endif
