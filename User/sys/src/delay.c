#include "delay.h"
#include "stm32f4xx.h"

typedef enum{
	DELAY_US = 0,	//US DELAY
	DELAY_MS = 1,	//MS DELAY
}DelayType;

static void DelayInit(DelayType dType)
{
	unsigned int sysCoreClock = SystemCoreClock;
	
	switch(dType){
		case DELAY_US:sysCoreClock /= 1000000;break;
		case DELAY_MS:sysCoreClock /= 1000;break;
		default:return;
	}
		
	SysTick_Config(sysCoreClock);
}

static void _delayClose(void)
{
	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

void Delay_Us(unsigned int us)
{
	DelayInit(DELAY_US);

	for(;us>0;us--)
		while(!((SysTick->CTRL)&(1<<16)));

	_delayClose();
}

/**
 *@function ms¼¶ÑÓÊ±º¯Êý 
 *@param 	unsigned int ms msTime
 *@return 	void
 */	
void Delay_Ms(unsigned int ms)
{
	DelayInit(DELAY_MS);
	
	for(;ms>0;ms--)
		while(!((SysTick->CTRL)&(1<<16)));
	
	_delayClose();
}
