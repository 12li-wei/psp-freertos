#include "FCSampleModule2.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule2";
static const char VERSION[20] = "V0001";
static FCSampleModule2* mInstance = NULL;

FCSampleModule2::FCSampleModule2():
	VModule(NAME, VERSION)
{
}

FCSampleModule2::~FCSampleModule2()
{
}

FCSampleModule2* FCSampleModule2::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule2();
	}
	return mInstance;
}

void FCSampleModule2::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule2::TaskCustomCallback(void const * argument)
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
	char data[][10] = {
		"aa",
		"bb",
		"cc",
		"dd",
		"ee",
		"ff",
		"gg",
		"hh",
		"ii",
		"jj"
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample2", 2, 10);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 2);
		if (idx >= 10) {
			idx = 0;
		}
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
}
