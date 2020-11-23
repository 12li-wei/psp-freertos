#include "FCDataLogManager.h"
#include "FCLogManager.h"
#include "topic_sys_status.h"

static const char NAME[20] = "FCDataLogManager";
static const char VERSION[20] = "V0001";
static FCDataLogManager* mInstance = NULL;

static int data_log_id = 0;
int FCDataLogManager::initDataLogHeader(DataLogHeader_t **head, unsigned short freq)
{
	DataLogHeader_t *h = NULL;

	h = (DataLogHeader_t *)pvPortMalloc(sizeof(DataLogHeader_t));
	if(h == NULL)
		return -1;
	h->next = NULL;
	h->item_cnt = 0;
	h->log_id = data_log_id++;
	h->logFreq = freq;

	return 0;
}

int FCDataLogManager::registerDataLog(char *describe, DataLogType_t logtype, DataLogHeader_t *logStart, VTopic* topic)
{
	DataLog_t *lognode = logStart->next;
	DataLog_t *lognodeNext = lognode;

	while(lognodeNext != NULL) {
		lognode = lognodeNext;
		lognodeNext = lognode->next;
	}

	lognodeNext = (DataLog_t *)pvPortMalloc(sizeof(DataLog_t));
	if(lognodeNext == NULL)
		return -1;
	lognodeNext->next = NULL;
	lognodeNext->log_type = logtype;
	lognodeNext->log_topic = topic;
	lognodeNext->index = logStart->item_cnt;
	strcpy(lognodeNext->describe, (const char *)describe);

	lognode->next = lognodeNext;
	logStart->item_cnt++;

	return 0;
}

void FCDataLogManager::writeDataLogHead(DataLogHeader_t *logHead, unsigned int *offset)
{
	char tmp_log[32] = {0};
	int log_len = 0;
	if(logHead->item_cnt > 0) {
		// write log id
		log_len = sprintf(tmp_log, "[ID_%d]\r\n", logHead->log_id);
		memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
		*offset += log_len;

		// write log freq info
		log_len = sprintf(tmp_log, "Freq=%d\r\n", logHead->logFreq);
		memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
		*offset += log_len;

		// write item count
		log_len = sprintf(tmp_log, "Item=%d\r\n",logHead->item_cnt);
		memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
		*offset += log_len;

		// write all log item info
		DataLog_t *now = logHead->next;
		for(int item=0;item < logHead->item_cnt;item++) {
			log_len = sprintf(tmp_log,"%d=[%s, %d]",item, now->describe, now->log_type);
			memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
			*offset += log_len;
			now = now->next;
		}
		// write end line flag
		log_len = sprintf(tmp_log,"\r\n");
		memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
		*offset += log_len;
	}
}

void FCDataLogManager::writeDataLogHeadEnd(unsigned int *offset)
{
	char tmp_log[32] = {0};
	int log_len = 0;
	log_len = sprintf(tmp_log,"\r\n/********************/\r\n");
	memcpy(mpDataLogBuf+(*offset), tmp_log, log_len);
	*offset += log_len;
}

FCDataLogManager::FCDataLogManager():
	VModule(NAME, VERSION)
{
	mDataLogTopicCnt = 0;
	for (int i=0; i<MAX_DATA_LOG_CNT; i++) {
		mDataLogTopic[i] = NULL;
	}
	mLog10hz = NULL;
	mLog40hz = NULL;
	mLog80hz = NULL;
	mLog100hz = NULL;
	mLog400hz = NULL;
	mHeadOffset = 0;
	mpDataLogBuf = (char *)pvPortMalloc(4096);
}

FCDataLogManager::~FCDataLogManager()
{
	mSdcardDev.close();
}

FCDataLogManager* FCDataLogManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCDataLogManager();
	}
	return mInstance;
}

void FCDataLogManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

int FCDataLogManager::start(uint16_t durationMs, osPriority priority)
{
	if (mModuleRunning == 1)
		return 0;

	// write data log header to file
	mHeadOffset = 0;
	if (mLog10hz) {
		writeDataLogHead(mLog10hz, &mHeadOffset);
	}
	if (mLog40hz) {
		writeDataLogHead(mLog40hz, &mHeadOffset);
	}
	if (mLog80hz) {
		writeDataLogHead(mLog80hz, &mHeadOffset);
	}
	if (mLog100hz) {
		writeDataLogHead(mLog100hz, &mHeadOffset);
	}
	if (mLog400hz) {
		writeDataLogHead(mLog400hz, &mHeadOffset);
	}
	writeDataLogHeadEnd(&mHeadOffset);

	return VModule::start(durationMs, priority);
}

void FCDataLogManager::printDataLogToDisk(char* data_log_buf, int buf_len)
{
}

