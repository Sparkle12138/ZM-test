#include <string.h>

#include "sys_timer.h"
#include "stm32f4xx.h"

/* 通讯设备 */
typedef struct{
	char *name;
	AssistDeviceState *state;
	void (*callBack)(void);
}AssistDevice;

/* 通讯设备数组 */
typedef struct{
	unsigned int size;
	const unsigned int maxSize;
	AssistDevice device[MAX_ASSIST_DEVICE];
}AssistDeviceArray;

/* 内部静态资源 */
static AssistDeviceArray __devices={0,MAX_ASSIST_DEVICE,{0}};

/**
 *@function 系统定时器初始化
 *@param 	unsigned int time_base	时基(US)
 *@return 	void
 */	
static void __sysTimerInit(unsigned int time_base)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = time_base;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 44;	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2,ENABLE);
}
 
/**
 *@function 设备退出监管
 *@param 	char *name				名字指针
 *@return 	执行结果
 */	
bool SysTimer_DeviceExit(char *name)
{
	if(name == NULL){return ERROR;}
	
	//检查该设备是否存在
	for(int i=0;i<__devices.size;i++)
	{
		if(0 == strcmp((void*)name,(void*)__devices.device[i].name))
		{			
			for(int j=i;j<__devices.size;j++)
			{
				memcpy(&__devices.device[j],&__devices.device[j+1],sizeof(AssistDevice));
			}
			__devices.size--;
			return true;
		}		
	}

	return false;
}	

/**
 *@function 设备注册监管
 *@param 	char *name				名字指针
 *			ComRecvState *state		状态指针
 *			void (*callBack)(void)	回调接口
 *@return 	执行结果
 */	
bool SysTimer_DeviceSignIn(char *name,AssistDeviceState *state,void (*callBack)(void))
{
	//指针有效判断
	if((name == NULL) || (state == NULL) || (callBack == NULL)){return false;}
	
	//查看内部资源是否还有空间添加新的设备
	if(__devices.size >= __devices.maxSize){return false;}
	
	//系统定时器初始化
	__sysTimerInit(SYS_TIMER_CLK_MS*1000);
	
	//查看该设备是否重复添加
	for(int i=0;i<__devices.size;i++)
	{	
		if(0 == strcmp(name,__devices.device[i].name)){return false;}		
	}
	
	//添加新的设备监管
	__devices.device[__devices.size].name = name;
	__devices.device[__devices.size].state = state;
	__devices.device[__devices.size].callBack = callBack;
	__devices.size++;

	//启动监管
	*state = ASSIST_DEVICE_STATE_START;
	
	return true;
}

/**
 *@function 系统定时器中断服务函数
 *@param 	void
 *@return 	void
 */	
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
		//更新设备通讯状态
		for(int i=0;i<__devices.size;i++)
		{
			//更新状态并查看是否触发
			if(*__devices.device[i].state == ASSIST_DEVICE_STATE_START)
			{
				*__devices.device[i].state = ASSIST_DEVICE_STATE_FINISH;
			}else if(*__devices.device[i].state == ASSIST_DEVICE_STATE_FINISH)
			{
				*__devices.device[i].state = ASSIST_DEVICE_STATE_NULL;
				__devices.device[i].callBack();				
			}
		}
	}
}
