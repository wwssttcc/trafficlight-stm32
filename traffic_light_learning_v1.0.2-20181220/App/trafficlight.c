#include "main.h"

TrafficLight g_trafficlight[4][3];	//�ܹ�4��·�ڣ�ÿ��·����3����̵ƣ�����ǰ�У���գ��ҹ�
uint8_t g_online_num = 0;						//���̵�������
TrafficLightPos g_online[12];				//���̵����߷�λ

//��ȡ���̵�������������λ,�����ϱ���0123����ֱ�ң�012
uint8_t trafficLightNum()
{
	uint8_t i, j;
	uint32_t ret = 0, status;
	
	//������λ
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
				g_online[g_online_num - 1].dir = i / 3;
				g_online[g_online_num - 1].to = i % 3;
				break;
			}
		}
	}
	
	//�ϱ���λ
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
				g_online[g_online_num - 1].dir = i/3 + 2;
				g_online[g_online_num - 1].to = i%3;
				break;
			}
		}
	}
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
		
		//�жϺ����Ƿ����������ϵĵ���������о���������
		for(j = ret2; j < ret2 + 3; j++)
		{
			status = ret1 & (0x01 << j);
			if(status == 0)
			{
				zero_num++;
			}
			
			//���̵Ʋ����ܳ���ͬʱ��������
			if(zero_num > 1)
			{
				warn_msg("���̵�״̬����%d dir %d to %d %x\r\n", zero_num, dir, to, ret1);
				g_trafficlight[dir][to].error_num++;
				if(g_trafficlight[dir][to].status == 1)
					g_trafficlight[dir][to].error_num = 0;
				g_trafficlight[dir][to].status = 2;		//ͬʱ���������Ƽ����ϵĵ���
				if(g_trafficlight[dir][to].error_num == 2)
				{
					err_msg("���̵�״̬����,���³�ʼ�� %d dir %d to %d %x\r\n", g_trafficlight[dir][to].error_num, dir, to, ret1);
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
				g_trafficlight[dir][to].status = 1;			//���̵�״̬����
		}
		
		for(j = ret2; j < ret2 + 3; j++)
		{
			status = ret1 & (0x01 << j);

			if(status == 0)
			{
				g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
				g_trafficlight[dir][to].current_light = j % 3;
			}
		}
	}
}

void trafficLightInit()
{
	uint8_t i, j, zero_num = 0, ret = 1;
	uint32_t ret1 = 0, status = 0;
	uint8_t dir, to;
	uint8_t res1 = 1, res2 = 0;
	
	memset(g_trafficlight, 0, 12 * sizeof(TrafficLight));
	
	//��ʼ�������̵�״̬��Ϊunknown
	for(i = 0; i < 4; i++)
		for(j = 0; j < 3; j++)
			g_trafficlight[i][j].current_light = unknown;
	
	//trafficLightNum();
	#if 1
	while(res1 != res2)
	{
		res1 = trafficLightNum();		//������̵��������Լ���λ
		g_online_num = 0;
		OSTimeDlyHMSM(0, 0, 0, 100);
		res2 = trafficLightNum();
		//IWDG_Feed();
	}
	#endif
	info_msg("��λ˵����\r\n");
	info_msg("�����ϱ���0123\r\n");
	info_msg("��ֱ�ң�012\r\n");
	info_msg("��ǰ���ߺ��̵��� %d\r\n", g_online_num);
	trafficLightRead();	
	for(i = 0; i < g_online_num; i++)
	{
		dir = g_online[i].dir;
		to = g_online[i].to;
		g_trafficlight[dir][to].study_flag = 1;
		g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
		g_trafficlight[dir][to].current_seconds = 255;
		info_msg("dir %d to %d light %d %d\r\n", dir, to, g_trafficlight[dir][to].current_light, g_trafficlight[dir][to].study_flag);
	}	
		
	info_msg("���̵Ƴ�ʼ�����\r\n");
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
			//���ѧϰ��ʱ�䳬��10���ӣ�����Ϊѧϰ��ɣ��Һ��̵�״̬һֱΪ����
			if(g_trafficlight[dir][to].study_num > 30000)
			{
				g_trafficlight[dir][to].study_flag = 0;
				g_trafficlight[dir][to].status = 3;
				info_msg("ѧϰʱ�䳬��10���ӣ�ֹͣѧϰ��dir %d to %d Ϊ����\r\n", dir, to);
			}				
			
			//���ϴκ��̵�״̬�뵱ǰ���̵�״̬��һ�£�����Ϊ״̬�����ı�
			if(g_trafficlight[dir][to].last_light != g_trafficlight[dir][to].current_light)
			{
				g_trafficlight[dir][to].current_ticks = OSTimeGet();
				//�������״̬�µĺ��̵�start_ticks����Ϊ0
				if(g_trafficlight[dir][to].start_ticks != 0)
				{
					//�������ǰ���̵�����
					g_trafficlight[dir][to].light_period[g_trafficlight[dir][to].last_light] = (g_trafficlight[dir][to].current_ticks - g_trafficlight[dir][to].start_ticks + 50) / 100;
					g_trafficlight[dir][to].total_ticks = g_trafficlight[dir][to].current_ticks - g_trafficlight[dir][to].start_ticks;
					info_msg("dir %d to %d, light %d total_ticks %d\r\n", dir, to, g_trafficlight[dir][to].current_light, g_trafficlight[dir][to].total_ticks);
				}
				g_trafficlight[dir][to].start_ticks = OSTimeGet();
				//ֻ�к��̻����ڶ����������ѧϰ����
				if(g_trafficlight[dir][to].light_period[red] && g_trafficlight[dir][to].light_period[green] && g_trafficlight[dir][to].light_period[yellow])
				{
					info_msg("dir %d to %dѧϰ���\r\n", dir, to);
					info_msg("ѧϰ����Ϊ��red %d green %d yellow %d\r\n", g_trafficlight[dir][to].light_period[red], g_trafficlight[dir][to].light_period[green], g_trafficlight[dir][to].light_period[yellow]);
					g_trafficlight[dir][to].study_flag = 0;
				}
			}
			
		}
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
		//���̵Ʋ�ѧϰʱ��ֻ���¼ÿ��״̬�ı�ʱstart_ticks��ֵ
		if(g_trafficlight[dir][to].study_flag == 0)
		{
			if(g_trafficlight[dir][to].last_light != g_trafficlight[dir][to].current_light)
				g_trafficlight[dir][to].start_ticks = OSTimeGet();
		}
	}
}

