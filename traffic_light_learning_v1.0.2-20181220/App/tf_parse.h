/*
 * parse.h
 *
 *  Created on: Dec 22, 2017
 *      Author: duser
 */

#ifndef PARSE_H_
#define PARSE_H_

//------------------------------------------------------------------------------
// Included headers
//------------------------------------------------------------------------------
#include<stdint.h>
#include "tf_uart.h"

//------------------------------------------------------------------------------
// Macros & Constants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
#define TF_WORKING_MODE 1
#define TF_LEARNING_MODE 0
/// traffic lights information
typedef struct __tf__
{
	uint8_t stage;
    uint8_t state;       	//交通信号灯状态<1=r 2=g 3=y>
    uint8_t time;          	//交通信号灯时间
    uint8_t nextstate;      //下一个信号灯状态
    uint8_t nexttime;       //下一个信号灯时间
} __attribute__((packed)) tTF;

/// v2i information
typedef struct ttf_info
{
	tTF tra_lights[8];			//traffic lights
	int status;
	int settingstatus;
	int settingstage;
} __attribute__((packed)) ttf_info;

#define READER_BUF_MAX 512
typedef struct _reader_info_
{
	int status;
	int pos;
	int overflow;
	int in[READER_BUF_MAX];
} reader_info;
//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

int tf_open(int fd,char* dev_name);
void tf_close(int fd);
int tf_init(int fd, int speed,int flow_ctrl,
		int databits,int stopbits,int parity);
int tf_read(int fd, ttf_info *ptf_info);
int tf_write(int fd, uint8_t *send_buf,int data_len);

#endif /* PARSE_H_ */
