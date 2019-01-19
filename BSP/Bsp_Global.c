/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

#ifdef USE_RTC
void Set_Time(void)
{
	/* 设置年月日和星期 */
	RTC_DateStructure.RTC_Year = 0x17;
	RTC_DateStructure.RTC_Month = 0x01;
	RTC_DateStructure.RTC_Date = 0x09;
	RTC_DateStructure.RTC_WeekDay = 0x02;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);	
	
	/* 设置时分秒，以及显示格式 */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x10;
	RTC_TimeStructure.RTC_Minutes = 0x22;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
}
#endif
/*
 * 功能：
 *       板级初始化
 * 参数：
 *       无
 * 返回：
 *       无
 * 说明：
 *       可用与模块中初始化的尽量使用模块初始化
 */
void BSP_Init(void)
{
	LED_Configuration();
	NVIC_Configuration();
	//CAN1_Configuration();
	USART_Configuration();
	HC165D_Config();
	#ifdef USE_RTC
	RTC_Config();
 	Set_Time();//调整时间
	#endif
}

