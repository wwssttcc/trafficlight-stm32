#include "main.h"

TrafficLight g_trafficlight[4][3];	//总共4个路口，每个路口有3组红绿灯，代表前行，左拐，右拐
uint8_t g_online_num = 0;						//红绿灯在线数
TrafficLightPos g_online[12];				//红绿灯在线方位
char g_send_light_info = 1;

//获取红绿灯在线数量，方位,东西南北：0123，左直右：012
uint8_t trafficLightNum()
{
	uint8_t i, j;
	uint32_t ret = 0, status;
	
	//东西方位
	ret = HC165D1_Read() & 0x00ffffff;
	info_msg("ret1 %x\r\n", ret);
	for(i = 0; i < 6; i++)
	{
		for(j = i*3; j < i*3+3; j++)
		{
			status = ret & (0x01 << j);
			if(status == 0)
			{
				g_online_num++;
				g_online[g_online_num - 1].dir = (Direction)(i / 3);
				g_online[g_online_num - 1].to = (Toward)(i % 3);
				break;
			}
		}
	}
	
	//南北方位
	ret = HC165D2_Read() & 0x00ffffff;
	info_msg("ret2 %x\r\n", ret);
	for(i = 0; i < 6; i++)
	{
		for(j = i*3; j < i*3+3; j++)
		{
			status = ret & (0x01 << j);
			if(status == 0)
			{
				g_online_num++;
				g_online[g_online_num - 1].dir = (Direction)(i/3 + 2);
				g_online[g_online_num - 1].to = (Toward)(i%3);
				break;
			}
		}
	}
	//g_online_num = 8;
	return g_online_num;
}

void trafficLightRead()
{
	uint8_t i = 0, j, zero_num = 0;
	uint8_t dir, to, led_num;
	uint32_t ret1 = 0, ret2 = 0, status = 0;
	
	//info_msg("HC165D1 %x\r\n", ret1);
	
	for(i = 0; i < g_online_num; i++)
	{
		led_num = g_online[i].dir * 3 + g_online[i].to;
		dir = g_online[i].dir;
		to = g_online[i].to;
		
		if(led_num < 6)
		{
			ret1 = HC165D1_Read() & 0x00ffffff;
			ret2 = led_num * 3;
		}
		else
		{
			ret1 = HC165D2_Read() & 0x00ffffff;
			ret2 = (led_num - 6) * 3;
		}
//		info_msg("read value %d  led %d dir %d to %d %x\r\n", i, led_num, dir, to, ret1);
		zero_num = 0;
		
		//判断红绿是否有两个以上的灯亮，如果有就舍弃数据
		for(j = ret2; j < ret2 + 3; j++)
		{
			status = ret1 & (0x01 << j);
			if(status == 0)
			{
				zero_num++;
			}
			
			//红绿灯不可能出现同时两个灯亮
			if(zero_num > 1)
			{
				warn_msg("red light status error:%d dir %d to %d %x\r\n", zero_num, dir, to, ret1);
				g_trafficlight[dir][to].error_num++;
				if(g_trafficlight[dir][to].status == 1)
					g_trafficlight[dir][to].error_num = 0;
				g_trafficlight[dir][to].status = 2;		//同时出现两个灯及以上的灯亮
				if(g_trafficlight[dir][to].error_num == 2)
				{
					err_msg("trafficlight status error, need init:%d dir %d to %d %x\r\n", g_trafficlight[dir][to].error_num, dir, to, ret1);
					g_trafficlight[dir][to].light_period[red] = 0;
					g_trafficlight[dir][to].light_period[green] = 0;
					g_trafficlight[dir][to].light_period[yellow] = 0;
					g_trafficlight[dir][to].study_flag = 1;
					g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
					g_trafficlight[dir][to].start_ticks = 0;
				}
				return;
			}
			else
				g_trafficlight[dir][to].status = 1;			//红绿灯状态正常
		}
		
		for(j = ret2; j < ret2 + 3; j++)
		{
			status = ret1 & (0x01 << j);

			if(status == 0)
			{
				g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
				g_trafficlight[dir][to].current_light = (LightType)(j % 3);
			}
		}
	}
}