uint8_t checkSum(uint8_t *pbuf, uint8_t len)
{
	uint8_t i, ret = 0;
	for(i = 0; i < len; i++)
	{
		ret = ret + pbuf[i];
	}
	return ret;
}

	#if 0
	if(light_info->last_seconds > 200 && g_trafficlight[dir][to].study_flag == 0) //���̵����ڱ��
	{
		g_trafficlight[dir][to].light_period[red] = 0;
		g_trafficlight[dir][to].light_period[green] = 0;
		g_trafficlight[dir][to].light_period[yellow] = 0;
		g_trafficlight[dir][to].study_flag = 1;
		g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
		g_trafficlight[dir][to].start_ticks = 0;
		info_msg("���̵����ڱ��dir %d to %d\r\n", dir, to);
	}
	

	for(i = 0; i < 3; i++)
		if(g_trafficlight[dir][to].last_seconds[i] > 0 && light_info->light != i && g_trafficlight[dir][to].study_flag == 0)
		{
			g_trafficlight[dir][to].light_period[red] = 0;
			g_trafficlight[dir][to].light_period[green] = 0;
			g_trafficlight[dir][to].light_period[yellow] = 0;
			g_trafficlight[dir][to].study_flag = 1;
			g_trafficlight[dir][to].last_light = g_trafficlight[dir][to].current_light;
			g_trafficlight[dir][to].start_ticks = 0;
			info_msg("���̵����ڱ�С��dir %d to %d\r\n", dir, to);
		}
	#endif

char start_led;
char start_time;
int start_ticks_qi;
extern struct tcp_pcb *g_pcb[3];
int trafficLightStatus(LightInfo *light_info, uint8_t num)
{
	uint8_t dir, to, i;
	uint32_t total_ticks = 0;
	uint8_t ret;
	uint8_t sec;
	dir = g_online[num].dir;
	to = g_online[num].to;
	
	light_info->direct = dir;
	light_info->to = to;
	light_info->light = g_trafficlight[dir][to].current_light;
	total_ticks = OSTimeGet() - g_trafficlight[dir][to].start_ticks;
	ret = total_ticks % 100;
	sec = total_ticks / 100;
	//if(ret > 5)		//ֻ����С��1s�����ʱ��Ÿ�ֵ
	//	return 2;
	if(sec == g_trafficlight[dir][to].current_seconds)
	{
		g_trafficlight[dir][to].current_seconds = sec;
		return 2;
	}
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
			light_info->light = 0;
		light_info->last_seconds = g_trafficlight[dir][to].light_period[light_info->light];
	}
	

		start_led = light_info->light;
		start_time = light_info->last_seconds;
		start_ticks_qi = OSTimeGet();

		if(light_info->light == red)
		{
			light_info->light = 'R';
			light_info->next_light = 'G';
			info_msg("���̵�ѧϰ����dir %d to %d:���ʣ��%02d�� %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
		}
		else if(light_info->light == green)
		{
			light_info->light = 'G';
			light_info->next_light = 'Y';
			info_msg("���̵�ѧϰ����dir %d to %d:�̵�ʣ��%02d�� %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
		}
		else
		{
			light_info->light = 'Y';
			light_info->next_light = 'R';
			info_msg("���̵�ѧϰ����dir %d to %d:�Ƶ�ʣ��%02d�� %d sec %d %d %d\r\n", dir, to, light_info->last_seconds, ret, sec, g_trafficlight[dir][to].last_light_sec, g_trafficlight[dir][to].current_light_sec);
		}
		
		light_info->to++;			//Э���Ϸ�����123����������012
		light_info->head1 = 0xA5;
		light_info->head2 = 0x5A;
		light_info->code1 = 0x83;
		light_info->code2 = 0xA0;
		light_info->code3 = 0x00;
		light_info->len = sizeof(LightInfo);
		light_info->tail1 = 0x5A;
		light_info->tail2 = 0xA5;
		#if 1
		//�����̵Ƴ�������ѧϰ��ʣ��ʱ�䶼��Ϊ1
		if(g_trafficlight[dir][to].study_flag == 1 || g_trafficlight[dir][to].status == 3)
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
		//send_wifi((uint8_t *)light_info, sizeof(LightInfo));	
		//printf("%s   ", aShowDate);
		//printf("%s\r\n", aShowTime);
		#endif
		g_trafficlight[dir][to].last_light_sec = g_trafficlight[dir][to].current_light_sec;
		for(i = 0; i < 3; i++)
		{
			if(g_pcb[i] != NULL)
			{
				tcp_write(g_pcb[i], light_info,sizeof(LightInfo),TCP_WRITE_FLAG_COPY);
				tcp_output(g_pcb[i]);
			}
		}
		memset(light_info, 0, sizeof(LightInfo));
		return 1;
}
