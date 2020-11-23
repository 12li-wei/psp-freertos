#ifndef _FC_PARAM_MANAGER_H_
#define _FC_PARAM_MANAGER_H_

/*
 * Include file of FCParamManager
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VModule.h"
#include "FCTopicManager.h"

class FCParamManager: public VModule {
private:

	FCParamManager();
	~FCParamManager();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCParamManager* getInstance();
	static void destroyInstance();

};

#endif /* _FC_PARAM_MANAGER_H_ */
