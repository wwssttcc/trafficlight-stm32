//====================================================================================================//
//  xxx.h
//
//  文件描述：      
//  硬件环境：  	
//  软件环境：  	
//  内容描述：
//
//
//  修改历史：  
//
//
//====================================================================================================//


#ifndef _DRV_TRAFFICLIGHT_H_
#define _DRV_TRAFFICLIGHT_H_

#define HC165D1_SDO_PORT    	GPIOD           //串行数据输出
#define HC165D1_SDO_PIN     	GPIO_Pin_10      //串行数据输出

#define HC165D1_CE_PORT    	GPIOD           //串行片选
#define HC165D1_CE_PIN     	GPIO_Pin_11      //串行片选

#define HC165D1_PL_PORT    	GPIOD           //并行时钟
#define HC165D1_PL_PIN     	GPIO_PIN_12      //并行时钟

#define HC165D1_SCLK_PORT    	GPIOD           //时钟
#define HC165D1_SCLK_PIN     	GPIO_Pin_13      //时钟

#define HC165D2_SDO_PORT    	GPIOC           //串行数据输出
#define HC165D2_SDO_PIN     	GPIO_Pin_6      //串行数据输出

#define HC165D2_CE_PORT    	GPIOC           //串行数据输出
#define HC165D2_CE_PIN     	GPIO_Pin_7      //串行数据输出

#define HC165D2_PL_PORT    	GPIOC           //并行数据加载
#define HC165D2_PL_PIN     	GPIO_PIN_8      //并行数据加载

#define HC165D2_SCLK_PORT    	GPIOC           //时钟输入
#define HC165D2_SCLK_PIN     	GPIO_Pin_9      //时钟输入




#if 1	// 使用固件库函数

	//第一组红绿灯
	#define HC165D1_CE_1()	        GPIO_SetBits(HC165D1_CE_PORT, HC165D1_CE_PIN)
	#define HC165D1_CE_0()	        GPIO_ResetBits(HC165D1_CE_PORT, HC165D1_CE_PIN)

	#define HC165D1_PL_1()	        GPIO_SetBits(HC165D1_PL_PORT, HC165D1_PL_PIN)
	#define HC165D1_PL_0()	        GPIO_ResetBits(HC165D1_PL_PORT, HC165D1_PL_PIN)

	#define HC165D1_SCLK_1() 	GPIO_SetBits(HC165D1_SCLK_PORT, HC165D1_SCLK_PIN)
	#define HC165D1_SCLK_0() 	GPIO_ResetBits(HC165D1_SCLK_PORT, HC165D1_SCLK_PIN)

	#define HC165D1_RD_SDO() 	GPIO_ReadInputDataBit(HC165D1_SDO_PORT,HC165D1_SDO_PIN)
	
	//第二组红绿灯
	#define HC165D2_CE_1()	        GPIO_SetBits(HC165D2_CE_PORT, HC165D1_CE_PIN)
	#define HC165D2_CE_0()	        GPIO_ResetBits(HC165D2_CE_PORT, HC165D1_CE_PIN)

	#define HC165D2_PL_1()	        GPIO_SetBits(HC165D2_PL_PORT, HC165D2_PL_PIN)
	#define HC165D2_PL_0()	        GPIO_ResetBits(HC165D2_PL_PORT, HC165D2_PL_PIN)

	#define HC165D2_SCLK_1() 	GPIO_SetBits(HC165D2_SCLK_PORT, HC165D2_SCLK_PIN)
	#define HC165D2_SCLK_0() 	GPIO_ResetBits(HC165D2_SCLK_PORT, HC165D2_SCLK_PIN)

	#define HC165D2_RD_SDO() 	GPIO_ReadInputDataBit(HC165D2_SDO_PORT,HC165D2_SDO_PIN)

#else	//　直接操作寄存器，一次翻转约 125ns; 一个完整的SCK 实测 1.56us　     ->16M
	#define HC165D_PL_1()           HC165D_PL_PORT->ODR |= HC165D_PL_PIN
	#define HC165D_PL_0()           HC165D_PL_PORT->ODR &= (uint8_t)(~HC165D_PL_PIN)

	#define HC165D_SCLK_1()         HC165D_SCLK_PORT->ODR |= HC165D_SCLK_PIN
	#define HC165D_SCLK_0() 	HC165D_SCLK_PORT->ODR &= (uint8_t)(~HC165D_SCLK_PIN)

	#define HC165D_RD_SDO() 	((BitStatus)(HC165D_SDO_PORT->IDR & (uint8_t)HC165D_SDO_PIN))


#endif









void HC165D_Config(void);
uint8_t HC165D_RD_Byte(void);     

#endif

