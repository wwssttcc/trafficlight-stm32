#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_


/* MAC ADcontinue;continue;DRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0
 
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
#define GW_ADDR2   1
#define GW_ADDR3   1  

#define UDP_CLIENT_PORT			1030
#define UDP_REMOTE_PORT			1031
#define UDP_UDP_REMOTE_IP		192,168,8,100 //此处的IP地址，要跟自己电脑的真实IP地址相同


extern struct udp_pcb *udp_pcb;
extern struct ip_addr ipaddr;
extern struct pbuf *udp_p;

void UDP_client_init(void);
void UDP_Send_Data(struct udp_pcb *pcb,struct pbuf *p);
void My_IP4_ADDR(struct ip_addr *ipaddr,unsigned char a,unsigned char b,unsigned char c,unsigned char d);
static void UDP_Delay(unsigned long ulVal);

#endif

