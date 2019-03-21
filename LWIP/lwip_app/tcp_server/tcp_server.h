#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//NETCONN API��̷�ʽ��TCP���������Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

 
#define TCP_SERVER_RX_BUFSIZE	1500		//����tcp server���������ݳ���
#define TCP_SERVER_TX_BUFSIZE	300		//����tcp server���������ݳ���

#define TCP_SERVER_PORT			8899	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_server_sendbuf[TCP_SERVER_TX_BUFSIZE];
extern u8 tcp_server_flag;			//TCP���������ݷ��ͱ�־λ
extern u32 g_network_count;

INT8U tcp_server_init(void);		//TCP��������ʼ��(����TCP�������߳�)
#endif

