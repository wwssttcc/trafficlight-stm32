#ifndef _TRAFFICLIGHT_H_
#define _TRAFFICLIGHT_H_
#include "main.h"

typedef enum LightType{
	red = 0,
	green,
	yellow,
	unknown,
}LightType;

typedef enum Direction{
	east = 0,
	south,
	west,
	north
}Direction;

typedef enum Toward{
	front = 0,
	left,
	right
}Toward;

typedef struct TrafficLightPos{
	Direction dir;
	Toward	to;
}TrafficLightPos;

typedef struct TrafficLight{
	LightType last_light;			//前一次红绿灯状态
	LightType current_light;	//当前红绿灯状态
	uint8_t	last_seconds[3];			//当前灯剩余时间
	uint8_t study_flag;				//学习标志
	uint32_t study_num;				//学习次数
	uint8_t light_period[4];	//学习后得出红绿灯周期
	uint8_t status;						//红绿灯错误状态，0：初始状态，1：正常，2：同时出现多个灯亮，3：常亮
	uint8_t error_num;				//连续出现多个灯次数
	uint32_t start_ticks;			//每一次红绿灯状态变化时的ticks
	uint32_t current_ticks;		//当前ticks值
	uint32_t total_ticks;			//total_ticks = current_ticks - start_ticks
	uint8_t current_seconds;
	LightType last_light_sec;			//前一次红绿灯状态
	LightType current_light_sec;	//当前红绿灯状态
}TrafficLight;

typedef struct LightInfo{
	uint8_t	head1;		//0xA5
	uint8_t	head2;		//0x5A
	uint8_t	code1;		//0x83
	uint8_t	code2;		//0xA0
	uint8_t	code3;		//0x00
	uint8_t	len;			//长度
	Direction direct;	//东西南北:0123	
	Toward	to;				//左直右:123			
	LightType	light;		//当前灯状态 字符RGY
	uint8_t	last_seconds;	//当前灯剩余时间
	uint8_t	next_light;		//下个灯状态
	uint8_t	next_light_period;	//下个灯状态周期
	uint8_t	red_period;
	uint8_t green_period;
	uint8_t yellow_period;
	uint8_t	check;		//校验码
	uint8_t	tail1;		//0x5A
	uint8_t	tail2;		//0xA5
}LightInfo;

extern TrafficLight g_trafficlight[4][3];
extern uint8_t g_online_num;

void trafficLightInit(void);
void trafficLightStudy(void);
void trafficLightWork(void);
void trafficLightRead(void);
int trafficLightStatus(LightInfo *light_info, uint8_t num);
#endif

