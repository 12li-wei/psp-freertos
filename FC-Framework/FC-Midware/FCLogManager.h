#ifndef _FC_LOG_MANAGER_H_
#define _FC_LOG_MANAGER_H_

/*
 * Include file of FCLogManager
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VModule.h"
#include "FCTopicManager.h"
#include "FCUartDev.h"
#include "FCSdcardDev.h"

typedef enum {
	LOG_L_FAULT = 0x00,
	LOG_L_ERROR = 0x01,
	LOG_L_WARN = 0x02,
	LOG_L_INFO = 0x03,
	LOG_L_DEBUG = 0x04,
}LogLevel_t;

#define LOG_MSG_START_F     "$$"
#define LOG_MSG_STOP_F      "##"

typedef struct log_msg_start {
	unsigned char startFlag[2];
	unsigned char logLen;
	unsigned char logLevel;
	unsigned char logMID;
	unsigned char logMIID;
} LogMsgStart;

#define LOG_MAX_ITEM 128
#define LOG_MAX_SIZE LOG_MAX_ITEM*20

typedef struct log_msg_end {
	unsigned long long timestamp;
	unsigned char stopFlag[2];
} LogMsgEnd;

class FCLogManager: public VModule {
private:
	char *mpGcsLogBuf;
	char *mpDiskLogBuf;
	int mDiskLogCurWriteIdx;
	int mDiskLogCurReadIdx;
	int mDiskLogFullEmpty; //0: empty, 1: have data, 2: full

	FCUartDev mUartDev;
	FCSdcardDev mSdcardDev;
	FCLogManager();
	~FCLogManager();

public:
	virtual void TaskCustomCallback(void const * argument);
	/**
	 * @brief:
	 *   print log to uart
	 * @param: level-> specify the log level: fault, error, warn, info, debug
	 * @param: mod_name-> specify module name
	 * @param: log_data-> log data
	 * @param: data_len-> log data length
	 * @retval:
	 *   return the topic handler,
	 *   return NULL if no relevant topic
	 **/
	void printLogToUart(LogLevel_t level, char* mod_name, char* log_data, int data_len);
	/**
	 * @brief:
	 *   print log to gcs
	 * @param: level-> specify the log level: fault, error, warn, info, debug
	 * @param: mod_name-> specify module name
	 * @param: log_data-> log data
	 * @param: data_len-> log data length
	 * @retval:
	 *   return the topic handler,
	 *   return NULL if no relevant topic
	 **/
	void printLogToGcs(LogLevel_t level, char* mod_name, char* log_data, int data_len);
	/**
	 * @brief:
	 *   print log to disk
	 * @param: level-> specify the log level: fault, error, warn, info, debug
	 * @param: mod_name-> specify module name
	 * @param: log_data-> log data
	 * @param: data_len-> log data length
	 * @retval:
	 *   return the topic handler,
	 *   return NULL if no relevant topic
	 **/
	void printLogToDisk(LogLevel_t level, char* mod_name, char* log_data, int data_len);

	static FCLogManager* getInstance();
	static void destroyInstance();

};

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
void printLogToUart(LogLevel_t level, char* mod_name, char* log_data);
void printLogToDisk(LogLevel_t level, char* mod_name, char* log_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FC_LOG_MANAGER_H_ */
