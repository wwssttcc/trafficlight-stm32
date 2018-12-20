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
	LightType last_light;			//ǰһ�κ��̵�״̬
	LightType current_light;	//��ǰ���̵�״̬
	uint8_t	last_seconds[3];			//��ǰ��ʣ��ʱ��
	uint8_t study_flag;				//ѧϰ��־
	uint32_t study_num;				//ѧϰ����
	uint8_t light_period[4];	//ѧϰ��ó����̵�����
	uint8_t status;						//���̵ƴ���״̬��0����ʼ״̬��1��������2��ͬʱ���ֶ��������3������
	uint8_t error_num;				//�������ֶ���ƴ���
	uint32_t start_ticks;			//ÿһ�κ��̵�״̬�仯ʱ��ticks
	uint32_t current_ticks;		//��ǰticksֵ
	uint32_t total_ticks;			//total_ticks = current_ticks - start_ticks
	uint8_t current_seconds;
	LightType last_light_sec;			//ǰһ�κ��̵�״̬
	LightType current_light_sec;	//��ǰ���̵�״̬
}TrafficLight;

typedef struct LightInfo{
	uint8_t	head1;		//0xA5
	uint8_t	head2;		//0x5A
	uint8_t	code1;		//0x83
	uint8_t	code2;		//0xA0
	uint8_t	code3;		//0x00
	uint8_t	len;			//����
	Direction direct;	//�����ϱ�:0123	
	Toward	to;				//��ֱ��:123			
	LightType	light;		//��ǰ��״̬ �ַ�RGY
	uint8_t	last_seconds;	//��ǰ��ʣ��ʱ��
	uint8_t	next_light;		//�¸���״̬
	uint8_t	next_light_period;	//�¸���״̬����
	uint8_t	red_period;
	uint8_t green_period;
	uint8_t yellow_period;
	uint8_t	check;		//У����
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

