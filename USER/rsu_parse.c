#include "tcp_server.h"
#include "includes.h"
#include "stdio.h"
#include "string.h"
#include "rsu_parse.h"
#include "trafficlight.h"
#include "iap.h"
extern char g_send_light_info;
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
	printf("------SET IP ------\r\n");
	printf("IP: %d.%d.%d.%d\r\n", data.ip[0], data.ip[1], data.ip[2], data.ip[3]);
	printf("SUBMASK: %d.%d.%d.%d\r\n", data.mask[0], data.mask[1], data.mask[2], data.mask[3]);
	printf("GATEWAY: %d.%d.%d.%d\r\n", data.gateway[0], data.gateway[1], data.gateway[2], data.gateway[3]);
	printf("MAC: %x:%x:%x:%x:%x:%x\r\n", data.mac1[0], data.mac1[1], data.mac1[2], data.mac1[3], data.mac2[0], data.mac2[1]);
	printf("UPDGRADE: %d\r\n", data.update_flag);
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
	g_online_num = 1;
	tcp_server_flag = 0x80;//标记要发送数据
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
	g_online_num = 3;
	tcp_server_flag = 0x80;//标记要发送数据
	
	return 1;
}

void reset(void)
{
	
	LightHead *light_head;
	LightTail	*light_tail;
	IPInfo data;
	
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
	g_online_num = 1;
	tcp_server_flag = 0x80;;//标记要发送数据
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	data.reset_num = 0;
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
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
	g_online_num = 1;
	tcp_server_flag = 0x80;;//标记要发送数据
	
	OSTimeDlyHMSM(0, 0, 1, 0);
	__set_FAULTMASK(1); //关闭所有中断
  NVIC_SystemReset(); //复位
	
	return 1;
}

uint8_t working_upgrade(uint32_t offset, uint8_t *pbuf, uint16_t len, uint8_t result)
{
//	uint16_t i = 0;
	LightHead *light_head;
	LightTail	*light_tail;
	//unsigned int data;

	if(result)
		iap_write_appbin(FLASH_APP1_ADDR + offset,pbuf, len);
	
	#if 0
	printf("-----------%d----------\r\n", len);
	for(i = 0; i < len/ 4; i++)
	{
		if(i%16 == 0)
			printf("\r\n");
		STMFLASH_Read(FLASH_APP1_ADDR+ offset+i*4,(unsigned int *)&data, 1);
		printf(" %08x ", data);
	//	if(rcv_buf[i] == 0xA5 && rcv_buf[i+1] == 0x5A)
		{
	//		break;
		}
	}
	#endif
	//STMFLASH_Read(FLASH_APP1_ADDR,(unsigned int *)&data, 1);
	//	printf(" %08x", data);
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
	g_online_num = 1;
	tcp_server_flag = 0x80;//标记要发送数据
	
	return 1;
}

uint8_t stop_upgrade(void)
{
	
	LightHead *light_head;
	LightTail	*light_tail;
	IPInfo data;
	
	#if 1
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	data.update_flag = 1;
	data.reset_num = 0;
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)&data,sizeof(IPInfo) / 4);
	OSTimeDlyHMSM(0, 0, 0, 100);
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	printf("update_flag %d\r\n", data.update_flag);
	#endif
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
	g_online_num = 1;
	tcp_server_flag = 0x80;//标记要发送数据
	
	
	//iap_load_app(FLASH_APP1_ADDR);
	__set_FAULTMASK(1); //关闭所有中断
  NVIC_SystemReset(); //复位
	
	return 1;
}

static u8 pack_num = 1;
uint8_t rsuParse(uint8_t *p, uint16_t len)
{
  int  i = 0;
	uint8_t command;
	//IPInfo *ip_info;
	uint16_t pack_len = 0;
	uint8_t check = 0;
	uint8_t result =0;
	
	for(i = 0; i < len; i++)
	{
		if(p[i] == 0xA5 && p[i+1] == 0x5A)
			break;
	}
	if(i == len)
		return 0;
	
	command = p[i+4];
	//printf("******recv ok ****** %d %d\r\n", i, command);
	switch(command)
	{
		case SETIP:
			printf("SETIP\r\n");
			g_send_light_info = 0;
			setIP((IPInfo *)(p+i+6));
			break;
		
		case GETINFO:
			printf("GETINFO\r\n");
			g_send_light_info = 0;
			getInfo();
			break;
			
		case RESET:
			printf("RESET\r\n");
			g_send_light_info = 0;
			reset();
			break;
		
		case UPGRADE_START:
			printf("UPGRADE_START\r\n");
			pack_num = 0;
			start_upgrade(p+i+6);
			pack_offset = 0;
			g_send_light_info = 0;
			//reset();
			break;

		case UPGRADE_PACK:
			printf("UPGRADE_PACK\r\n");
			g_send_light_info = 0;
			pack_len = p[i+6];
			pack_len = ((pack_len << 8) & 0xff00 )+ p[i+5];
			printf("pack_len %d, pack_offset %d \r\n", pack_len, pack_offset);
			if(pack_offset == 0)
			{
				if(((*(vu32*)(p+i+8+4))&0xFF000000)==0x08000000)
				{
					printf("app start upgrade\r\n");
					result = 1;
				}
				else
				{
					printf("app data illegal\r\n");
					result = 0;
				}
			}
			check = checkSum(p+i, sizeof(LightHead)+2+pack_len);
			
			if(p[i+sizeof(LightHead)+2+pack_len] == check)
			{
				result = 1;			
				
				if(pack_num == p[i+7])
				{
					result = 1;
					//printf("pack num %d %d\r\n", pack_num, p[i+7]);
					pack_num++;
				}
				else
					result = 0;
				//printf("pack_num %d,%d result %d \r\n", pack_num, p[7], result);
				
			}
			else
			{
				printf("check sum fail %x %x\r\n", p[i+sizeof(LightHead)+2+pack_len], check);
				result = 0;
			}
			working_upgrade(pack_offset, &p[i+8], pack_len, result);
			
			if(result)
				pack_offset += pack_len;
			
			break;

		case UPGRADE_STOP:
			printf("UPGRADE_STOP\r\n");
			pack_offset = 0;
			g_send_light_info = 0;
			stop_upgrade();
			break;
	}
	memset(p, 0, len);
	return 1;
}
