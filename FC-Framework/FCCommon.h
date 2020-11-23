#ifndef _FC_COMMON_H_
#define _FC_COMMON_H_

/*
 *
 * Common include files used in fc-framework
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "FCErrorCode.h"

//#define config_STAT_MODULE_FREQ  1
#define config_USE_UART_LOG 1
#define config_USE_DISK_LOG 1
#define config_USE_GCS_LOG 1


#define MODULE_ENTRY(type) \
	type::getInstance() \

#define MODULE_START(type, dura, prio) \
	MODULE_ENTRY(type)->start(dura, prio)

#define MODULE_CALL(type, func) \
	MODULE_ENTRY(type)->func

#endif /* _FC_COMMON_H_ */
