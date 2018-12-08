//====================================================================================================//
//  xxx.c
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


#include "main.h"


//00��0x00
//01��0x08
//02��0x04
//03��0x0C
//04��0x02
//05��0x0A
//06��0x06
//07��0x0E
//08��0x01
//09��0x09

//11��0x88
//22��0x44
//33��0xCC
//44��0x22
//55��0xAA
//66��0x66
//77��0xEE
//88��0x11
//99��0x99


void HC165D_Config(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); 		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12| GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	HC165D1_CE_0();
	HC165D1_PL_0();
	HC165D1_SCLK_0();
	
	HC165D2_CE_0();
	HC165D2_PL_0();
	HC165D2_SCLK_0();
}


uint32_t HC165D1_Read(void)     
{
     uint8_t i;
     uint32_t Temp;
     HC165D1_PL_0();        //HC165D���ز�������
     HC165D1_PL_1();        //HC165D���沢������
     for(i = 0;i<24;i++)
     {    
         HC165D1_SCLK_0();  
         Temp <<= 1;    //temp����һλ
         if(HC165D1_RD_SDO()) //��Ϊ��������bitstaus�Ǹ�ö�ٱ�����ֻ��0�ͣ�0����ֵ 
         {
             Temp|=0x01;
         }
         HC165D1_SCLK_1(); 
     }
     //UART1_SendChar(Temp);
     return(Temp);
}

uint32_t HC165D2_Read(void)     
{
     uint8_t i;
     uint32_t Temp;
     HC165D2_PL_0();        //HC165D���ز�������
     HC165D2_PL_1();        //HC165D���沢������
     for(i = 0;i<24;i++)
     {    
         HC165D2_SCLK_0();  
         Temp <<= 1;    //temp����һλ
         if(HC165D2_RD_SDO()) //��Ϊ��������bitstaus�Ǹ�ö�ٱ�����ֻ��0�ͣ�0����ֵ 
         {
             Temp|=0x01;
         }
         HC165D2_SCLK_1(); 
     }
     //UART1_SendChar(Temp);
     return(Temp);
}


