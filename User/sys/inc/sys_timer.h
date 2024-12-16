#ifndef __INCLUDE_SYS_TIMER_H__
#define __INCLUDE_SYS_TIMER_H__

#define SYS_TIMER_CLK_MS	1

#include <stdbool.h>

/* ������豸����,��������Ӱ�������ٶ� */
#define MAX_ASSIST_DEVICE 0x08

/* �����豸״̬ */
typedef enum {
	ASSIST_DEVICE_STATE_NULL=0,
	ASSIST_DEVICE_STATE_START,
	ASSIST_DEVICE_STATE_FINISH,
}AssistDeviceState;

/* �����ӿ� */
bool SysTimer_DeviceExit(char *name);
bool SysTimer_DeviceSignIn(char *name,AssistDeviceState *state,void (*callBack)(void));

#endif /* _INCLUDE_SYS_TIMER_H_ */
