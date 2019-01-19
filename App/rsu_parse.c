#include "Main.h"
#include "rsu_parse.h"
#include "trafficlight.h"
#include "iap.h"
extern char g_send_light_info;
extern uint8_t tcp_server_flag;
extern char tcp_server_sendbuf[120];
static uint32_t pack_offset = 0;
uint8_t setIP(IPInfo *ip_info)
{
	IPInfo data;
	LightHead *light_head;
	LightTail	*light_tail;
	uint8_t i = 0;
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	for(i = 0; i < 4; i++)
	{
		data.ip[i] = ip_info->ip[i];
		data.mask[i] = ip_info->mask[i];
		data.gateway[i] = ip_info->gateway[i];
		data.mac1[i] = ip_info->mac1[i];
		data.mac2[i] = ip_info->mac2[i];
	}
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	info_msg("------SET IP ------\r\n");
	info_msg("IP: %d.%d.%d.%d\r\n", data.ip[0], data.ip[1], data.ip[2], data.ip[3]);
	info_msg("SUBMASK: %d.%d.%d.%d\r\n", data.mask[0], data.mask[1], data.mask[2], data.mask[3]);
	info_msg("GATEWAY: %d.%d.%d.%d\r\n", data.gateway[0], data.gateway[1], data.gateway[2], data.gateway[3]);
	info_msg("MAC: %x:%x:%x:%x:%x:%x\r\n", data.mac1[0], data.mac1[1], data.mac1[2], data.mac1[3], data.mac2[0], data.mac2[1]);
	info_msg("UPDGRADE: %d\r\n", data.update_flag);
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = SETIP;
	light_head->len = 1;
	tcp_server_sendbuf[sizeof(LightHead)] = 1;
	light_tail = (LightTail *)&tcp_server_sendbuf[sizeof(LightHead) + 1];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, sizeof(LightHead) + 1);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	return 1;
}

uint8_t getInfo()
{
	IPInfo data;
	LightHead *light_head;
	LightTail	*light_tail;
	uint8_t len = 0;
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = GETINFO;
	light_head->len = sizeof(IPInfo);
	len = sizeof(LightHead);
	memcpy(tcp_server_sendbuf+len, &data, sizeof(IPInfo));
	len += sizeof(IPInfo);
	light_tail = (LightTail *)&tcp_server_sendbuf[len];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, len);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	
	return 1;
}

void reset(void)
{
	
	LightHead *light_head;
	LightTail	*light_tail;
	
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = RESET;
	tcp_server_sendbuf[sizeof(LightHead)] = 1;
	light_tail = (LightTail *)&tcp_server_sendbuf[sizeof(LightHead) + 1];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, sizeof(LightHead) + 1);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	
	OSTimeDlyHMSM(0, 0, 1, 0);
	__set_FAULTMASK(1); //关闭所有中断
  NVIC_SystemReset(); //复位
}

uint8_t start_upgrade(uint8_t *ptr)
{
	
	LightHead *light_head;
	LightTail	*light_tail;
	IPInfo data;
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	data.update_flag = 0;
	data.version[0] = ptr[0];
	data.version[1] = ptr[1];
	data.version[2] = ptr[2];
	data.date[0] = ptr[4];
	data.date[1] = ptr[5];
	data.date[2] = ptr[6];
	data.date[3] = ptr[7];
	
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = UPGRADE_START;
	light_head->len = 1;
	tcp_server_sendbuf[sizeof(LightHead)] = 1;
	light_tail = (LightTail *)&tcp_server_sendbuf[sizeof(LightHead) + 1];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, sizeof(LightHead) + 1);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	
	OSTimeDlyHMSM(0, 0, 1, 0);
	__set_FAULTMASK(1); //关闭所有中断
  NVIC_SystemReset(); //复位
	
	return 1;
}

