#include "tcp_server.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "led.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "delay.h"
#include "trafficlight.h"
#include "led.h" 
u32 g_network_count = 0;
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 tcp_server_sendbuf[TCP_SERVER_TX_BUFSIZE]="Explorer STM32F407 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP���������ݷ��ͱ�־λ

//TCP�ͻ�������
#define TCPSERVER_PRIO		6
//�����ջ��С
#define TCPSERVER_STK_SIZE	300
//�����ջ
OS_STK TCPSERVER_TASK_STK1[TCPSERVER_STK_SIZE];
//OS_STK TCPSERVER_TASK_STK2[TCPSERVER_STK_SIZE];
struct netconn *conn;
//tcp����������
static void tcp_server_thread1(void *arg)
{
	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *newconn[2];
	char newconn_status[2] = {0, 0};
	static ip_addr_t ipaddr;
	static u16_t 			port;
	u8 i;
	char comm_send = -1;
	char send_flag[2] = {0,0};
	LWIP_UNUSED_ARG(arg);
	conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //�󶨶˿� 8�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	printf("tcp_server_thread1\r\n");
	while (1) 
	{
		for(i = 0; i < 2; i++)
		{
			
			if(newconn_status[i] == 0)
			{
				err = netconn_accept(conn,&newconn[i]);  //������������
				if(err==ERR_OK)
				{
					newconn[i]->recv_timeout = 10;
					newconn_status[i] = 1;
				}
			}
			

			if (newconn_status[i] > 0)    //���������ӵ�����
			{ 
				struct netbuf *recvbuf;
				
				if(newconn_status[i] == 1)
				{
				newconn_status[i] = 2;
				netconn_getaddr(newconn[i],&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
				
				remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
				remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
				remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
				remot_addr[0] = (uint8_t)(ipaddr.addr);
				printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
				}
				
					if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
					{
						if(g_send_light_info)
						{
							
							send_trafficlight_info();
							if(send_flag[i])
							err = netconn_write(newconn[i] ,tcp_server_sendbuf,sizeof(LightInfo) * g_online_num,NETCONN_COPY); //??tcp_server_sendbuf????
						}
						else
						{
							err = netconn_write(newconn[comm_send] ,tcp_server_sendbuf,sizeof(LightInfo) * g_online_num,NETCONN_COPY); //??tcp_server_sendbuf????
						}
				
						
						if(err != ERR_OK)
						{
							printf("����ʧ��\r\n");
						}
						tcp_server_flag &= ~LWIP_SEND_DATA;
					}
					
					if((recv_err = netconn_recv(newconn[i],&recvbuf)) == ERR_OK)  	//���յ�����
					{		
						OS_ENTER_CRITICAL(); //���ж�
						LED1 = !LED1;
						memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
						for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
						{
							//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
							//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
							if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
							else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
							data_len += q->len;  	
							if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
						}
						OS_EXIT_CRITICAL();  //���ж�
						data_len=0;  //������ɺ�data_lenҪ���㡣	
						if(tcp_server_recvbuf[0] == 0xaa)
						{
							send_flag[i] = 1;
							g_network_count = 0;
							if(g_send_light_info)
							tcp_server_flag = 0x80;
						}
						else if(tcp_server_recvbuf[0] == 0xbb)
						{
							printf("net quit\r\n");
							netconn_close(newconn[i]);
							netconn_delete(newconn[i]);
							newconn_status[i] = 0;
							send_flag[i] = 0;
							printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
						}
						else
							comm_send = i;
						netbuf_delete(recvbuf);
					}else if(recv_err == ERR_CLSD || recv_err == ERR_RST)  //�ر�����
					{
						if(newconn_status[i] != 0)
						{
							netconn_close(newconn[i]);
							netconn_delete(newconn[i]);
						}
						newconn_status[i] = 0;
						send_flag[i] = 0;
						printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
						break;
					}
			}
		}
	}
}

#if 0
static void tcp_server_thread2(void *arg)
{
	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *newconn;
	static ip_addr_t ipaddr;
	static u16_t 			port;
	u8 send_buf[200];
	
	LWIP_UNUSED_ARG(arg);
	printf("tcp_server_thread2\r\n");
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //������������
		
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					if(g_send_light_info)
					{
						memset(send_buf, 0, 200);
						send_trafficlight_info(send_buf);
						err = netconn_write(newconn ,send_buf,sizeof(LightInfo) * g_online_num,NETCONN_COPY); //����tcp_server_sendbuf�е�����
					}
					else
					{
						err = netconn_write(newconn ,tcp_server_sendbuf,sizeof(LightInfo) * g_online_num,NETCONN_COPY); //����tcp_server_sendbuf�е�����
					}
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
				
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//���յ�����
				{		
					OS_ENTER_CRITICAL(); //���ж�
					LED1 = !LED1;
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣	
					if(tcp_server_recvbuf[0] == 0xab)
						tcp_server_flag = 0x80;
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
			}
		}
	}
}
#endif
//����TCP�������߳�
//����ֵ:0 TCP�����������ɹ�
//		���� TCP����������ʧ��
INT8U tcp_server_init(void)
{
	INT8U res;
	OS_CPU_SR cpu_sr;
	
	
	OS_ENTER_CRITICAL();	//���ж�
	res = OSTaskCreate(tcp_server_thread1,(void*)0,(OS_STK*)&TCPSERVER_TASK_STK1[TCPSERVER_STK_SIZE-1],TCPSERVER_PRIO); //����TCP�������߳�
	delay_ms(1000);
	//res = OSTaskCreate(tcp_server_thread2,(void*)0,(OS_STK*)&TCPSERVER_TASK_STK2[TCPSERVER_STK_SIZE-1],7); //����TCP�������߳�
	OS_EXIT_CRITICAL();		//���ж�
	
	return res;
}


