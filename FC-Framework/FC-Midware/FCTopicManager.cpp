#include "FCTopicManager.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCTopicManager";
static const char VERSION[20] = "V0001";
static FCTopicManager* mInstance = NULL;

FCTopicManager::FCTopicManager():
	VModule(NAME, VERSION)
{
	for (int i=0; i<MAX_TOPIC_NUM; i++) {
		mTopicList[i] = NULL;
	}
	osMutexDef_t mutexdef = {0};
	mTopicListMtx = osMutexCreate(&mutexdef);
}

FCTopicManager::~FCTopicManager()
{
	osMutexDelete(&mTopicListMtx);
}

FCTopicManager* FCTopicManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCTopicManager();
	}
	return mInstance;
}

void FCTopicManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

int FCTopicManager::start(uint16_t durationMs, osPriority priority)
{
	return 0;
}

void FCTopicManager::TaskCustomCallback(void const * argument)
{
}

VTopic* FCTopicManager::advTopic(char* topic_name, uint16_t item_size, uint16_t item_max_cnt, uint8_t can_noitfy)
{
	int i=0;
	char* pname;
	osMutexWait(mTopicListMtx, osWaitForever);
	for(i=0; i<MAX_TOPIC_NUM; i++) {
		// find the topic_name from mTopicList, if exist already, return it directly
		if (mTopicList[i] == NULL)
			break;
		pname = mTopicList[i]->getName() + strlen("/topic/");
		if(strcmp(pname, topic_name) == 0) {
			osMutexRelease(mTopicListMtx);
			return mTopicList[i];
		}
	}
	if (i == MAX_TOPIC_NUM) {
		osMutexRelease(mTopicListMtx);
		return NULL;
	}
	// if do not find, create a new topic and add it into mTopicList
	mTopicList[i] = new VTopic(topic_name, item_size, item_max_cnt, can_noitfy);
	osMutexRelease(mTopicListMtx);

	return mTopicList[i];
}

int FCTopicManager::pubTopic(VTopic* topic, uint8_t* data, uint16_t data_len)
{
	if (topic == NULL)
		return -1;

	return topic->updateData(data, data_len);
}
	
VTopic* FCTopicManager::subTopic(char* topic_name, osThreadId thread_id, uint8_t support_isr = 0)
{
	int i=0, find=0;
	char* pname;

	osMutexWait(mTopicListMtx, osWaitForever);
	for(i=0; i<MAX_TOPIC_NUM; i++) {
		if (mTopicList[i] == NULL)
			break;
		pname = mTopicList[i]->getName() + strlen("/topic/");
		if(strcmp(pname, topic_name) == 0) {
			find = 1;
			break;
		}
	}

	if (find == 0) {
		osMutexRelease(mTopicListMtx);
		return NULL;
	}

	mTopicList[i]->addTopicListener(thread_id, support_isr);
	osMutexRelease(mTopicListMtx);

	return mTopicList[i];
}

int FCTopicManager::checkTopic(VTopic* topic, osThreadId thread_id, uint16_t timeout)
{
	if (topic == NULL)
		return 0;
	
	TopicListener_t* listener = topic->findListener(thread_id);
	if (listener == NULL)
		return 0;

	if (listener->need_notify) {
		osEvent retevt = WAIT_UPDATE(timeout);
		if (retevt.status == osErrorTimeoutResource)
			return 0;
		else
			return topic->getDataRemainCnt(listener);
	} else {
		return topic->getDataRemainCnt(listener);
	}
}

int FCTopicManager::copyTopicData(VTopic* topic, osThreadId thread_id, uint8_t* ret_buf, uint8_t data_cnt)
{
	if (topic == NULL || ret_buf == NULL)
		return 0;

	return topic->fetchData(thread_id, ret_buf, data_cnt);
}

void FCTopicManager::dumpAllTopics()
{
	int i=0;
	char log[80];
	printLogToUart(LOG_L_DEBUG, "FCTopicManager", "======================");
	printLogToUart(LOG_L_DEBUG, "FCTopicManager", "Dump all topic info:");
	while(mTopicList[i]!=NULL && i<MAX_TOPIC_NUM) {
		sprintf(log, "|--%s", mTopicList[i]->getName());
		printLogToUart(LOG_L_DEBUG, "FCTopicManager", log);
		i++;
	}
	printLogToUart(LOG_L_DEBUG, "FCTopicManager", "======================");
}

VTopic* adviseTopic(char* topic_name, uint16_t item_size, uint16_t item_max_cnt, uint8_t support_isr)
{
	return FCTopicManager::getInstance()->advTopic(topic_name, item_size, item_max_cnt, support_isr);
}

int publishTopic(VTopic* topic, uint8_t* data, uint16_t data_len)
{
	return FCTopicManager::getInstance()->pubTopic(topic, data, data_len);
}

VTopic* subscribeTopic(char* topic_name, osThreadId thread_id, uint8_t need_notify)
{
	return FCTopicManager::getInstance()->subTopic(topic_name, thread_id, need_notify);
}

int checkTopic(VTopic* topic, osThreadId thread_id, uint16_t timeout)
{
	return FCTopicManager::getInstance()->checkTopic(topic, thread_id, timeout);
}

int copyTopicData(VTopic* topic, osThreadId thread_id,  uint8_t* ret_buf, uint8_t data_cnt)
{
	return FCTopicManager::getInstance()->copyTopicData(topic, thread_id, ret_buf, data_cnt);
}

void dumpAllTopics()
{
	FCTopicManager::getInstance()->dumpAllTopics();
}
