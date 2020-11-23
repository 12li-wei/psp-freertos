#include "FCSampleModule4.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule4";
static const char VERSION[20] = "V0001";
static FCSampleModule4* mInstance = NULL;

FCSampleModule4::FCSampleModule4():
	VModule(NAME, VERSION)
{
}

FCSampleModule4::~FCSampleModule4()
{
}

FCSampleModule4* FCSampleModule4::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule4();
	}
	return mInstance;
}

void FCSampleModule4::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule4::TaskCustomCallback(void const * argument)
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
		"zzzz",
		"yyyy",
		"xxxx",
		"wwww",
		"vvvv",
		"uuuu",
		"tttt",
		"ssss",
		"rrrr",
		"qqqq"
	};
	static int idx = 0;
	VTopic* topic = adviseTopic("sample4", 5, 10);
	if (topic != NULL) {
		publishTopic(topic, (uint8_t*)data[idx++], 5);
		if (idx >= 10) {
			idx = 0;
		}
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
}
