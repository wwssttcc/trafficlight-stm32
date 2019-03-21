#ifndef _IWDG_H_
#define _IWDG_H_
#include "sys.h"
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

#endif
