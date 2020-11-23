#ifndef _FC_SAMPLE_MODULE_1_H_
#define _FC_SAMPLE_MODULE_1_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule1: public VModule {
private:

	FCSampleModule1();
	~FCSampleModule1();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule1* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_1_H_ */
