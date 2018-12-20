#ifndef _RTC_H_
#define _RTC_H_


extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_InitTypeDef  RTC_InitStructure;
extern RTC_AlarmTypeDef RTC_AlarmStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
extern uint8_t aShowTime[50];
extern uint8_t aShowDate[50];
uint8_t USART_Scanf(uint32_t value);
void RTC_TimeShow(void);
void RTC_DateShow(void);
void RTC_Config(void);

#endif

