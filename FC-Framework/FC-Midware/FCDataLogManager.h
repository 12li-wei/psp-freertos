#ifndef _FC_DATA_LOG_MANAGER_H_
#define _FC_DATA_LOG_MANAGER_H_

/*
 * Include file of FCDataLogManager
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VModule.h"
#include "FCTopicManager.h"
#include "FCSdcardDev.h"
#include "topic_data_log.h"

#define MAX_FREQ 400

typedef enum _DataLogType {
	LOG_U8 = 0x01,
	LOG_S8,
	LOG_U16,
	LOG_S16,
	LOG_U32,
	LOG_S32,
	LOG_F32,
	LOG_F64,
	LOG_NULL,
} DataLogType_t;

typedef enum _DataLogFreq {
	FREQ_10_HZ = 0X01,
	FREQ_40_HZ,
	FREQ_80_HZ,
	FREQ_100_HZ,
	FREQ_400_HZ,
} DataLogFreq_t;

typedef struct _DataLogBuf {
	uint8_t id;
	uint8_t index;
	uint8_t val[8];
} DataLogBuf_t;

typedef struct _DataLog {
	struct _DataLog *next;	//next node
	DataLogType_t log_type;
	unsigned int index;
	VTopic* log_topic;	//log topic
	char describe[20];
} DataLog_t;

typedef struct _DataLogHeader {
	DataLog_t *next;
	unsigned short logFreq;
	unsigned char item_cnt;
	unsigned char log_id;
} DataLogHeader_t;

class FCDataLogManager: public VModule {
private:
	VTopic* mDataLogTopic[MAX_DATA_LOG_CNT];
	char* mpDataLogBuf;
	DataLogHeader_t* mLog10hz;
	DataLogHeader_t* mLog40hz;
	DataLogHeader_t* mLog80hz;
	DataLogHeader_t* mLog100hz;
	DataLogHeader_t* mLog400hz;

	unsigned int mHeadOffset;

	uint8_t mDataLogTopicCnt;
	FCSdcardDev mSdcardDev;

	FCDataLogManager();
	~FCDataLogManager();

private:
	int initDataLogHeader(DataLogHeader_t **head, unsigned short freq);
	int registerDataLog(char *describe, DataLogType_t logtype, DataLogHeader_t *logStart, VTopic* topic);
	void writeDataLogHead(DataLogHeader_t *logHead, unsigned int *offset);
	void writeDataLogHeadEnd(unsigned int *offset);
	void fetchDataLog(DataLogHeader_t* head, unsigned int *offset);
	void printDataLogToDisk(char* data_log_buf, int buf_len);

public:
	virtual int start(uint16_t durationMs, osPriority priority);
	virtual void TaskCustomCallback(void const * argument);

	VTopic* registerDataLogType(char* logdesc, DataLogType_t logtype, DataLogFreq_t logfreq);
	static FCDataLogManager* getInstance();
	static void destroyInstance();

};

#endif /* _FC_DATA_LOG_MANAGER_H_ */
