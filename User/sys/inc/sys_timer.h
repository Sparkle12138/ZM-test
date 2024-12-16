#ifndef __INCLUDE_SYS_TIMER_H__
#define __INCLUDE_SYS_TIMER_H__

#define SYS_TIMER_CLK_MS	1

#include <stdbool.h>

/* 最大辅助设备数量,数量过大影响运行速度 */
#define MAX_ASSIST_DEVICE 0x08

/* 辅助设备状态 */
typedef enum {
	ASSIST_DEVICE_STATE_NULL=0,
	ASSIST_DEVICE_STATE_START,
	ASSIST_DEVICE_STATE_FINISH,
}AssistDeviceState;

/* 函数接口 */
bool SysTimer_DeviceExit(char *name);
bool SysTimer_DeviceSignIn(char *name,AssistDeviceState *state,void (*callBack)(void));

#endif /* _INCLUDE_SYS_TIMER_H_ */
