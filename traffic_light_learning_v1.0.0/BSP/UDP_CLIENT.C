#include "main.h"

//const static unsigned char UDPData[]="UDP客户端实验例程!\r\n";
unsigned char UDPData[]="红灯剩余10秒\r\n";
//unsigned char UDPData[5];
struct udp_pcb *udp_pcb;
struct ip_addr ipaddr;
struct pbuf *udp_p;
/***********************************************************************
函数名称：void UDP_client_init(void)
功    能：完成udp客户端初始化
***********************************************************************/
void UDP_client_init(void)
{
	udp_p = pbuf_alloc(PBUF_RAW,sizeof(LightInfo),PBUF_RAM);
	//udp_p = pbuf_alloc(PBUF_RAW,sizeof(UDPData),PBUF_RAM);
	udp_p -> payload = (void *)UDPData;
	My_IP4_ADDR(&ipaddr,UDP_UDP_REMOTE_IP);				//远端IP
	udp_pcb = udp_new();
	udp_bind(udp_pcb,IP_ADDR_ANY,UDP_CLIENT_PORT); 	/* 绑定本地IP地址 */
	udp_connect(udp_pcb,&ipaddr,UDP_REMOTE_PORT); 		/* 连接远程主机 */		
}
/***********************************************************************
函数名称：My_IP4_ADDR(void)
功    能：IP地址的装配
***********************************************************************/
void My_IP4_ADDR(struct ip_addr *ipaddr,unsigned char a,unsigned char b,unsigned char c,unsigned char d)
{
	ipaddr->addr = htonl(((u32_t)((a) & 0xff) << 24) | \
                               ((u32_t)((b) & 0xff) << 16) | \
                               ((u32_t)((c) & 0xff) << 8) | \
                                (u32_t)((d) & 0xff));
}
/***********************************************************************
函数名称：void UDP_client_init(void)
功    能：完成udp客户端初始化
***********************************************************************/
void UDP_Send_Data(struct udp_pcb *pcb,struct pbuf *p)
{
	udp_send(pcb,p);
	UDP_Delay(0XFFFFF);//延时，不能发送太快	
}

/***********************************************************************
函数名称：Delay(unsigned long ulVal)
功    能：利用循环产生一定的延时
***********************************************************************/
static void UDP_Delay(unsigned long ulVal) /* 利用循环产生一定的延时 */
{
	while ( --ulVal != 0 );
}
