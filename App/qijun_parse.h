/*
 * parse.h
 *
 *  Created on: Dec 22, 2017
 *      Author: duser
 */

#ifndef _QIJUN_PARSE_H_
#define _QIJUN_PARSE_H_

//------------------------------------------------------------------------------
// Included headers
//------------------------------------------------------------------------------
#include<stdint.h>
//#include "tf_parse.h"

//------------------------------------------------------------------------------
// Macros & Constants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
#define MAX_STAGE_NUM 8
/// traffic lights information
typedef struct _stage_info_
{
	int stage_id;
	int stage_time1;
	int stage_time2;
	int startingtime;

} stage_info;

typedef struct _qijun_info_
{
	int status;
	int cycletime;
	int stagenum;
	int learningcount;
	int errcount;
	int settingstatus;
	unsigned char  learning_stage_id;
	stage_info stage[MAX_STAGE_NUM];
} qijun_info;

#define TF_WORKING_MODE 1
#define TF_LEARNING_MODE 0
/// traffic lights information
typedef struct __tf__
{
	uint8_t stage;
    uint8_t state;       	//???????<1=r 2=g 3=y>
    uint8_t time;          	//???????
    uint8_t nextstate;      //????????
    uint8_t nexttime;       //????????
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

void qijun_reader(char c,ttf_info *pttf_info);
void init_qijunReader(void);
void qijun_reader(char c,ttf_info *pttf_info);
int SwithOnManualControl(char *pbuf);
int SetManualControl(char *pbuf);
int SetAutoControl(char *pbuf);
int CloseManualControl(char *pbuf);
int SetControlStage(char *pbuf,char stage);
#endif /* PARSE_H_ */
