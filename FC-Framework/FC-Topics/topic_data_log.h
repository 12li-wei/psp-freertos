#ifndef _TOPIC_DATA_LOG_H
#define _TOPIC_DATA_LOG_H

#include "stdint.h"

#define MAX_DATA_LOG_CNT 40

typedef struct _topic_data_log {
	uint8_t val[8];
}topic_data_log;

#endif /* _TOPIC_DATA_LOG_H */
