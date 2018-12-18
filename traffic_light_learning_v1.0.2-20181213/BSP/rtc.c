#include "main.h"	

//#define RTC_Debug   /* ����ѡ�����ģʽ */
/* ѡ��RTC��ʱ��Դ */
//#define RTC_CLOCK_SOURCE_LSE       // LSE ���ⲿ����ʱ�ӣ�Ƶ��Ϊ32.768KHZ ��Ҫ����RTCʱ��
#define RTC_CLOCK_SOURCE_LSI     // LSI ���ڲ�����ʱ�ӣ�Ƶ��Ϊ32KHZ
RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef  RTC_DateStructure;


__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
uint8_t aShowTime[50] = {0};
uint8_t aShowDate[50] = {0};

	
//RTC����
void RTC_Config(void)
{
	/* ʹ��PWRʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* �������RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* ѡ��LSI��Ϊʱ��Դ */
#if defined (RTC_CLOCK_SOURCE_LSI)  
	
	/* Enable the LSI OSC */ 
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
	
	/* ѡ��LSE��ΪRTCʱ�� */
#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* ʹ��LSE����  */
	RCC_LSEConfig(RCC_LSE_ON);

	/* �ȴ����� */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* ѡ��RTCʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;

#else
#error Please select the RTC Clock source inside the main.c file
#endif 

	/* ʹ��RTCʱ�� */
	RCC_RTCCLKCmd(ENABLE);

	/* �ȴ�RTC APB�Ĵ���ͬ�� */
	RTC_WaitForSynchro();

	/* ����RTC���ݼĴ����ͷ�Ƶ��  */
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* ʹ�� RTC Alarm A �ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* ʹ������ */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	/* ���RTC���ӱ�־ */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

//��ʾʱ��
void RTC_TimeShow(void)
{

	/* �õ���ǰʱ����ʱ�� */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	/* ʱ����ʾ��ʽ : hh:mm:ss */
	sprintf((char*)aShowTime,"%0.2d:%0.2d:%0.2d",RTC_TimeStructure.RTC_Hours, 
	                                           RTC_TimeStructure.RTC_Minutes, 
	                                           RTC_TimeStructure.RTC_Seconds);
}

//��ʾ����
void RTC_DateShow(void)
{
	/* �õ���ǰʱ����ʱ�� */
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	/* ʱ����ʾ��ʽ : hh:mm:ss */
	sprintf((char*)aShowDate," ��ǰʱ���ǣ�20%0.2d��%0.2d��%0.2d�յ�%0.1d��",RTC_DateStructure.RTC_Year, 
	                                                         RTC_DateStructure.RTC_Month, 
	                                                          RTC_DateStructure.RTC_Date,
											               RTC_DateStructure.RTC_WeekDay);
}

//��ʾ���õ�����ʱ��
void RTC_AlarmShow(void)
{	
	uint8_t showalarm[50] = {0};

	/* ���õ�ǰ���� */
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	sprintf((char*)showalarm,"%0.2d:%0.2d:%0.2d", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}
