#ifndef _TOPIC_SENSOR_GPS_H
#define _TOPIC_SENSOR_GPS_H

#include "stdint.h"

typedef struct _topic_sensor_gps {
	unsigned char gpsfixmode;
	double longitude;
	double latitude;
	float altitude;
	float northvelocity;
	float eastvelocity;
	float downvelocity;
	float diffvelocity[3];
	unsigned int gpstime;
	unsigned int measureticktime;
	unsigned int systemtime;
	unsigned char satellitenumber;
	float pdop;
	float hdop;
	float vdop;
	float positionaccuracy;
	float speedaccuracy;
	float horizontalaccuracy;
	float verticalaccuracy;
	float groundspeed;
	float heading;
	float courseaccuracy;
}topic_sensor_imu;

#endif /* _TOPIC_SENSOR_GPS_H */
