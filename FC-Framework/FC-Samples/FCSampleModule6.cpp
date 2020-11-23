#include "FCSampleModule6.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule6";
static const char VERSION[20] = "V0001";
static FCSampleModule6* mInstance = NULL;

FCSampleModule6::FCSampleModule6():
	VModule(NAME, VERSION)
{
}

FCSampleModule6::~FCSampleModule6()
{
}

FCSampleModule6* FCSampleModule6::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule6();
	}
	return mInstance;
}

void FCSampleModule6::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule6::TaskCustomCallback(void const * argument)
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
		"00000a",
		"11111b",
		"22222c",
		"33333d",
		"44444e",
		"55555f",
		"66666g",
		"77777h",
		"88888i",
		"99999j"
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample6", 6, 10);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 6);
		if (idx >= 10) {
			idx = 0;
		}
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
}
