#ifndef _V_MODULE_H_
#define _V_MODULE_H_

/*
 * Include file of VModule
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VNode.h"

#define MAX_MODULE_NUM 255

class VModule: public VNode {
protected:
	osThreadId mThreadId;
	uint8_t mModuleRunning;
	uint16_t mDurationMs;

public:
#ifdef config_STAT_MODULE_FREQ
	float mRealHz;
	uint32_t mRealHzCounter;
	uint64_t mLastHzRecordTs;
#endif

	VModule(const char* name, const char* version);
	~VModule();
	inline uint16_t getDuration() {return mDurationMs;}
	/**
	 * @brief:
	 *   Start to run the module
   * @param: durationMs-> duration time during each TaskCustomCallback
   * @param: priority-> priority of the VModule thread
	 * @retval:
	 *   none
	 **/
	virtual int start(uint16_t durationMs, osPriority priority);
	/**
	 * @brief:
	 *   Stop to run the module
	 * @retval:
	 *   none
	 **/
	virtual void stop();
	/**
	 * @brief:
	 *   Custom thread callback function
	 *   user module which inherit from the VModule must overwrite the function 
   * @param: argument pointer that is passed to the thread function as start argument.
	 * @retval:
	 *   none
	 **/
	virtual void TaskCustomCallback(void const * argument) = 0;

};
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
void dumpAllModules();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _V_MODULE_H_ */
