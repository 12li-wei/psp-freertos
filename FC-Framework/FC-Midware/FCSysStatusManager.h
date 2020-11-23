#ifndef _FC_SYS_STATUS_MANAGER_H_
#define _FC_SYS_STATUS_MANAGER_H_

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

class FCSysStatusManager: public VModule {

private:
	uint8_t mUsbPlugin;
	VTopic* mSysStatusTopic;
	FCSysStatusManager();
	~FCSysStatusManager();

public:
	virtual int start(uint16_t durationMs, osPriority priority);
	virtual void stop();
	virtual void TaskCustomCallback(void const * argument);

	static FCSysStatusManager* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SYS_STATUS_MANAGER_H_ */
