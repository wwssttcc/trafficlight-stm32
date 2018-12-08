
#include "main.H"
#include <stdio.h>

#define USART_REC_LEN  			200
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//RS485
volatile unsigned char RS485_REC_Flag = 0;
volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//���ڽ�������
volatile unsigned int RS485_rec_counter = 0;//����RS485���ռ���

//��������
void USART_Configuration(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	USART_DeInit(USART1);
	
	USART_StructInit(&USART_InitStructure);
	USART_ClockStructInit(&USART_ClockInitStruct);	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //�ܽ�PA9����ΪUSART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_ClockInit(USART1,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 9600; //���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No; //��У��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure); 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);        ///////�����ж�ʹ��
	USART_ClearITPendingBit(USART1, USART_IT_TC);//����ж�TCλ
	USART_Cmd(USART1,ENABLE);//���ʹ�ܴ���
	
	//USART2 ��ʼ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	USART_DeInit(USART2);	
	
	USART_StructInit(&USART_InitStructure);
	USART_ClockStructInit(&USART_ClockInitStruct);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        
	GPIO_Init(GPIOD, &GPIO_InitStructure);                  
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	
	USART_ClockInit(USART2,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 9600; //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No; //��У��  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); 

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);        ///////�����ж�ʹ��
	USART_ClearITPendingBit(USART2, USART_IT_TC);//����ж�TCλ
	USART_Cmd(USART2,ENABLE);//ʹ�ܴ���
/***********************************GPIOD.7��RS485�������******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RS_485_RX_EN;  		//485����ʹ��
}




 /*����һ���ֽ�����*/
 void UART1SendByte(unsigned char SendData)
{	   
        USART_SendData(USART1,SendData);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	    
}  

/*����һ���ֽ�����*/
unsigned char UART1GetByte(unsigned char* GetData)
{   	   
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        { 
					LED1_OFF;
					return 0;//û���յ����� 
		}
        *GetData = USART_ReceiveData(USART1); 

        return 1;//�յ�����
}

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--) ;
} 
/*����һ�����ݣ����Ϸ��ؽ��յ����������*/
char send_buf[] = "\r\n�����͵���ϢΪ:\r\n";
//char send_buf[] = {0x0D,0x0A,0xC4,0xFA,0xB7,0xA2,0xCB,0xCD,0xB5,0xC4,0xCF,0xFB,0xCF,0xA2,0xCE,0xAA,0x3A,0x0D,0x0A};
void UART1Test(void)
{
		uint8_t len;
		uint8_t t;
		
    if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);         //�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
		}
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();  
//#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
//#endif
} 

//���ڽ����ж�
void USART2_IRQHandler(void)  
{
	OSIntEnter(); 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�������
	{	
		RS485_buff[RS485_rec_counter] = USART2->DR;//
		RS485_rec_counter ++;
/********��RS485_END_FLAG1��RS485_END_FLAG2������ַ���Ϊһ֡���ݵĽ�����ʶ************/
		if(RS485_rec_counter >= 2)	//ֻ�н��յ�2���������ϲ����ж�
		{
			if(RS485_buff[RS485_rec_counter - 2] == RS485_END_FLAG1 && RS485_buff[RS485_rec_counter - 1] == RS485_END_FLAG2) 	//֡��ʼ��־   
			{
				RS485_REC_Flag = 1;
			}
		}
		if(RS485_rec_counter > RS485_REC_BUFF_SIZE)//�������ջ�������С
		{
			RS485_rec_counter = 0;
		}
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART2, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
    }
	OSIntExit(); 
}

//485���ݷ���
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	RS_485_TX_EN;		//485����ʹ��
	RS485_Delay(300);		//������ʱ��ע����ʱ�ĳ��̸��ݲ�����������������ԽС����ʱӦ��Խ��
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0);	
	}	
	RS485_Delay(200); 		//������ʱ��ע����ʱ�ĳ��̸��ݲ�����������������ԽС����ʱӦ��Խ��
	RS_485_RX_EN;  		//485����ʹ��
}

//485�շ���ʱ����
static void RS485_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}

void RS485_test(void)
{
	if(RS485_REC_Flag == 1)	   //������ڽ��յ�һ֡���ݣ��ԡ�?;����β��
	{
		RS485_REC_Flag = 0;
		RS485_Send_Data(RS485_buff,RS485_rec_counter);
		RS485_rec_counter = 0;
	}
}