static topic_data_log data_log_buf[MAX_DATA_LOG_CNT]={0};
void FCDataLogManager::fetchDataLog(DataLogHeader_t* head, unsigned int *offset)
{
	if(head == NULL)
		return;

	*offset = 0;
	DataLog_t *logdata = head->next;
	DataLogBuf_t tempbuf = {0};

	while(logdata != NULL) {
		if (logdata->log_topic != NULL) {
			int datacnt = checkTopic(logdata->log_topic, mThreadId, MAX_DATA_LOG_CNT);
			if (datacnt > 0) {
				datacnt = copyTopicData(logdata->log_topic, mThreadId, (uint8_t*)data_log_buf, datacnt);
				for (int i=0; i<datacnt; i++) {
					tempbuf.id = head->log_id;
					tempbuf.index = logdata->index;
					memcpy(tempbuf.val, data_log_buf[i].val, sizeof(tempbuf.val));
					memcpy(mpDataLogBuf+(*offset), &tempbuf, sizeof(DataLogBuf_t));
					(*offset) += sizeof(DataLogBuf_t);
				}
			}
		}
		logdata = logdata->next;
	}
}

void FCDataLogManager::TaskCustomCallback(void const * argument)
{
	if (mHeadOffset > 0) {
		TCHAR path[50] = {0};
		sprintf(path, "DATA_LOG_%lld.LOG", getTimeStamp());
		if (mSdcardDev.open(path, FA_WRITE | FA_CREATE_ALWAYS) == 0) {
			mSdcardDev.write(mpDataLogBuf, mHeadOffset);
		}
	}
	mHeadOffset = 0;
	if (mLog10hz) {
		fetchDataLog(mLog10hz, &mHeadOffset);
	}
	if (mLog40hz) {
		fetchDataLog(mLog40hz, &mHeadOffset);
	}
	if (mLog80hz) {
		fetchDataLog(mLog80hz, &mHeadOffset);
	}
	if (mLog100hz) {
		fetchDataLog(mLog100hz, &mHeadOffset);
	}
	if (mLog400hz) {
		fetchDataLog(mLog400hz, &mHeadOffset);
	}
	printDataLogToDisk(mpDataLogBuf, mHeadOffset);
	mHeadOffset = 0;

	VTopic* pTopic = subscribeTopic("sys_status", mThreadId, 1);
	if (pTopic != NULL) {
		int sysupdated = checkTopic(pTopic, mThreadId, MAX_SYS_STATUS_SIZE);
		if (sysupdated) {
			topic_sys_status sys_status;
			sysupdated = copyTopicData(pTopic, mThreadId, (uint8_t*)&sys_status, sysupdated);
			if (sys_status.usb_plugin) {
				printLogToUart(LOG_L_FAULT, "FCDataLogManager", "usb plug in");
			} else {
				printLogToUart(LOG_L_FAULT, "FCDataLogManager", "usb plug out");
			}
		}
	}
}

VTopic* FCDataLogManager::registerDataLogType(char* logdesc, DataLogType_t logtype, DataLogFreq_t logfreq)
{
	DataLogHeader_t *log_nhz = NULL;
	unsigned short freq = 0;
	switch(logfreq) {
		case FREQ_10_HZ:
			freq = 10;
			if (mLog10hz == NULL) {
				if (initDataLogHeader(&mLog10hz, freq) < 0)
					return NULL;
			}
			log_nhz = mLog10hz;
			break;
		case FREQ_40_HZ:
			freq = 40;
			if (mLog40hz == NULL) {
				if (initDataLogHeader(&mLog40hz, freq) < 0)
					return NULL;
			}
			log_nhz = mLog40hz;
			break;
		case FREQ_80_HZ:
			freq = 80;
			if (mLog80hz == NULL) {
				if (initDataLogHeader(&mLog80hz, freq) < 0)
					return NULL;
			}
			log_nhz = mLog80hz;
			break;
		case FREQ_100_HZ:
			freq = 100;
			if (mLog100hz == NULL) {
				if (initDataLogHeader(&mLog100hz, freq) < 0)
					return NULL;
			}
			log_nhz = mLog100hz;
			break;
		case FREQ_400_HZ:
			freq = 400;
			if (mLog400hz == NULL) {
				if (initDataLogHeader(&mLog400hz, freq) < 0)
					return NULL;
			}
			log_nhz = mLog400hz;
			break;
		default:
			return NULL;
	}
	if (mDataLogTopicCnt >= MAX_DATA_LOG_CNT)
		return NULL;

	char topicname[30]={0};
	snprintf(topicname, 30, "data_log_%d", mDataLogTopicCnt);
	mDataLogTopic[mDataLogTopicCnt] = adviseTopic(topicname, sizeof(topic_data_log), MAX_DATA_LOG_CNT, 1);
	VTopic* pTopic = subscribeTopic(topicname, mThreadId, 1);
	if (pTopic != NULL) {
		registerDataLog(logdesc, logtype, log_nhz, pTopic);
	}
	pTopic = mDataLogTopic[mDataLogTopicCnt++];

	return pTopic;
}
