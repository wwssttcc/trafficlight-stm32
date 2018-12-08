/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
 
#ifndef _MAIN_H_
#define _MAIN_H_


//添加必要的头文件
#include "stm32f4xx.h" 
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include "lwip/UDP.h"
#include "netconf.H"
#include "M_Type_P.h"
#include "M_CMSIS_P.h" 
#include "M_Global.h"
#include "Task_Startup.h"
#include "Bsp_Global.h"
#include "Task_Test.h"
#include "led.h"   
#include "UDP_CLIENT.h"
#include "NVIC.h"
#include "SCI.H"
#include "CAN.H"
#include "drv_trafficlight.h"

extern int tafficlight_dbg_level;

#define dprintf(level, fmt, arg...)     if (tafficlight_dbg_level >= level) \
        printf("[DEBUG]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define err_msg(fmt, arg...) do { if (tafficlight_dbg_level >= 1)		\
	printf("[ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
	printf("[ERR]\t" fmt, ## arg);	\
	} while (0)
#define info_msg(fmt, arg...) do { if (tafficlight_dbg_level >= 1)		\
	printf("[INFO]\t " fmt, ## arg); else \
	printf("[INFO]\t" fmt, ## arg);	\
	} while (0)
#define warn_msg(fmt, arg...) do { if (tafficlight_dbg_level >= 1)		\
	printf("[WARN]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
	printf("[WARN]\t" fmt, ## arg);	\
	} while (0)

//ET Module版本号
#define M_VERSION	100


//1单片机环境 

#define M_DEV_MCU   1
 #define RMII_MODE  1

//全局初始化函数声明
void M_Global_init(void);

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

typedef struct LightInfo{
	uint16_t	head;
	uint8_t		code1;
	uint8_t		code2;
	uint8_t		code3;
	uint8_t		len;
	Direction direct;
	Toward		to;
	LightType	light;
	uint8_t		last_seconds;
	uint8_t		reserve1;
	uint16_t	reserver2;
	uint8_t		check;
	uint16_t	tail;
}LightInfo;
//////////////////////////////////////////////////////////////////////////
/////////////////////////以下包含不同模块的头文件/////////////////////////
//////////////////////////////////////////////////////////////////////////

//软延时函数
#include "M_Delay_P.h"

void Delay(uint32_t nCount);


#endif
