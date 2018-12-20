/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#ifndef _APP_TASK_STARTUP_H_
#define _APP_TASK_STARTUP_H_


//启动任务堆栈大小
#define TASK_STARTUP_STK_SIZE  100


extern OS_STK Stk_TaskStartUp[TASK_STARTUP_STK_SIZE];

void Task_StartUp(void *pdata);


#endif
