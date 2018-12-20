/*
 * parse.c
 *
 *  Created on: Dec 22, 2017
 *      Author: duser
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qijun_parse.h"
//#include "crc.h"
#include "main.h"

static reader_info  qijunReader;
static qijun_info Qinifo;
static qijun_info Monitorinfo;

int IsStageStep1(unsigned char status)
{
	int step;
	step = status&0xc0;
	if(step == 0)
		return 1;

	return 0;
}

int IsStageStep2(unsigned char status)
{
	int step;
	step = status&0xc0;
	if(step == 0x40)
		return 1;
	return 0;
}

int getStageSy(qijun_info *pInfo,unsigned char status)
{
	int id,i;
	char flag = 0;
	id = status&0x3f;

	for(i=0;i<pInfo->stagenum;i++)
	{
		if(pInfo->stage[i].stage_id == id)
		{
			flag =1;
			return i;
		}
	}
	return -1;
}
/*新建一个相位信息*/
void newStage(qijun_info *pInfo,unsigned char status,unsigned char time,unsigned char cycle)
{
	int id,i;
	id = status&0x3f;
	//printf("newStage \r\n");
	i=pInfo->stagenum;
	pInfo->stage[i].stage_id = id;
	if(IsStageStep1(status))
	{
		pInfo->stage[i].stage_time1 = time;
		if(time ==1)
			pInfo->stage[i].startingtime = cycle;
	}
	if(IsStageStep2(status))
		pInfo->stage[i].stage_time2 = time;

	pInfo->stagenum++;
	pInfo->learning_stage_id = pInfo->stagenum;
	pInfo->learningcount++;
	if(cycle > pInfo->cycletime)
		pInfo->cycletime = cycle;
}

/*更新相位信息*/
void updateStage(qijun_info *pInfo,int sy,unsigned char status,unsigned char time,unsigned char cycle)
{
	int id,i;
	id = status&0x3f;
	//printf("updateStage \r\n");
	i=sy;
	if(IsStageStep1(status))
	{
		if(pInfo->stage[i].stage_time1 < time )
			pInfo->stage[i].stage_time1 = time;
		if(time ==1)
			pInfo->stage[i].startingtime = cycle;
	}
	if(IsStageStep2(status))
	{
		if(pInfo->stage[i].stage_time2 < time )
			pInfo->stage[i].stage_time2 = time;
	}
	if(cycle > pInfo->cycletime)
		pInfo->cycletime = cycle;
}

/*学习模式是否结束*/
int isLearningEnding(qijun_info *pInfo,int sy,unsigned char status,unsigned char time,unsigned char cycle)
{
	int i,timecount;
	printf("isLearningEnding   learningcount:%d learning_stage_id:%d stagenum:%d \r\n",pInfo->learningcount,pInfo->learning_stage_id,pInfo->stagenum);
	if(pInfo->learning_stage_id != (sy +1))
	{
		pInfo->learningcount++;
		pInfo->learning_stage_id = sy +1;
	}
	if((sy == 0) && (pInfo->learningcount > pInfo->stagenum))
	{
		timecount = 0;
		for(i=0;i<pInfo->stagenum;i++)
		{
			if(pInfo->stage[i].startingtime == 0)
			{
				printf("isLearningEnding  err: stage %d .startingtime == 0 \r\n",i);
				return 0;
			}
			timecount += pInfo->stage[i].stage_time1;
			timecount += pInfo->stage[i].stage_time2;
		}
		if(pInfo->cycletime != timecount)
		{
			printf("isLearningEnding  err: cycletime %d != timecount %d \r\n",pInfo->cycletime,timecount);
			return -1;
		}

		return 1;
	}
	return 0;
}

/*复位数据*/
void RestMode(void)
{
	memset((char *)&Qinifo,0,sizeof(qijun_info));
	memset((char *)&Monitorinfo,0,sizeof(qijun_info));
}

/*学习模式*/
void LearningMode(char *pbuf)
{
    unsigned char  status,time,cycle;
    int iRet;
    int iSy;
    status =pbuf[0];
    time =pbuf[1];
    cycle =pbuf[2];

    printf("LearningMOde %x %x %x \r\n",pbuf[0],pbuf[1],pbuf[2]);
    iSy = getStageSy(&Qinifo,status);
    if(iSy == -1)
    {
    	newStage(&Qinifo,status,time,cycle);
    }else
    {
    	iRet =isLearningEnding(&Qinifo,iSy,status,time,cycle);
    	if( iRet > 0)
    		Qinifo.status =1;
    	else if(iRet == 0)
    		updateStage(&Qinifo,iSy,status,time,cycle);
    	else
    		RestMode();
    }
}

