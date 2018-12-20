#ifndef _CAN_H_
#define _CAN_H_

#define CAN_BAUD_NUM    18		//可用配置波特率个数

extern unsigned char CAN1_data[8];
extern unsigned char can1_rec_flag;
extern unsigned char CAN2_data[8];
extern unsigned char can2_rec_flag;
void CAN1_Configuration(void);
void CAN2_Configuration(void);
void can1_tx(unsigned int ID,unsigned char  Data);
void can2_tx(unsigned int ID,unsigned char  Data);
void CAN1_WriteData(unsigned int ID);
void CAN_Baud_Process(unsigned int Baud,CAN_InitTypeDef *CAN_InitStructure);
#endif
