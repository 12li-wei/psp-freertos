#include "FCSampleModule3.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule3";
static const char VERSION[20] = "V0001";
static FCSampleModule3* mInstance = NULL;

FCSampleModule3::FCSampleModule3():
	VModule(NAME, VERSION)
{
}

FCSampleModule3::~FCSampleModule3()
{
}

FCSampleModule3* FCSampleModule3::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule3();
	}
	return mInstance;
}

void FCSampleModule3::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule3::TaskCustomCallback(void const * argument)
{
/*
	// test subscriber
	char uartlog[200]={0};
	VTopic* topic = subscribeTopic("sample1", mThreadId, 0);
	if (topic != NULL) {
		int checkrst = checkTopic(topic, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", getName(), checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", getName());
		}
	} else {
		sprintf(uartlog, "[%s], no topic", getName());
	}
*/
	// test publisher
	char data[][4] = {
		"kkk",
		"lll",
		"mmm",
		"nnn",
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample3", 3, 4);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 3);
		if (idx >= 4) {
			idx = 0;
		}
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
}
