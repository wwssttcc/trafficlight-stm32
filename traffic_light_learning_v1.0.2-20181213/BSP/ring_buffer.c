#include "ring_buffer.h"

RingBufffer_t	g_rbuffer;

void ringBufWrite(uint8_t data)
{
	g_rbuffer.ring_buf[g_rbuffer.tail] = data;
	if(++g_rbuffer.tail >= BUFFER_MAX)
		g_rbuffer.tail = 0;
		
	if(g_rbuffer.tail == g_rbuffer.head)
		if(++g_rbuffer.head >= BUFFER_MAX)
			g_rbuffer.head = 0;
}

uint8_t ringBufRead(uint8_t *pdata)
{
	if(g_rbuffer.head == g_rbuffer.tail)
	{
		return 1;
	}
	else
	{
		*pdata = g_rbuffer.ring_buf[g_rbuffer.head];
		if(++g_rbuffer.head >= BUFFER_MAX)
			g_rbuffer.head = 0;
			
		return 0;
	}
}

uint16_t read_ringbuf(uint8_t * pbuf, uint16_t len)
{
	uint16_t ret = 0, i;
	
	for(i = 0; i < len; i++)
	{
		if(ringBufRead(pbuf + i) == 0)
			ret++;
		else
			return ret;
	}
	
	return ret;
}