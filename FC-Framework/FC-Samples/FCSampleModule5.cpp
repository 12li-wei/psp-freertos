#include "FCSampleModule5.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule5";
static const char VERSION[20] = "V0001";
static FCSampleModule5* mInstance = NULL;

FCSampleModule5::FCSampleModule5():
	VModule(NAME, VERSION)
{
}

FCSampleModule5::~FCSampleModule5()
{
}

FCSampleModule5* FCSampleModule5::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule5();
	}
	return mInstance;
}

void FCSampleModule5::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule5::TaskCustomCallback(void const * argument)
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
	char data[][6] = {
		"o",
		"p",
		"q",
		"r",
		"s",
		"t",
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample5", 1, 6);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 1);
		if (idx >= 6) {
			idx = 0;
		}
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
}
