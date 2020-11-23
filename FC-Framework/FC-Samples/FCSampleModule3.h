#ifndef _FC_SAMPLE_MODULE_3_H_
#define _FC_SAMPLE_MODULE_3_H_

#include "VModule.h"
#include "VTopic.h"

class FCSampleModule3: public VModule {
private:

	FCSampleModule3();
	~FCSampleModule3();

public:
	virtual void TaskCustomCallback(void const * argument);

	static FCSampleModule3* getInstance();
	static void destroyInstance();
};

#endif /* _FC_SAMPLE_MODULE_3_H_ */
