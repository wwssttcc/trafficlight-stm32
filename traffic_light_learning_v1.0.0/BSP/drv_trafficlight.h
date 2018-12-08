//====================================================================================================//
//  xxx.h
//
//  �ļ�������      
//  Ӳ��������  	
//  ���������  	
//  ����������
//
//
//  �޸���ʷ��  
//
//
//====================================================================================================//


#ifndef _DRV_TRAFFICLIGHT_H_
#define _DRV_TRAFFICLIGHT_H_

#define HC165D1_SDO_PORT    	GPIOD           //�����������
#define HC165D1_SDO_PIN     	GPIO_Pin_10      //�����������

#define HC165D1_CE_PORT    	GPIOD           //����Ƭѡ
#define HC165D1_CE_PIN     	GPIO_Pin_11      //����Ƭѡ

#define HC165D1_PL_PORT    	GPIOD           //����ʱ��
#define HC165D1_PL_PIN     	GPIO_PIN_12      //����ʱ��

#define HC165D1_SCLK_PORT    	GPIOD           //ʱ��
#define HC165D1_SCLK_PIN     	GPIO_Pin_13      //ʱ��

#define HC165D2_SDO_PORT    	GPIOC           //�����������
#define HC165D2_SDO_PIN     	GPIO_Pin_6      //�����������

#define HC165D2_CE_PORT    	GPIOC           //�����������
#define HC165D2_CE_PIN     	GPIO_Pin_7      //�����������

#define HC165D2_PL_PORT    	GPIOC           //�������ݼ���
#define HC165D2_PL_PIN     	GPIO_PIN_8      //�������ݼ���

#define HC165D2_SCLK_PORT    	GPIOC           //ʱ������
#define HC165D2_SCLK_PIN     	GPIO_Pin_9      //ʱ������




#if 1	// ʹ�ù̼��⺯��

	//��һ����̵�
	#define HC165D1_CE_1()	        GPIO_SetBits(HC165D1_CE_PORT, HC165D1_CE_PIN)
	#define HC165D1_CE_0()	        GPIO_ResetBits(HC165D1_CE_PORT, HC165D1_CE_PIN)

	#define HC165D1_PL_1()	        GPIO_SetBits(HC165D1_PL_PORT, HC165D1_PL_PIN)
	#define HC165D1_PL_0()	        GPIO_ResetBits(HC165D1_PL_PORT, HC165D1_PL_PIN)

	#define HC165D1_SCLK_1() 	GPIO_SetBits(HC165D1_SCLK_PORT, HC165D1_SCLK_PIN)
	#define HC165D1_SCLK_0() 	GPIO_ResetBits(HC165D1_SCLK_PORT, HC165D1_SCLK_PIN)

	#define HC165D1_RD_SDO() 	GPIO_ReadInputDataBit(HC165D1_SDO_PORT,HC165D1_SDO_PIN)
	
	//�ڶ�����̵�
	#define HC165D2_CE_1()	        GPIO_SetBits(HC165D2_CE_PORT, HC165D1_CE_PIN)
	#define HC165D2_CE_0()	        GPIO_ResetBits(HC165D2_CE_PORT, HC165D1_CE_PIN)

	#define HC165D2_PL_1()	        GPIO_SetBits(HC165D2_PL_PORT, HC165D2_PL_PIN)
	#define HC165D2_PL_0()	        GPIO_ResetBits(HC165D2_PL_PORT, HC165D2_PL_PIN)

	#define HC165D2_SCLK_1() 	GPIO_SetBits(HC165D2_SCLK_PORT, HC165D2_SCLK_PIN)
	#define HC165D2_SCLK_0() 	GPIO_ResetBits(HC165D2_SCLK_PORT, HC165D2_SCLK_PIN)

	#define HC165D2_RD_SDO() 	GPIO_ReadInputDataBit(HC165D2_SDO_PORT,HC165D2_SDO_PIN)

#else	//��ֱ�Ӳ����Ĵ�����һ�η�תԼ 125ns; һ��������SCK ʵ�� 1.56us��     ->16M
	#define HC165D_PL_1()           HC165D_PL_PORT->ODR |= HC165D_PL_PIN
	#define HC165D_PL_0()           HC165D_PL_PORT->ODR &= (uint8_t)(~HC165D_PL_PIN)

	#define HC165D_SCLK_1()         HC165D_SCLK_PORT->ODR |= HC165D_SCLK_PIN
	#define HC165D_SCLK_0() 	HC165D_SCLK_PORT->ODR &= (uint8_t)(~HC165D_SCLK_PIN)

	#define HC165D_RD_SDO() 	((BitStatus)(HC165D_SDO_PORT->IDR & (uint8_t)HC165D_SDO_PIN))


#endif









void HC165D_Config(void);
uint8_t HC165D_RD_Byte(void);     

#endif

