#include "stm32f4xx.h"
#include "log.h"
#include "delay.h"
#include "sys_timer.h"
#include "sys_config.h"

#include "bsp_led.h"
#include "bsp_button.h"

#include <string.h>

#define SYS_CLK 1000

static AssistDeviceState sysClock;
static unsigned  int timeCount = 0;

/**
 *@function system colck base call back
 *@param 	void
 *@return 	void
 */	
static void sysClkCallBack(void){timeCount++;sysClock=ASSIST_DEVICE_STATE_START;};

/**
 *@function clear System Clock Flag
 *@param 	void
 *@return 	void
 */	
static void clearSystemClockFlag(void){timeCount = 0;}

/**
 *@function check system clock
 *@param 	void
 *@return 	flag
 */	
static FlagStatus getSystemClockFlag(void){if(timeCount > SYS_CLK)return SET;return RESET;}

/**
 *@function System initialization. It is recommended that all initialization operations be performed here.
 *@param 	void
 *@return 	void
 */	
static void System_Init(void)
{
	//等待系统供电稳定
	Delay_Ms(1000);	
	
	//设置中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//获取系统时钟速度
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	JLOG(LOG_INFOR,"System clock is %dHz",clocks.SYSCLK_Frequency);

	//系统作为设备注册
	SysTimer_DeviceSignIn("System",&sysClock,sysClkCallBack);
	JLOG(LOG_INFOR,"System start...");
	sysClock = ASSIST_DEVICE_STATE_START;
}

typedef struct {
	bool run;
	bool flicker;
	union {
		int duration;
		int flicker_number;
	}param;	
}LEDSeverControl;

static LEDSeverControl __red_led_server;
static LEDSeverControl __green_led_server;
static AssistDeviceState redLedClock;
static AssistDeviceState greenLedClock;

static void redLedClkCallBack(void)
{
	if(__red_led_server.run){
		if(__red_led_server.flicker){
			if(-1 != __red_led_server.param.duration){
				if(__red_led_server.param.duration > 0){
					__red_led_server.param.duration--;
					getLEDHandle()->open(LED_RED);
				}else{
					__red_led_server.run = false;
				}
			}
		}else{
			if(-1 != __red_led_server.param.flicker_number){				
				if(0 == (timeCount%250)){				
					if(__red_led_server.param.flicker_number > 0){
						__red_led_server.param.flicker_number--;
						getLEDHandle()->toggle(LED_RED);
					}else{
						__red_led_server.run = false;
					}
				}
			}		
		}
	}else{
		SysTimer_DeviceExit("RED_LED");
		getLEDHandle()->close(LED_RED);
	}
	
	redLedClock=ASSIST_DEVICE_STATE_START;
}

static void greenLedClkCallBack(void)
{
	if(__green_led_server.run){
		if(__green_led_server.flicker){
			if(-1 != __green_led_server.param.duration){
				if(__green_led_server.param.duration > 0){
					__green_led_server.param.duration--;
					getLEDHandle()->open(LED_GREEN);
				}else{
					__green_led_server.run = false;
				}
			}
		}else{
			if(-1 != __green_led_server.param.flicker_number){				
				if(0 == (timeCount%250)){				
					if(__green_led_server.param.flicker_number > 0){
						__green_led_server.param.flicker_number--;
						getLEDHandle()->toggle(LED_RED);
					}else{
						__green_led_server.run = false;
					}
				}
			}		
		}
	}else{
		SysTimer_DeviceExit("RED_GREEN");
		getLEDHandle()->close(LED_GREEN);
	}
	
	greenLedClock=ASSIST_DEVICE_STATE_START;
}

static void buttonListening(ButtonEvent event)
{
	LED *led = getLEDHandle();
	switch(event.button_id){
		case BUTTON_1:{
			switch(event.event_type){
				case BUTTON_EVENT_PRESS:{		
					
					break;
				}
			
				case BUTTON_EVENT_RELEASE:{				
					if(false == getSysInfor()->avtivete_unlock){
						getSysInfor()->avtivete_unlock = true;		
						__green_led_server.run = true;
						__green_led_server.flicker = false;
						__green_led_server.param.duration = 2000;	//常亮两秒
						SysTimer_DeviceSignIn("GREEN_LED",&greenLedClock,greenLedClkCallBack);
					}					
					break;
				}
				
				case BUTTON_EVENT_LONG:{
					if(3000 == event.press_time){
						getSysInfor()->avtivete_record = true;		
						__green_led_server.run = true;
						__green_led_server.flicker = true;
						__green_led_server.param.flicker_number = -1;	//持续闪烁
						SysTimer_DeviceSignIn("GREEN_LED",&greenLedClock,greenLedClkCallBack);
					}
					break;
				}
				
				default:break;
			}
		
			break;
		}
		
	
		default:return;
	}
}

int main(void)
{
	System_Init();	

	SYS_INFOR *sys_infor = getSysInfor();
	LED *led = getLEDHandle();
	Button *button = getButtonHandle();
	button->setEventListening(buttonListening);
	button->scanCmd(true);
	
	while(1)
	{		
		if(true == getSystemClockFlag()){
			clearSystemClockFlag();
			
			
		}	

		if(true == sys_infor->avtivete_record){
			/* 执行录制操作 */
			bool state = /*录制接口，接收录制是否成功的结果*/	true;
			
			Delay_Ms(3000);//实际操作请删该行
			
			__green_led_server.run = false;
			
			if(state){
				sys_infor->possession_sample = true;
			
				__red_led_server.run = true;
				__red_led_server.flicker = false;
				__red_led_server.param.duration = 2000;	//常亮2秒
				SysTimer_DeviceSignIn("RED_LED",&redLedClock,redLedClkCallBack);
				
				/* 再此添加录制成功的响应操作 */
			}else{
				/* 再此添加录制失败的响应操作 */
			}
		}
		
		if(true == sys_infor->avtivete_unlock){
			/* 执行解锁操作 */
			bool state = /*解锁接口，接收解锁是否成功的结果*/	false;
			
			Delay_Ms(3000);//实际操作请删该行
			
			__green_led_server.run = false;
			
			if(state){
				__green_led_server.run = true;
				__green_led_server.flicker = true;
				__green_led_server.param.flicker_number = 6;	//闪烁3次
				SysTimer_DeviceSignIn("GREEN_LED",&greenLedClock,greenLedClkCallBack);
								
				/* 再此添加解锁成功的响应操作 */
				
			}else{
				__red_led_server.run = true;
				__red_led_server.flicker = true;
				__red_led_server.param.flicker_number = 6;
				SysTimer_DeviceSignIn("RED_LED",&redLedClock,redLedClkCallBack);			
				
				/* 再此添加解锁失败的响应操作 */
				
			}
			
		}
	}
}

void FactoryResetCallback(SYS_INFOR *infor)
{
	memset(infor,0x00,sizeof(SYS_INFOR));

	
	strcpy(infor->ver,"V1.0");
}
