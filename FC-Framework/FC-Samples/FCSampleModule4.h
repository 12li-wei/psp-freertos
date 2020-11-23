#ifndef _FC_SAMPLE_MODULE_4_H_
#define _FC_SAMPLE_MODULE_4_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule4: public VModule {
private:

	FCSampleModule4();
	~FCSampleModule4();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule4* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_4_H_ */
