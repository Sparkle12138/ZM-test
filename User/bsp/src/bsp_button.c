#include <stdbool.h>
#include "sys_timer.h"
#include "stm32f4xx.h"
#include "bsp_button.h"

#define BIT_EN Bit_SET
#define BINDING_API(A,B) (A=B)

#define CLK_PSC_MS	(10)

static Button __button={0};
static AssistDeviceState btnClock;
static void (*__listening)(ButtonEvent);
static unsigned int __press_time[BUTTON_SIZE];

static void bspf_GPIOInit(GPIO_TypeDef *port,unsigned short pin)
{
	//1.GPIO RCC enable
	RCC_AHB1PeriphClockCmd(0x01 << (((unsigned int)port 
		- (unsigned int)AHB1PERIPH_BASE)/0x400),ENABLE);
	
	//2.param config
	GPIO_InitTypeDef GpioInitStruct={0};
	GpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
	GpioInitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	//3.initialization GPIO
	GpioInitStruct.GPIO_Pin = pin;
	GPIO_Init(port,&GpioInitStruct);
}

static unsigned char bspf_buttonSoftwareScan(void)
{
	unsigned char state = 0;
	static unsigned char state_old = 0;

	if(BIT_EN == GPIO_ReadInputDataBit(BUTTON_1_GPIO,BUTTON_1_PIN))
	{  
		state |= BUTTON_1;
		__press_time[0] += CLK_PSC_MS;
	}else{
		__press_time[0] = 0;
	}
	
	if((state&0x7f) != state_old)
	{
		if(NULL != __listening){
			ButtonEvent event;		
					
			if((state&BUTTON_1) != (state_old&BUTTON_1)){	
				event.button_id = BUTTON_1;			
				if((state&BUTTON_1) == BUTTON_1){
					event.press_time = __press_time[0];
					if(event.press_time == CLK_PSC_MS){
						event.event_type = BUTTON_EVENT_PRESS;
					}else if((event.press_time>1000) && ((event.press_time%200) == 0)){
						event.event_type = BUTTON_EVENT_LONG;
					}
				}else{
					event.press_time = __press_time[0];	
					event.event_type = BUTTON_EVENT_RELEASE;
				}
				__listening(event);
			}
		}
		
		state_old = state&0x7f;
		*(unsigned char*)&__button.state = state | 0x80;		
	}else 
		return BUTTON_NO_CHANGE;
	
	return __button.state;
}

static void btnClkCallBack(void){
	
	static unsigned int psc=0;
	if(((++psc)%CLK_PSC_MS) == 0){
		bspf_buttonSoftwareScan();
	}
	
	btnClock=ASSIST_DEVICE_STATE_START;
};

static void scanCmd(bool en)
{
	if(en){
		SysTimer_DeviceSignIn("Button",&btnClock,btnClkCallBack);	
		btnClock = ASSIST_DEVICE_STATE_START;
	}else{
		SysTimer_DeviceExit("Button");
	}
}

static void setEventListening(void (*listening)(ButtonEvent))
{
	__listening = listening;
}

Button *getButtonHandle(void)
{
	static bool initFlag = false;
	
	if(true == initFlag){
		return &__button;
	}
	
	bspf_GPIOInit(BUTTON_1_GPIO,BUTTON_1_PIN);

	*(unsigned char*)&__button.state = 0x00;
	BINDING_API(__button.softwareScan,bspf_buttonSoftwareScan);
	BINDING_API(__button.scanCmd,scanCmd);
	BINDING_API(__button.setEventListening,setEventListening);
		
	initFlag = true;
	return &__button;
}