/*监控程序*/
int MonitorComp(qijun_info *pInfo,qijun_info *pMonitorInfo)
{

	int iRet = 0;
	int i,j;
	if(pMonitorInfo->stagenum != pInfo->stagenum)
	{
		iRet =1;
		return iRet;
	}

	if(pMonitorInfo->cycletime != pInfo->cycletime)
	{
		iRet =1;
		return iRet;
	}

	for(i=0;i<pMonitorInfo->stagenum;i++)
	{
		for(j=0;j<pMonitorInfo->stagenum;j++)
		{
			if(pMonitorInfo->stage[i].stage_id != pInfo->stage[j].stage_id)
				continue;
			if(pMonitorInfo->stage[i].stage_time1 !=  pInfo->stage[j].stage_time1)
				iRet =1;
			if(pMonitorInfo->stage[i].stage_time2 !=  pInfo->stage[j].stage_time2)
				iRet =1;
			if(pMonitorInfo->stage[i].startingtime !=  pInfo->stage[j].startingtime)
				iRet =1;
			break;
		}
		if(iRet)
			return iRet;
	}

	return iRet;
}


int Monitoring(char *pbuf)
{
    unsigned char  status,time,cycle;
    int iRet;
    int iSy;
    status =pbuf[0];
    time =pbuf[1];
    cycle =pbuf[2];

    printf("Monitoring %x %x %x \r\n",pbuf[0],pbuf[1],pbuf[2]);
    iSy = getStageSy(&Monitorinfo,status);
    if(iSy == -1)
    {
    	newStage(&Monitorinfo,status,time,cycle);
    }else
    {
    	iRet =isLearningEnding(&Monitorinfo,iSy,status,time,cycle);
    	if( iRet > 0)
    		Monitorinfo.status =1;
    	else if(iRet == 0)
    		updateStage(&Monitorinfo,iSy,status,time,cycle);
    	else
    		memset((char *)&Monitorinfo,0,sizeof(qijun_info));
    }

    if(Monitorinfo.status ==1)
    {
    	if(MonitorComp(&Qinifo,&Monitorinfo))
    	{
    		Qinifo.errcount ++;
    	}
    	memset((char *)&Monitorinfo,0,sizeof(qijun_info));
    }
    return 0;
}


extern char start_led;
extern char start_time;
extern int start_ticks_qi;
/*工作模式*/
void WorkingMode(char *pbuf,ttf_info *pttf_info)
{
    unsigned char  status,time,cycle;
    int iRet,i,j, ret;
    int startingtime;
		char led;
    //printf("WorkingMode \r\n");
    status =pbuf[0];
    time =pbuf[1];
    cycle =pbuf[2];
    iRet = getStageSy(&Qinifo,status);
    if((iRet == -1) ||(Qinifo.cycletime < cycle))
    {
    	Qinifo.errcount++;
    }else
    {
    	i=iRet;
		for(j=0;j<Qinifo.stagenum;j++)
		{
			startingtime =Qinifo.stage[j].startingtime -1;
			if(j!=i)
			{
				pttf_info->tra_lights[j].stage = Qinifo.stage[j].stage_id;
				if(startingtime > cycle)
				{
					pttf_info->tra_lights[j].time = startingtime - cycle;
				}else
				{
					pttf_info->tra_lights[j].time = startingtime + Qinifo.cycletime - cycle;
				}

				if((pttf_info->tra_lights[j].time < 1) ||(pttf_info->tra_lights[j].time == Qinifo.cycletime))
				{
					pttf_info->tra_lights[j].time = Qinifo.stage[j].stage_time1+Qinifo.stage[j].stage_time2-3;
					pttf_info->tra_lights[j].state = 'G';
				}else
					pttf_info->tra_lights[j].state = 'R';
			}else
			{
				if(IsStageStep1(status))
				{
					pttf_info->tra_lights[j].time = Qinifo.stage[j].stage_time1+Qinifo.stage[j].stage_time2-3-time;
					pttf_info->tra_lights[j].state = 'G';
				}
				else if(IsStageStep2(status))
				{
					if(Qinifo.stage[j].stage_time2 > (3 +time))
					{
						pttf_info->tra_lights[j].time =  Qinifo.stage[j].stage_time2 -time-3;
						pttf_info->tra_lights[j].state = 'G';

					}else
					{
						pttf_info->tra_lights[j].time =  Qinifo.stage[j].stage_time2 -time;
						if(pttf_info->tra_lights[j].time < 1)
						{
							pttf_info->tra_lights[j].stage = Qinifo.stage[j].stage_id;
							if(startingtime > cycle)
							{
								pttf_info->tra_lights[j].time = startingtime - cycle;
							}else
							{
								pttf_info->tra_lights[j].time = startingtime + Qinifo.cycletime - cycle;
							}
							pttf_info->tra_lights[j].state = 'R';
						}else
							pttf_info->tra_lights[j].state = 'Y';
					}
				}
				pttf_info->tra_lights[j].stage = Qinifo.stage[j].stage_id;
			}
			if(pttf_info->tra_lights[j].state == 'R')
				led = 0;
			else if(pttf_info->tra_lights[j].state == 'G')
				led = 1;
			else if(pttf_info->tra_lights[j].state == 'Y')
				led = 2;
			
			if(led == start_led && start_time == pttf_info->tra_lights[j].time)
			{
				ret = OSTimeGet() - start_ticks_qi;
				info_msg("时延 %dms\r\n", ret * 10);
			}
			
			info_msg("qijun: stage%d state:%c time:%d\r\n",j,pttf_info->tra_lights[j].state,pttf_info->tra_lights[j].time);
		}
    }

    if(Qinifo.errcount > 3)
    {
    	RestMode();
    }

}

