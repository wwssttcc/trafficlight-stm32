#ifndef _RSU_PARSE_H_
#define _RSU_PARSE_H_
#include "sys.h"
#include "stmflash.h"

typedef struct LightHead
{
	uint8_t	head1;		//0xA5
	uint8_t	head2;		//0x5A
	uint8_t	code1;		//0x83
	uint8_t	code2;		//0xA0
	uint8_t	code3;		//0x00
	uint8_t	len;			//³¤¶È
}LightHead;

typedef struct LightTail
{
	uint8_t	check;		//0xA5
	uint8_t	tail1;		//0x5A
	uint8_t	tail2;		//0x83
}LightTail;

typedef struct IPInfo
{
	uint8_t ip[4];
	uint8_t mask[4];
	uint8_t gateway[4];
	uint8_t mac1[4];
	uint8_t mac2[4];
	uint8_t version[4];
	uint8_t date[4];
	uint16_t update_flag;
	uint16_t reset_num;
}IPInfo;

#define GETINFO	0x01
#define SETIP		0x02
#define RESET		0x03
#define UPGRADE_START		0x04
#define UPGRADE_PACK		0x05
#define	UPGRADE_STOP		0x06

uint8_t rsuParse(uint8_t *p, uint16_t len);

#endif
