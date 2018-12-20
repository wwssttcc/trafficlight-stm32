#ifndef	_RING_BUFFER_H
#define	_RING_BUFFER_H

#include "main.h"

#define BUFFER_MAX	300

typedef struct RingBuffer{
	unsigned	char head;
	unsigned	char tail;
	unsigned 	char ring_buf[BUFFER_MAX];
	}RingBufffer_t;
	

	
void ringBufWrite(uint8_t data);
uint8_t ringBufRead(uint8_t *pdata);
uint16_t read_ringbuf(uint8_t * pbuf, uint16_t len);

#endif