/*得到红绿灯工作信息*/
void get_qijun_info(ttf_info *pttf_info, char *pbuf)
{
    const char pt = ',';
    char *dest;
    int i;
    tTF *tf_data = NULL;
	
		tf_data = (tTF *)&pttf_info->tra_lights;
    //printf("get_qijun_info %x,%x,%x\r\n",pbuf[0],pbuf[1],pbuf[2]);

    if(Qinifo.status ==0)
    {
    	LearningMode(pbuf);

    }else if(Qinifo.status ==1)
    {
    	WorkingMode(pbuf,pttf_info);
    	//Monitoring(pbuf);
    	pttf_info->status =Qinifo.status;
    }else
    {//err
    	memset((char *)&Qinifo,0,sizeof(qijun_info));
    }

//    printf("get_qijun_info cycletime:%0x stagenum:%0x settingstatus:%d\r\n",Qinifo.cycletime,Qinifo.stagenum,pttf_info->settingstatus);
    //dest = pbuf;

    //printf("tf state : %c  tf time : %d\r\n", tf_data->state, tf_data->time);
}

/*设置红绿灯，红绿灯返回信息处理*/
void qijun_setting_info(ttf_info *pttf_info, char *pbuf)
{
	printf("pttf_info->settingstatus %0d  pbuf[0] :%d\r\n",pttf_info->settingstatus,pbuf[0] );
	if(pbuf[0] == 0x02)
	{
		Qinifo.settingstatus++;
	}else
		Qinifo.settingstatus=0;
	if(Qinifo.settingstatus > 5)
		Qinifo.settingstatus=0;
	//if(Qinifo.settingstatus == 1)
	//	Qinifo.settingstatus++;
	pttf_info->settingstatus = Qinifo.settingstatus;
	printf("pttf_info->settingstatus %0d  Qinifo.settingstatus :%d\r\n",pttf_info->settingstatus,Qinifo.settingstatus );
}

/*红绿灯数据解析*/
void qijun_parse(reader_info *reader,ttf_info *pttf_info)
{
	  char code[3] = {0x83,0xc4,0x00};
	  char code1[3] = {0x85,0xD0,0x00};
	  int i,j;
	 // printf("qijun_parse %0x %0x %0x %0x %0x\r\n",reader->in[0],reader->in[1],reader->in[2],reader->in[3],reader->in[4]);
	 // if(!memcmp(&reader->in[2],code,3))
	  if((reader->in[2] == 0x83)&&(reader->in[3] == 0xc4)&&(reader->in[4] == 0x00))/*红绿灯工作状态*/
	  {/*
			for(i=0;i<25;i++)
			{
				 j=4*i;
				 printf("qijun_parse %x %x %x %x\r\n",reader->in[j],reader->in[j+1],reader->in[j+2],reader->in[j+3]);
			}*/
		   code[0]=reader->in[5];
		   code[1]=reader->in[6];
		   code[2]=reader->in[7];
		  get_qijun_info(pttf_info, code);
	  }
	  if((reader->in[2] == code1[0])&&(reader->in[3] == code1[1])&&(reader->in[4] == code1[2]))/*设置红绿灯，红绿反馈*/
	  {
			for(i=0;i<6;i++)
			{
				 j=4*i;
				 printf("send_qijun_parse %x %x %x %x\r\n",reader->in[j],reader->in[j+1],reader->in[j+2],reader->in[j+3]);
			}
		   code[0]=reader->in[5];
		   code[1]=reader->in[6];
		   code[2]=reader->in[7];
		   qijun_setting_info(pttf_info, code);
	  }

}

void qijun_reader(char c,ttf_info *pttf_info)
{
	int len;
	int i;
	if(qijunReader.overflow){
		qijunReader.overflow =(c != 0xa5);
	}

	if(qijunReader.pos >= READER_BUF_MAX -1)
	{
		qijunReader.overflow =1;
		qijunReader.pos=0;
		return;
	}

	if(c == 0x5a)
	{
		if(qijunReader.status == 0xa5)
		{
			qijunReader.pos=1;
			qijunReader.in[0] = qijunReader.status;
			//qijunReader.in[1] = c;
		}
	}

	qijunReader.in[qijunReader.pos] = c;
	qijunReader.pos++;

	if(c == 0xa5)
	{
		if(qijunReader.status == 0x5a)
		{
			qijun_parse(&qijunReader,pttf_info);
			memset(qijunReader.in,0,READER_BUF_MAX);
			qijunReader.pos = 0;
		}
	}

	qijunReader.status = c;

}

void init_qijunReader(void)
{
	memset((char *)&qijunReader,0,sizeof(reader_info));
	memset((char *)&Qinifo,0,sizeof(qijun_info));
	memset((char *)&Monitorinfo,0,sizeof(qijun_info));
	info_msg("qijun 初始化完成\r\n");
}

