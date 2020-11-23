#ifndef _FC_SAMPLE_MODULE_6_H_
#define _FC_SAMPLE_MODULE_6_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule6: public VModule {
private:

	FCSampleModule6();
	~FCSampleModule6();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule6* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_6_H_ */
