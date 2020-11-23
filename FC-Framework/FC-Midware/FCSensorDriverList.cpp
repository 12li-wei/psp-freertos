#include "FCDev.h"
#include "FCMpu6500Dev.h"
#include "FCMagIst8310Dev.h"

#define SENSOR_INIT(type) \
	(FCDev*)new type() \

extern FCDev* gSensorList[] =
{
	SENSOR_INIT(FCMpu6500Dev),
	SENSOR_INIT(FCMagIst8310Dev),
};
extern const int gSensorCount = sizeof(gSensorList)/sizeof(FCDev*);
