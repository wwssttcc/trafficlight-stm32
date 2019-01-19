/***********************************************************************
�ļ����ƣ�
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
#include "main.h"

#ifdef USE_RTC
void Set_Time(void)
{
	/* ���������պ����� */
	RTC_DateStructure.RTC_Year = 0x17;
	RTC_DateStructure.RTC_Month = 0x01;
	RTC_DateStructure.RTC_Date = 0x09;
	RTC_DateStructure.RTC_WeekDay = 0x02;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);	
	
	/* ����ʱ���룬�Լ���ʾ��ʽ */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x10;
	RTC_TimeStructure.RTC_Minutes = 0x22;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
}
#endif
/*
 * ���ܣ�
 *       �弶��ʼ��
 * ������
 *       ��
 * ���أ�
 *       ��
 * ˵����
 *       ������ģ���г�ʼ���ľ���ʹ��ģ���ʼ��
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
 	Set_Time();//����ʱ��
	#endif
}

