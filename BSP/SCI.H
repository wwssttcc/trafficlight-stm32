#ifndef  _SCI_H_
#define  _SCI_H_


#define RS232_REC_BUFF_SIZE				100
#define RS232_END_FLAG1	'#'			//RS232一桢数据结束标志1 
#define RS232_END_FLAG2	'*'			//RS232一桢数据结束标志2 


extern volatile unsigned char RS232_REC_Flag ;
extern volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE] ;//用于接收数据
extern volatile unsigned int RS232_rec_counter ;//用于RS232接收计数

void RS232_Send_Data(unsigned char *send_buff,unsigned int length);
void USART_Configuration(void);

#define RS_485_RX_EN 	GPIO_ResetBits(GPIOD , GPIO_Pin_4)	//接收使能
#define RS_485_TX_EN 	GPIO_SetBits(GPIOD , GPIO_Pin_4) 	//发送使能
#define RS485_REC_BUFF_SIZE				100

#define RS485_END_FLAG1	'\r'			//RS485一桢数据结束标志1 
#define RS485_END_FLAG2	'\n'			//RS485一桢数据结束标志2 

extern volatile unsigned char RS485_REC_Flag ;
extern volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE] ;//用于接收数据
extern volatile unsigned int RS485_rec_counter ;//用于RS485接收计数

void RS485_Send_Data(unsigned char *send_buff,unsigned int length);
static void RS485_Delay(uint32_t nCount);
void send_wifi(uint8_t *pbuf, uint8_t len);
 void UART3SendByte(unsigned char SendData);
#endif
