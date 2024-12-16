#ifndef __BSP_BUTTON_H__
#define __BSP_BUTTON_H__

#include <stdbool.h>
#ifndef NULL
	#define NULL ((void*)0)
#endif /*NULL*/

#define BUTTON_SIZE	1

#define BUTTON_NO_CHANGE 0x00

#define BUTTON_1 0x01

#define BUTTON_1_GPIO	GPIOA
#define BUTTON_1_PIN	GPIO_Pin_0

typedef enum {
	BUTTON_EVENT_PRESS=0,
	BUTTON_EVENT_RELEASE,
	BUTTON_EVENT_LONG,
}ButtonEventType;

typedef struct {
	ButtonEventType event_type;
	unsigned int button_id;
	unsigned int press_time;	/*ms*/
}ButtonEvent;

typedef struct {
	const unsigned char state;
	
	void (*scanCmd)(bool en);
	unsigned char (*softwareScan)(void);		
	void (*setEventListening)(void (*listening)(ButtonEvent));
}Button;

Button *getButtonHandle(void);

#endif /* __BSP_BUTTON_H__ */
