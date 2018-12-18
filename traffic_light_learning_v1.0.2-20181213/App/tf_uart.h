/*
 * util-uart.h
 *
 *  Created on: Dec 22, 2017
 *      Author: duser
 */

#ifndef UTIL_UART_H_
#define UTIL_UART_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define FALSE  -1
#define TRUE   0

int tf_uart_open(int fd,char* port);
int tf_uart_close(int fd);
int tf_uart_set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
int tf_uart_init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
int tf_uart_recv(int fd, uint8_t *rcv_buf,int data_len);
int tf_uart_send(int fd, uint8_t *send_buf,int data_len);

#endif /* UTIL_UART_H_ */
