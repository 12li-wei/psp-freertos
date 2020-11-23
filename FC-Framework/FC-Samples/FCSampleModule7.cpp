#include "FCSampleModule7.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSampleModule7";
static const char VERSION[20] = "V0001";
static FCSampleModule7* mInstance = NULL;

FCSampleModule7::FCSampleModule7():
	VModule(NAME, VERSION)
{
}

FCSampleModule7::~FCSampleModule7()
{
}

FCSampleModule7* FCSampleModule7::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSampleModule7();
	}
	return mInstance;
}

void FCSampleModule7::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

void FCSampleModule7::TaskCustomCallback(void const * argument)
{
/*
	VTopic* topic2 = subscribeTopic("sample2", mThreadId, 0);
	if (topic2 != NULL) {
		int checkrst = checkTopic(topic2, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic2, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", "sample2", checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sample2");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sample2");
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
	memset(uartlog, 0, 200);

	VTopic* topic3 = subscribeTopic("sample3", mThreadId, 0);
	if (topic3 != NULL) {
		int checkrst = checkTopic(topic3, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic3, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", "sample3", checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sample3");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sample3");
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
	memset(uartlog, 0, 200);

	VTopic* topic4 = subscribeTopic("sample4", mThreadId, 0);
	if (topic4 != NULL) {
		int checkrst = checkTopic(topic4, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic4, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", "sample4", checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sample4");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sample4");
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
	memset(uartlog, 0, 200);

	VTopic* topic5 = subscribeTopic("sample5", mThreadId, 0);
	if (topic5 != NULL) {
		int checkrst = checkTopic(topic5, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic5, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", "sample5", checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sample5");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sample5");
	}
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
	memset(uartlog, 0, 200);

	VTopic* topic6 = subscribeTopic("sample6", mThreadId, 0);
	if (topic6 != NULL) {
		int checkrst = checkTopic(topic6, mThreadId, 100);
		if (checkrst > 0) {
			char data[5*10]={0};
			copyTopicData(topic6, mThreadId, (uint8_t*)data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data=%s", "sample6", checkrst, data);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sample6");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sample6");
	}
*/
//	printLogToUart(LOG_L_DEBUG, LogModId_Mid, uartlog);
//	dumpAllTopics();
//	dumpAllModules();
//	dumpAllDevs();
}
