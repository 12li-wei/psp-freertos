#ifndef _TOPIC_SENSOR_MAG_H
#define _TOPIC_SENSOR_MAG_H

#include "stdint.h"

#define MAX_MAG_SIZE 20

typedef struct _topic_sensor_mag {
	uint64_t timestamp;
	int16_t x_raw;
	int16_t y_raw;
	int16_t z_raw;
}topic_sensor_mag;

#endif /* _TOPIC_SENSOR_MAG_H */
