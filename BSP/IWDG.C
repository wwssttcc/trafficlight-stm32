#include "main.h"	

__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;

//初始化独立看么狗
void IWDG_Init(u8 prer,u16 rlr)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组为2
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写	
	IWDG_SetPrescaler(prer); //设置IWDG分频系数,分频因子=4*2^prer，最大为256
	IWDG_SetReload(rlr);   //设置IWDG装载值
	IWDG_ReloadCounter(); //reload
	IWDG_Enable();  
  //	时间：Tout=((4*2^prer)*rlr)/32 (ms)
}
//喂狗函数
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
