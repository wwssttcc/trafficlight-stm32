#ifndef _RSU_PARSE_H_
#define _RSU_PARSE_H_
#include "Main.h"

typedef struct IPInfo
{
	uint8_t ip[4];
	uint8_t mask[4];
	uint8_t gateway[4];
}IPInfo;

#define SETIP		0x02

uint8_t rsuParse();

#endif