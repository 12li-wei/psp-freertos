#ifndef _TOPIC_SYS_STATUS_H
#define _TOPIC_SYS_STATUS_H

#include "stdint.h"

#define MAX_SYS_STATUS_SIZE 1

typedef struct _topic_sys_status {
	uint8_t usb_plugin;
}topic_sys_status;

#endif /* _TOPIC_SYS_STATUS_H */
