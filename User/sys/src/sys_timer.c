#include <string.h>

#include "sys_timer.h"
#include "stm32f4xx.h"

/* ͨѶ�豸 */
typedef struct{
	char *name;
	AssistDeviceState *state;
	void (*callBack)(void);
}AssistDevice;

/* ͨѶ�豸���� */
typedef struct{
	unsigned int size;
	const unsigned int maxSize;
	AssistDevice device[MAX_ASSIST_DEVICE];
}AssistDeviceArray;

/* �ڲ���̬��Դ */
static AssistDeviceArray __devices={0,MAX_ASSIST_DEVICE,{0}};

/**
 *@function ϵͳ��ʱ����ʼ��
 *@param 	unsigned int time_base	ʱ��(US)
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
 *@function �豸�˳����
 *@param 	char *name				����ָ��
 *@return 	ִ�н��
 */	
bool SysTimer_DeviceExit(char *name)
{
	if(name == NULL){return ERROR;}
	
	//�����豸�Ƿ����
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
 *@function �豸ע����
 *@param 	char *name				����ָ��
 *			ComRecvState *state		״ָ̬��
 *			void (*callBack)(void)	�ص��ӿ�
 *@return 	ִ�н��
 */	
bool SysTimer_DeviceSignIn(char *name,AssistDeviceState *state,void (*callBack)(void))
{
	//ָ����Ч�ж�
	if((name == NULL) || (state == NULL) || (callBack == NULL)){return false;}
	
	//�鿴�ڲ���Դ�Ƿ��пռ�����µ��豸
	if(__devices.size >= __devices.maxSize){return false;}
	
	//ϵͳ��ʱ����ʼ��
	__sysTimerInit(SYS_TIMER_CLK_MS*1000);
	
	//�鿴���豸�Ƿ��ظ����
	for(int i=0;i<__devices.size;i++)
	{	
		if(0 == strcmp(name,__devices.device[i].name)){return false;}		
	}
	
	//����µ��豸���
	__devices.device[__devices.size].name = name;
	__devices.device[__devices.size].state = state;
	__devices.device[__devices.size].callBack = callBack;
	__devices.size++;

	//�������
	*state = ASSIST_DEVICE_STATE_START;
	
	return true;
}

/**
 *@function ϵͳ��ʱ���жϷ�����
 *@param 	void
 *@return 	void
 */	
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
		//�����豸ͨѶ״̬
		for(int i=0;i<__devices.size;i++)
		{
			//����״̬���鿴�Ƿ񴥷�
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
