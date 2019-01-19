/********************************************************************
�ļ����ƣ�TCP_SERVER.h
��    �ܣ�
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
*********************************************************************/
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_
#include "main.h"
/* MAC ADcontinue;continue;DRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   12
#define MAC_ADDR1   45
#define MAC_ADDR2   34
#define MAC_ADDR3   59
#define MAC_ADDR4   76
#define MAC_ADDR5   116
 
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   8
#define IP_ADDR3   252
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   8
#define GW_ADDR3   1  

#define TCP_SERVER_PORT 		8899 
#define TCP_SERVER_RX_BUFSIZE 2000
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//û������
	ES_TCPSERVER_ACCEPTED,		//�пͻ����������� 
	ES_TCPSERVER_CLOSING,		//�����ر�����
}; 

//LWIP�ص�����ʹ�õĽṹ��
struct tcp_server_struct
{
	uint8_t state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
}; 

uint8_t tcp_server_init(void);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
#endif
