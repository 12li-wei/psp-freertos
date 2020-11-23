#ifndef _TOPIC_SENSOR_BARO_H
#define _TOPIC_SENSOR_BARO_H

#include "stdint.h"

typedef struct _topic_sensor_baro {
	float pressure;
	float altitude;
	float temperature;
}_topic_sensor_baro;

#endif /* _TOPIC_SENSOR_BARO_H */
