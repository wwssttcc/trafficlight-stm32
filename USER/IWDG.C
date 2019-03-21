#include "sys.h"

__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;

//��ʼ��������ô��
void IWDG_Init(u8 prer,u16 rlr)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����Ϊ2
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д	
	IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��,��Ƶ����=4*2^prer�����Ϊ256
	IWDG_SetReload(rlr);   //����IWDGװ��ֵ
	IWDG_ReloadCounter(); //reload
	IWDG_Enable();  
  //	ʱ�䣺Tout=((4*2^prer)*rlr)/32 (ms)
}
//ι������
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}