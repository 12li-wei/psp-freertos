#ifndef _FC_SAMPLE_MODULE_2_H_
#define _FC_SAMPLE_MODULE_2_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule2: public VModule {
private:

	FCSampleModule2();
	~FCSampleModule2();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule2* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_2_H_ */
