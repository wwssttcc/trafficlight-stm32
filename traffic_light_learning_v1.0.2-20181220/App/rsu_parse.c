#include "Main.h"
#include "rsu_parse.h"

uint8_t setIP(IPInfo *ip_info)
{
	IPInfo data;
	
	STMFLASH_Write(FLASH_SAVE_ADDR,(unsigned int *)ip_info,sizeof(IPInfo) / 4);
	STMFLASH_Read(FLASH_SAVE_ADDR,(unsigned int *)&data, sizeof(IPInfo) / 4);
	info_msg("------SET IP ------\r\n");
	info_msg("IP: %d.%d.%d.%d\r\n", data.ip[0], data.ip[1], data.ip[2], data.ip[3]);
	info_msg("SUBMASK: %d.%d.%d.%d\r\n", data.mask[0], data.mask[1], data.mask[2], data.mask[3]);
	info_msg("GATEWAY: %d.%d.%d.%d\r\n", data.gateway[0], data.gateway[1], data.gateway[2], data.gateway[3]);
}

uint8_t rsuParse()
{
	uint8_t rcv_buf[512];
  int  rcv_len, i;
	uint8_t command;
	IPInfo *ip_info;
	
	rcv_len = read_ringbuf(rcv_buf, 256);
	
	for(i = 0; i < rcv_len; i++)
	{
		if(rcv_buf[i] == 0xA5 && rcv_buf[i+1] == 0x5A)
		{
			break;
		}
	}
	
	if(i == rcv_len)
		return 0;
	
	command = rcv_buf[i+4];
	info_msg("******recv ok ****** %d %d\r\n", i, command);
	if(command == SETIP)
	{
		setIP((IPInfo *)(rcv_buf+6));
	}
	return 1;
}