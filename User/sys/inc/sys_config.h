#ifndef __INCLUDE_SYS_CONFIG_H__
#define __INCLUDE_SYS_CONFIG_H__

#include <stdbool.h>

#ifndef NULL
	#define NULL 0
#endif /* NULL */

/* FLASH Physical space£¨The first address of the block£© */
#define FLASH_PHYSICAL_SPACE ((*(unsigned short*)0x1FFF7A22-1)*0x400+0x08000000)

#define DEVICE_ID_ADDRESS	((void*)0x1FFF7A10)

#define FIRMWARE_VERSION_FIELD_SIZE 0x40

/* System config information */
typedef struct {
	/* Initial Sequence Number */
	const unsigned int ISN;
	
	bool avtivete_record;
	
	bool avtivete_unlock;
	
	bool possession_sample;
	//firmware version
	char ver[FIRMWARE_VERSION_FIELD_SIZE];	
	
}SYS_INFOR;

SYS_INFOR* getSysInfor(void);
bool SaveSysInfor(SYS_INFOR *infor);
bool LoadSysInfor(SYS_INFOR *infor);
void FactoryResetCallback(SYS_INFOR *infor);

#endif /* __INCLUDE_SYS_CONFIG_H__ */
