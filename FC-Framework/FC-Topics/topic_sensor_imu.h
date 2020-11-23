#ifndef _TOPIC_SENSOR_IMU_H
#define _TOPIC_SENSOR_IMU_H

#include "stdint.h"

#define MAX_IMU_SIZE 25

typedef struct _topic_sensor_imu {
	uint64_t timestamp;
	uint16_t gyro[3];
	uint16_t acc[3];
	uint16_t temp;	
}topic_sensor_imu;

#endif /* _TOPIC_SENSOR_IMU_H */
