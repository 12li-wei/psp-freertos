#ifndef _FC_SENSOR_MANAGER_H_
#define _FC_SENSOR_MANAGER_H_

/*
 * Include file of FCSensorManager
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VModule.h"
#include "FCDev.h"
#include "FCTopicManager.h"

class FCSensorManager: public VModule {

private:
	VTopic* mImuTopic[2];
	VTopic* mMagTopic;
	VTopic* mBaroTopic;
	FCSensorManager();
	~FCSensorManager();

public:
	void registerImu();
	virtual int start(uint16_t durationMs, osPriority priority);
	virtual void stop();
	virtual void TaskCustomCallback(void const * argument);

	static FCSensorManager* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SENSOR_MANAGER_H_ */
