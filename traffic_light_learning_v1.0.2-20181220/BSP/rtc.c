#include "main.h"	

//#define RTC_Debug   /* 用于选择调试模式 */
/* 选择RTC的时钟源 */
//#define RTC_CLOCK_SOURCE_LSE       // LSE 是外部低速时钟，频率为32.768KHZ 主要用于RTC时钟
#define RTC_CLOCK_SOURCE_LSI     // LSI 是内部低速时钟，频率为32KHZ
RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef  RTC_DateStructure;


__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
uint8_t aShowTime[50] = {0};
uint8_t aShowDate[50] = {0};

	
//RTC配置
void RTC_Config(void)
{
	/* 使能PWR时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* 允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* 选择LSI作为时钟源 */
#if defined (RTC_CLOCK_SOURCE_LSI)  
	
	/* Enable the LSI OSC */ 
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* 选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
	
	/* 选择LSE作为RTC时钟 */
#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* 使能LSE振荡器  */
	RCC_LSEConfig(RCC_LSE_ON);

	/* 等待就绪 */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* 选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;

#else
#error Please select the RTC Clock source inside the main.c file
#endif 

	/* 使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);

	/* 等待RTC APB寄存器同步 */
	RTC_WaitForSynchro();

	/* 配置RTC数据寄存器和分频器  */
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* 使能 RTC Alarm A 中断 */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* 使能闹钟 */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	/* 清除RTC闹钟标志 */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

//显示时间
void RTC_TimeShow(void)
{

	/* 得到当前时分秒时间 */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	/* 时间显示格式 : hh:mm:ss */
	sprintf((char*)aShowTime,"%0.2d:%0.2d:%0.2d",RTC_TimeStructure.RTC_Hours, 
	                                           RTC_TimeStructure.RTC_Minutes, 
	                                           RTC_TimeStructure.RTC_Seconds);
}

//显示日期
void RTC_DateShow(void)
{
	/* 得到当前时分秒时间 */
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	/* 时间显示格式 : hh:mm:ss */
	sprintf((char*)aShowDate," 当前时间是：20%0.2d年%0.2d月%0.2d日第%0.1d周",RTC_DateStructure.RTC_Year, 
	                                                         RTC_DateStructure.RTC_Month, 
	                                                          RTC_DateStructure.RTC_Date,
											               RTC_DateStructure.RTC_WeekDay);
}

//显示设置的闹钟时间
void RTC_AlarmShow(void)
{	
	uint8_t showalarm[50] = {0};

	/* 设置当前闹钟 */
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	sprintf((char*)showalarm,"%0.2d:%0.2d:%0.2d", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, 
												RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}
