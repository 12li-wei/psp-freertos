#include "FCSampleModule1.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule1";
static const char VERSION[20] = "V0001";
static FCSampleModule1* mInstance = NULL;

FCSampleModule1::FCSampleModule1():
	VModule(NAME, VERSION)
{
}

FCSampleModule1::~FCSampleModule1()
{
}

FCSampleModule1* FCSampleModule1::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule1();
	}
	return mInstance;
}

void FCSampleModule1::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule1::TaskCustomCallback(void const * argument)
{
	char data[][10] = {
		"00000",
		"11111",
		"22222",
		"33333",
		"44444",
		"55555",
		"66666",
		"77777",
		"88888",
		"99999"
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample1", 5, 10);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 5);
		if (idx >= 10) {
			idx = 0;
		}
	}
}
