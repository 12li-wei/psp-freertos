#ifndef _FC_MAIN_MODULE_H_
#define _FC_MAIN_MODULE_H_

#include "VModule.h"
#include "VTopic.h"

class FCMainModule: public VModule {
private:

	FCMainModule();
	~FCMainModule();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCMainModule* getInstance();
	static void destroyInstance();
};

#endif /* _FC_MAIN_MODULE_H_ */
