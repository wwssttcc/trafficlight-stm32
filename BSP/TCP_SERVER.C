#include "main.h"
#include "ring_buffer.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"

uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
/***********************************************************************
函数名称：tcp_server_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
功    能：TCP数据接收和发送
注    意：这是一个回调函数，当一个TCP段到达这个连接时会被调用
***********************************************************************/
uint8_t tcp_server_flag;
char tcp_server_sendbuf[120]={0};
extern uint8_t g_online_num;						//红绿灯在线数
//lwIP tcp_err函数的回调函数
void tcp_server_error(void *arg,err_t err)
{  
	LWIP_UNUSED_ARG(err);  
	printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//释放内存
} 

//lwIP tcp_recv()函数的回调函数
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  struct tcp_server_struct *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_server_struct *)arg;
	if(p==NULL) //从客户端接收到空数据
	{
		es->state=ES_TCPSERVER_CLOSING;//需要关闭TCP 连接了
		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!=ERR_OK)	//从客户端接收到一个非空数据,但是由于某种原因err!=ERR_OK
	{
		if(p)pbuf_free(p);	//释放接收pbuf
		ret_err=err;
	}else if(es->state==ES_TCPSERVER_ACCEPTED) 	//处于连接状态
	{
		if(p!=NULL)  //当处于连接状态并且接收到的数据不为空时将其打印出来
		{
			memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) /*write_ringbuf(q->payload, (TCP_SERVER_RX_BUFSIZE-data_len));*/memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
				else /*write_ringbuf(q->payload, q->len);*/memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
			}
			tcp_server_flag|=1<<6;	//标记接收到数据了
 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
		}
	}else//服务器关闭了
	{
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		es->p=NULL;
		pbuf_free(p); //释放内存
		ret_err=ERR_OK;
	}
	return ret_err;
}


//此函数用来发送数据
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	struct pbuf *ptr;
	u16 plen;
	err_t wr_err=ERR_OK;
	 while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
	 {
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{ 
			plen=ptr->len;
			es->p=ptr->next;			//指向下一个pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//更新tcp窗口大小
		}else if(wr_err==ERR_MEM)es->p=ptr;
	 }
} 

//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->p)tcp_server_senddata(tpcb,es);//发送数据
	return ERR_OK;
} 

//关闭tcp连接
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
	if(es)mem_free(es); 
	tcp_server_flag&=~(1<<5);//标记连接断开了
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//在 tcp.c里面 
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 
extern struct tcp_pcb *tcp_tw_pcbs;				//在 tcp.c里面  
//强制删除TCP Server主动断开时的time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	__IO uint32_t LocalTime = 0;
	u8 t=0;
	while(tcp_active_pcbs!=NULL&&t<200)
	{
		LocalTime += 10;
		LwIP_Periodic_Handle(LocalTime);		//继续轮询
		t++;
		OSTimeDlyHMSM(0, 0, 0, 100);			//等待tcp_active_pcbs为空  
	}
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//如果有等待状态的pcbs
	{
		tcp_pcb_purge(pcb); 
		tcp_tw_pcbs=pcb->next;
		pcb2=pcb;
		pcb=pcb->next;
		memp_free(MEMP_TCP_PCB,pcb2);	
	}
}
extern LightInfo light_info[12];
//lwIP tcp_poll的回调函数
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
	if(es!=NULL)
	{
		if(tcp_server_flag&(1<<7))	//判断是否有数据要发送
		{
			es->p=pbuf_alloc(PBUF_TRANSPORT,sizeof(LightInfo) * 12,PBUF_POOL);//申请内存
			pbuf_take(es->p,(char*)&tcp_server_sendbuf, sizeof(LightInfo) * 12);
			tcp_server_senddata(tpcb,es); 		//轮询的时候发送要发送的数据
			tcp_server_flag&=~(1<<7);  			//清除数据发送标志位
			if(es->p!=NULL)pbuf_free(es->p); 	//释放内存	
		}else if(es->state==ES_TCPSERVER_CLOSING)//需要关闭连接?执行关闭操作
		{
			tcp_server_connection_close(tpcb,es);//关闭连接
		}
		ret_err=ERR_OK;
	}else
	{
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err=ERR_ABRT; 
	}
	return ret_err;
} 

err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	err_t ret_err;
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//设置新创建的pcb优先级
	es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //分配内存
		if(es!=NULL) //内存分配成功
	{
		es->state=ES_TCPSERVER_ACCEPTED;  	//接收连接
		es->pcb=newpcb;
		es->p=NULL;
		
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//初始化tcp_recv()的回调函数
		tcp_err(newpcb,tcp_server_error); 	//初始化tcp_err()回调函数
		tcp_poll(newpcb,tcp_server_poll,1);	//初始化tcp_poll回调函数
		tcp_sent(newpcb,tcp_server_sent);  	//初始化发送回调函数
		  
		tcp_server_flag|=1<<5;				//标记有客户端连上了
		ret_err=ERR_OK;
	}else ret_err=ERR_MEM;
	return ret_err;
}

struct tcp_pcb *tcppcbnew;  	//定义一个TCP服务器控制块
struct tcp_pcb *tcppcbconn;  	//定义一个TCP服务器控制块
uint8_t tcp_server_init(void)
{
	err_t err;
	tcppcbnew = tcp_new();
	
	if(tcppcbnew)
	{
		err = tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);
		if(err == ERR_OK)
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//设置tcppcb进入监听状态
			tcp_accept(tcppcbconn,tcp_server_accept); 	//初始化LWIP的tcp_accept的回调函数
			return 0;
		}
		return 1;
	}
	return 0;
}