void trafficLightInit()
{
	uint8_t i, j;
	uint8_t dir, to;
	uint8_t res1 = 1, res2 = 0;
	
	memset(g_trafficlight, 0, 12 * sizeof(TrafficLight));
	
	//初始化将红绿灯状态置为unknown
	for(i = 0; i < 4; i++)
		for(j = 0; j < 3; j++)
			g_trafficlight[i][j].current_light = unknown;
	
	//trafficLightNum();
	#if 1
	while(res1 != res2)
	{
		res1 = trafficLightNum();		//测出红绿灯在线数以及方位
		g_online_num = 0;
		OSTimeDlyHMSM(0, 0, 0, 100);
		res2 = trafficLightNum();
		//IWDG_Feed();
	}
	#endif
	info_msg("Tips：\r\n");
	info_msg("east wset south north:0123\r\n");
	info_msg("left font right：012\r\n");
	info_msg("trafficlight online: %d\r\n", g_online_num);
	trafficLightRead();	
	for(i = 0; i < g_online_num; i++)
	{
		dir = g_online[i].dir;
		to = g_online[i].to;
		g_trafficlight[dir][to].study_flag = 1;
		g_trafficlight[dir][to].start_study_flag = 1;
		g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
		g_trafficlight[dir][to].current_seconds = 255;
		info_msg("dir %d to %d light %d %d\r\n", dir, to, g_trafficlight[dir][to].current_light, g_trafficlight[dir][to].study_flag);
	}	
		
	info_msg("trafficlight init success\r\n");
}

void trafficLightStudy()
{
	uint8_t i;
	uint8_t dir, to;
	
	for(i = 0; i < g_online_num; i++)
	{
		dir = g_online[i].dir;
		to = g_online[i].to;
		if(g_trafficlight[dir][to].study_flag == 1)
		{
			g_trafficlight[dir][to].study_num++;
			//如果学习的时间超过5分钟，则认为学习完成，且红绿灯状态一直为常亮
			if(g_trafficlight[dir][to].study_num > 15000)
			{
				g_trafficlight[dir][to].study_flag = 0;
				g_trafficlight[dir][to].status = 3;
				info_msg("学习时间超过5分钟，停止学习，dir %d to %d 为常亮\r\n", dir, to);
			}				
			
			//若上次红绿灯状态与当前红绿灯状态不一致，则认为状态发生改变
			if(g_trafficlight[dir][to].last_light != g_trafficlight[dir][to].current_light)
			{
				g_trafficlight[dir][to].current_ticks_study = OSTimeGet();
				//测得正常状态下的红绿的start_ticks不能为0
				if(g_trafficlight[dir][to].start_ticks_study != 0)
				{
					//计算出当前红绿灯周期
					g_trafficlight[dir][to].light_period_study[g_trafficlight[dir][to].last_light] = (g_trafficlight[dir][to].current_ticks_study - g_trafficlight[dir][to].start_ticks_study + 50) / 100;
					g_trafficlight[dir][to].light_period[g_trafficlight[dir][to].last_light] = g_trafficlight[dir][to].light_period_study[g_trafficlight[dir][to].last_light];
					g_trafficlight[dir][to].total_ticks = g_trafficlight[dir][to].current_ticks_study - g_trafficlight[dir][to].start_ticks_study;
					info_msg("dir %d to %d, light %d total_ticks %d\r\n", dir, to, g_trafficlight[dir][to].current_light, g_trafficlight[dir][to].total_ticks_study);
				}
				g_trafficlight[dir][to].start_ticks_study = OSTimeGet();
				//只有红绿黄周期都测输出来，学习结束
				if(g_trafficlight[dir][to].light_period_study[red] && g_trafficlight[dir][to].light_period_study[green] && g_trafficlight[dir][to].light_period_study[yellow])
				{
					info_msg("dir %d to %d学习完成\r\n", dir, to);
					g_trafficlight[dir][to].start_study_flag = 0;
					info_msg("学习周期为：red %d green %d yellow %d\r\n", g_trafficlight[dir][to].light_period[red], g_trafficlight[dir][to].light_period[green], g_trafficlight[dir][to].light_period[yellow]);
					g_trafficlight[dir][to].study_flag = 0;
					g_trafficlight[dir][to].status = 0;
				}
			}
			
		}
	}
}

void trafficLight_study_once()
{
	uint8_t i;
	uint8_t dir, to;
	
	for(i = 0; i < g_online_num; i++)
	{
		dir = g_online[i].dir;
		to = g_online[i].to;
		g_trafficlight[dir][to].study_flag = 1;
		g_trafficlight[dir][to].start_ticks_study = 0;
		g_trafficlight[dir][to].total_ticks_study = 0;
		g_trafficlight[dir][to].current_ticks_study = 0;
		g_trafficlight[dir][to].status = 0;
		//g_trafficlight[dir][to].start_ticks = 0;
		g_trafficlight[dir][to].light_period_study[red] = 0;
		g_trafficlight[dir][to].light_period_study[green] = 0;
		g_trafficlight[dir][to].light_period_study[yellow] = 0;
	}
}

