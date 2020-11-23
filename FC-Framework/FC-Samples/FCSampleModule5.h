#ifndef _FC_SAMPLE_MODULE_5_H_
#define _FC_SAMPLE_MODULE_5_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule5: public VModule {
private:

	FCSampleModule5();
	~FCSampleModule5();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule5* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_5_H_ */
