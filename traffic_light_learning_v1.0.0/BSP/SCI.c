
#include "main.H"
#include <stdio.h>

#define USART_REC_LEN  			200
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//RS485
volatile unsigned char RS485_REC_Flag = 0;
volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//用于接收数据
volatile unsigned int RS485_rec_counter = 0;//用于RS485接收计数

//串口配置
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //管脚PA9复用为USART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_ClockInit(USART1,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 9600; //设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No; //无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure); 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);        ///////接收中断使能
	USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位
	USART_Cmd(USART1,ENABLE);//最后使能串口
	
	//USART2 初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	USART_DeInit(USART2);	
	
	USART_StructInit(&USART_InitStructure);
	USART_ClockStructInit(&USART_ClockInitStruct);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        
	GPIO_Init(GPIOD, &GPIO_InitStructure);                  
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	
	USART_ClockInit(USART2,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 9600; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No; //无校验  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); 

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);        ///////接收中断使能
	USART_ClearITPendingBit(USART2, USART_IT_TC);//清除中断TC位
	USART_Cmd(USART2,ENABLE);//使能串口
/***********************************GPIOD.7，RS485方向控制******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RS_485_RX_EN;  		//485接收使能
}




 /*发送一个字节数据*/
 void UART1SendByte(unsigned char SendData)
{	   
        USART_SendData(USART1,SendData);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	    
}  

/*接收一个字节数据*/
unsigned char UART1GetByte(unsigned char* GetData)
{   	   
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        { 
					LED1_OFF;
					return 0;//没有收到数据 
		}
        *GetData = USART_ReceiveData(USART1); 

        return 1;//收到数据
}

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--) ;
} 
/*接收一个数据，马上返回接收到的这个数据*/
char send_buf[] = "\r\n您发送的消息为:\r\n";
//char send_buf[] = {0x0D,0x0A,0xC4,0xFA,0xB7,0xA2,0xCB,0xCD,0xB5,0xC4,0xCF,0xFB,0xCF,0xA2,0xCE,0xAA,0x3A,0x0D,0x0A};
void UART1Test(void)
{
		uint8_t len;
		uint8_t t;
		
    if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);         //向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		}
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();  
//#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
  } 
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
//#endif
} 

//串口接收中断
void USART2_IRQHandler(void)  
{
	OSIntEnter(); 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到了数据
	{	
		RS485_buff[RS485_rec_counter] = USART2->DR;//
		RS485_rec_counter ++;
/********以RS485_END_FLAG1和RS485_END_FLAG2定义的字符作为一帧数据的结束标识************/
		if(RS485_rec_counter >= 2)	//只有接收到2个数据以上才做判断
		{
			if(RS485_buff[RS485_rec_counter - 2] == RS485_END_FLAG1 && RS485_buff[RS485_rec_counter - 1] == RS485_END_FLAG2) 	//帧起始标志   
			{
				RS485_REC_Flag = 1;
			}
		}
		if(RS485_rec_counter > RS485_REC_BUFF_SIZE)//超过接收缓冲区大小
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

//485数据发送
void RS485_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	RS_485_TX_EN;		//485发送使能
	RS485_Delay(300);		//稍作延时，注意延时的长短根据波特率来定，波特率越小，延时应该越长
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0);	
	}	
	RS485_Delay(200); 		//稍作延时，注意延时的长短根据波特率来定，波特率越小，延时应该越长
	RS_485_RX_EN;  		//485接收使能
}

//485收发延时函数
static void RS485_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}

void RS485_test(void)
{
	if(RS485_REC_Flag == 1)	   //如果串口接收到一帧数据（以“?;”结尾）
	{
		RS485_REC_Flag = 0;
		RS485_Send_Data(RS485_buff,RS485_rec_counter);
		RS485_rec_counter = 0;
	}
}









