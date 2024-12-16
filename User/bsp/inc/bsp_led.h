#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#define LED1 0x01
#define LED2 0x02

#define LED_1_GPIO	GPIOG
#define LED_1_PIN	GPIO_Pin_13

#define LED_2_GPIO	GPIOG
#define LED_2_PIN	GPIO_Pin_14

#define LED_RED LED2
#define LED_GREEN LED1

#define LED_ERR LED_RED
#define LED_SYS LED_GREEN

#define LED_ALL (LED1|LED2)

typedef struct{
	const unsigned char state;
	void (*open)(unsigned char);
	void (*close)(unsigned char);
	void (*toggle)(unsigned char);
}LED;

LED* getLEDHandle(void);

#endif /* __BSP_LED_H__ */
