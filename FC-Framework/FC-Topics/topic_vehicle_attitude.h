#ifndef _TOPIC_VEHICLE_ATTITUDE_H
#define _TOPIC_VEHICLE_ATTITUDE_H

#include "stdint.h"

typedef struct _topic_vehicle_attitude {
	double rollspeed;
	double pitchspeed;
	double yawspeed;
	double q[4];
}topic_vehicle_attitude;

#endif /* _TOPIC_VEHICLE_ATTITUDE_H */
