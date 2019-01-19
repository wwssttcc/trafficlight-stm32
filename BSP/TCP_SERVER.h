/********************************************************************
文件名称：TCP_SERVER.h
功    能：
编写时间：2013.4.25
编 写 人：
注    意：
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
	ES_TCPSERVER_NONE = 0,		//没有连接
	ES_TCPSERVER_ACCEPTED,		//有客户端连接上了 
	ES_TCPSERVER_CLOSING,		//即将关闭连接
}; 

//LWIP回调函数使用的结构体
struct tcp_server_struct
{
	uint8_t state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
}; 

uint8_t tcp_server_init(void);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
#endif
