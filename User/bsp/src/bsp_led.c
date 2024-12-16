#include <stdbool.h>
#include "bsp_led.h"
#include "stm32f4xx.h"

#define BINDING_API(A,B) (A=B)

static LED __led={0};

static void bspf_GPIOInit(GPIO_TypeDef *port,unsigned int pin)
{
	//1.GPIO RCC enable
	RCC_AHB1PeriphClockCmd(0x01 << (((unsigned int)port 
		- (unsigned int)AHB1PERIPH_BASE)/0x400),ENABLE);
	
	//2.param config
	GPIO_InitTypeDef GpioInitStruct={0};
	GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GpioInitStruct.GPIO_OType = GPIO_OType_PP;
	GpioInitStruct.GPIO_Speed = GPIO_High_Speed;
	GpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	
	//3.initialization GPIO
	GpioInitStruct.GPIO_Pin = pin;
	GPIO_Init(port,&GpioInitStruct);
}

/**
 *@function led to open
 *@param 	unsigned char led_id 
 *@return 	void
 */	
static void bspf_ledOpen(unsigned char led_id)
{
	if(LED1 == (led_id&LED1))
	{
		GPIO_SetBits(LED_1_GPIO,LED_1_PIN);
	}
	
	if(LED2 == (led_id&LED2))
	{
		GPIO_SetBits(LED_2_GPIO,LED_2_PIN);
	}
	
	*(unsigned char*)&__led.state = ((__led.state&(~led_id))&LED_ALL);
}

/**
 *@function led to close
 *@param 	unsigned char led_id 
 *@return 	void
 */	
static void bspf_ledClose(unsigned char led_id)
{
	if(LED1 == (led_id&LED1))
	{
		GPIO_ResetBits(LED_1_GPIO,LED_1_PIN);
	}
	
	if(LED2 == (led_id&LED2))
	{
		GPIO_ResetBits(LED_2_GPIO,LED_2_PIN);
	}
		
	*(unsigned char*)&__led.state = ((__led.state|led_id)&LED_ALL);
}

static void bspf_ledToggle(unsigned char led_id)
{
	if(LED1 == (led_id&LED1)){
		GPIO_ToggleBits(LED_1_GPIO,LED_1_PIN);
	}
	
	if(LED2 == (led_id&LED2)){
		GPIO_ToggleBits(LED_2_GPIO,LED_2_PIN);
	}

	*(unsigned char*)&__led.state = ((__led.state^led_id)&LED_ALL);
}

/**
 *@function get LED control handle
 *@param 	void
 *@return 	void
 */	
LED* getLEDHandle(void)
{
	static bool initFlag = false;
	if(true == initFlag){
		return &__led;
	}
	
	bspf_GPIOInit(LED_1_GPIO,LED_1_PIN);
	bspf_GPIOInit(LED_2_GPIO,LED_2_PIN);

	bspf_ledClose(LED_ALL);
	
	BINDING_API(__led.open,bspf_ledOpen);
	BINDING_API(__led.close,bspf_ledClose);
	BINDING_API(__led.toggle,bspf_ledToggle);
	
	initFlag = true;
	return &__led;
}