uint8_t working_upgrade(uint32_t offset, uint8_t *pbuf, uint16_t len, uint8_t result)
{
	//uint16_t i = 0;
	LightHead *light_head;
	LightTail	*light_tail;
	
	#if 0
	printf("\r\n");
	for(i = 0; i < 1188; i++)
	{
		if(i%16 == 0)
			printf("\r\n");
		printf("%02x ", pbuf[i]);
	//	if(rcv_buf[i] == 0xA5 && rcv_buf[i+1] == 0x5A)
		{
	//		break;
		}
	}
	#endif
	if(result)
		iap_write_appbin(FLASH_APP1_ADDR + offset,pbuf, len);
	
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = UPGRADE_PACK;
	light_head->len = 1;
	tcp_server_sendbuf[sizeof(LightHead)] = result;
	light_tail = (LightTail *)&tcp_server_sendbuf[sizeof(LightHead) + 1];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, sizeof(LightHead) + 1);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	
	return 1;
}

uint8_t stop_upgrade(void)
{
	
	LightHead *light_head;
	LightTail	*light_tail;
	IPInfo data;
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	data.update_flag = 1;
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	info_msg("update_flag %d\r\n", data.update_flag);
	memset(tcp_server_sendbuf, 0, sizeof(tcp_server_sendbuf));
	light_head = (LightHead *)tcp_server_sendbuf;
	light_head->head1 = 0xA5;
	light_head->head2 = 0x5A;
	light_head->code1 = 0x83;
	light_head->code2 = 0xA0;
	light_head->code3 = UPGRADE_STOP;
	light_head->len = 1;
	tcp_server_sendbuf[sizeof(LightHead)] = 1;
	light_tail = (LightTail *)&tcp_server_sendbuf[sizeof(LightHead) + 1];
	light_tail->check = checkSum((uint8_t *)tcp_server_sendbuf, sizeof(LightHead) + 1);
	light_tail->tail1 = 0x5A;
	light_tail->tail2 = 0xA5;
	tcp_server_flag|=1<<7;//标记要发送数据
	
	OSTimeDlyHMSM(0, 0, 1, 0);
	//iap_load_app(FLASH_APP1_ADDR);
	__set_FAULTMASK(1); //关闭所有中断
  NVIC_SystemReset(); //复位
	
	return 1;
}

extern uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];
uint8_t rsuParse(uint8_t *p, uint16_t len)
{
  int  i = 0;
	uint8_t command;
	//IPInfo *ip_info;
	uint16_t pack_len = 0;
	uint8_t check = 0;
	uint8_t result =0;
	
	if(p[0] != 0xA5 || p[1] != 0x5A)
		return 0;
	
	command = p[i+4];
	//info_msg("******recv ok ****** %d %d\r\n", i, command);
	switch(command)
	{
		case SETIP:
			info_msg("SETIP\r\n");
			g_send_light_info = 0;
			setIP((IPInfo *)(p+6));
			break;
		
		case GETINFO:
			info_msg("GETINFO\r\n");
			g_send_light_info = 0;
			getInfo();
			break;
			
		case RESET:
			info_msg("RESET\r\n");
			g_send_light_info = 0;
			reset();
			break;
		
		case UPGRADE_START:
			info_msg("UPGRADE_START\r\n");
			start_upgrade(p+6);
			pack_offset = 0;
			g_send_light_info = 0;
			//reset();
			break;
		
		case UPGRADE_PACK:
			info_msg("UPGRADE_PACK\r\n");
			g_send_light_info = 0;
			pack_len = p[6];
			pack_len = ((pack_len << 8) & 0xff00 )+ p[5];
			info_msg("pack_len %d, pack_offset %d\r\n", pack_len, pack_offset);
			
			if(pack_offset == 0)
			{
				if(((*(vu32*)(p+8+4))&0xFF000000)==0x08000000)//?????0X08XXXXXX.
				{
					info_msg("app start upgrade\r\n");
				}
				else
				{
					info_msg("app data illegal\r\n");
					break;
				}
			}
			check = checkSum(p, sizeof(LightHead)+2+pack_len);
			
			if(p[sizeof(LightHead)+2+pack_len] == check)
			{
				result = 1;
				pack_offset += pack_len;
			}
			else
			{
				info_msg("check sum fail %x %x\r\n", p[sizeof(LightHead)+2+pack_len], check);
				result = 0;
			}
			working_upgrade(pack_offset, &p[8], pack_len, result);
			break;
		
		case UPGRADE_STOP:
			info_msg("UPGRADE_STOP\r\n");
			pack_offset = 0;
			g_send_light_info = 0;
			stop_upgrade();
			break;
	}
	memset(p, 0, len);
	return 1;
}
