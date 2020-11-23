#ifndef _FC_SAMPLE_MODULE_7_H_
#define _FC_SAMPLE_MODULE_7_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule7: public VModule {
private:

	FCSampleModule7();
	~FCSampleModule7();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule7* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_7_H_ */