void trafficLightWork()
{
	uint8_t i;
	uint8_t dir, to;
	
	for(i = 0; i < g_online_num; i++)
	{
			dir = g_online[i].dir;
			to = g_online[i].to;
		//红绿灯不学习时，只需记录每个状态改变时start_ticks的值
		if(g_trafficlight[dir][to].start_study_flag == 0)
		{
			if(g_trafficlight[dir][to].last_light != g_trafficlight[dir][to].current_light)
				g_trafficlight[dir][to].start_ticks = OSTimeGet();
		}
	}
}

uint8_t checkSum(uint8_t *pbuf, uint16_t len)
{
	uint8_t ret = 0;
	uint16_t i;
	for(i = 0; i < len; i++)
	{
		ret = ret + pbuf[i];
	}
	return ret;
}

char start_led;
char start_time;
int start_ticks_qi;
extern uint8_t tcp_server_flag;
extern char tcp_server_sendbuf[120];
int trafficLightStatus(LightInfo *light_info, uint8_t num)
{
	uint8_t dir, to;
	uint32_t total_ticks = 0;
	//uint8_t ret;
	uint8_t sec;
	dir = g_online[num].dir;
	to = g_online[num].to;
	
	light_info->direct = (Direction)dir;
	light_info->to = (Toward)to;
	light_info->light = g_trafficlight[dir][to].current_light;
	total_ticks = OSTimeGet() - g_trafficlight[dir][to].start_ticks;
	//ret = total_ticks % 100;
	sec = total_ticks / 100;
	
	#if 0
	if(sec == g_trafficlight[dir][to].current_seconds)
	{
		g_trafficlight[dir][to].current_seconds = sec;
			return 2;
	}
	#endif
	g_trafficlight[dir][to].current_light_sec = g_trafficlight[dir][to].current_light;
	g_trafficlight[dir][to].current_seconds = sec;
	light_info->last_seconds = g_trafficlight[dir][to].light_period[light_info->light] - total_ticks / 100 - 1;
	g_trafficlight[dir][to].last_seconds[light_info->light] = light_info->last_seconds; 
	if(light_info->last_seconds == 255)
		return 3;
	if(light_info->last_seconds == 0)
	{
		light_info->light++;
		if(light_info->light > 2)
			light_info->light = (LightType)0;
		light_info->last_seconds = g_trafficlight[dir][to].light_period[light_info->light];
	}
	

		start_led = light_info->light;
		start_time = light_info->last_seconds;
		start_ticks_qi = OSTimeGet();
	
		/* 红绿灯常亮，则剩余时间为255 */
		if(g_trafficlight[dir][to].status == 3)
			light_info->last_seconds = 255;
		
		if(light_info->light == red)
		{
			light_info->light = 'R';
			light_info->next_light = 'G';
			//info_msg("trafficlight：dir %d to %d:R %02ds %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
			info_msg("trafficlight：dir %d to %d:R %02ds\r\n", dir, to, light_info->last_seconds);
		}
		else if(light_info->light == green)
		{
			light_info->light = 'G';
			light_info->next_light = 'Y';
			//info_msg("trafficlight：dir %d to %d:G %02ds %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
			info_msg("trafficlight：dir %d to %d:G %02ds\r\n", dir, to, light_info->last_seconds);
		}
		else
		{
			light_info->light = 'Y';
			light_info->next_light = 'R';
			//info_msg("trafficlight：dir %d to %d:Y %02ds %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
			info_msg("trafficlight：dir %d to %d:Y %02ds\r\n", dir, to, light_info->last_seconds);
		}
		
		light_info->to++;			//协议上方向是123，程序中是012
		light_info->head1 = 0xA5;
		light_info->head2 = 0x5A;
		light_info->code1 = 0x83;
		light_info->code2 = 0xA0;
		light_info->code3 = 0x00;
		light_info->len = sizeof(LightInfo);
		light_info->tail1 = 0x5A;
		light_info->tail2 = 0xA5;
		#if 1
		//若红绿灯常亮或者学习，剩余时间都置为1
		//if((g_trafficlight[dir][to].study_flag == 1 && g_trafficlight[dir][to].start_study_flag == 1) || g_trafficlight[dir][to].status == 3)
		if(g_trafficlight[dir][to].start_study_flag == 1 || g_trafficlight[dir][to].status == 3)
		{
			light_info->last_seconds = 255;
		}
		else
		{
			light_info->red_period = g_trafficlight[dir][to].light_period[red];
			light_info->green_period = g_trafficlight[dir][to].light_period[green];
			light_info->yellow_period = g_trafficlight[dir][to].light_period[yellow];
		}
		light_info->check = checkSum((uint8_t *)light_info, sizeof(LightInfo) - 3);
		send_wifi((uint8_t *)light_info, sizeof(LightInfo));	
		//printf("%s   ", aShowDate);
		//printf("%s\r\n", aShowTime);
		#endif
		g_trafficlight[dir][to].last_light_sec = g_trafficlight[dir][to].current_light_sec;

		return 1;
}
