#include "FCLogManager.h"

static const char NAME[20] = "FCLogManager";
static const char VERSION[20] = "V0001";
static FCLogManager* mInstance = NULL;

FCLogManager::FCLogManager():
	VModule(NAME, VERSION)
{
	mpGcsLogBuf = (char*)pvPortMalloc(LOG_MAX_SIZE);
	mpDiskLogBuf = (char*)pvPortMalloc(LOG_MAX_SIZE);

	mDiskLogCurWriteIdx = 0;
	mDiskLogCurReadIdx = 0;
	mDiskLogFullEmpty = 0;
}

FCLogManager::~FCLogManager()
{
	mUartDev.close();
	mSdcardDev.close();
}

FCLogManager* FCLogManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCLogManager();
	}
	return mInstance;
}

void FCLogManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

char log_buf_uart[LOG_MAX_ITEM]={0};
void FCLogManager::TaskCustomCallback(void const * argument)
{
	char *pbuf=NULL, *pbufend=NULL;
	int strsize = 0;

	// output log to disk
	mDiskLogFullEmpty = 1;
	if (mpDiskLogBuf != NULL && mDiskLogFullEmpty != 0) {
//		if (mDiskDev.open("mylogtest.txt", FA_CREATE_ALWAYS | FA_WRITE) != 0) {
/*
			if (mDiskLogFullEmpty != 0) {
				if (mDiskLogFullEmpty == 2) {
					mDiskLogCurReadIdx = mDiskLogCurWriteIdx;
				}
				pbuf = mpDiskLogBuf+mDiskLogCurReadIdx;
				pbufend = strstr(pbuf, LOG_MSG_STOP_F);
				if (pbufend == NULL) {
					pbufend = strstr(mpDiskLogBuf, LOG_MSG_STOP_F);
					strsize = pbufend-mpDiskLogBuf;
					memcpy(log_buf_uart, pbuf, LOG_MAX_SIZE-mDiskLogCurReadIdx);
					pbuf = log_buf_uart+(LOG_MAX_SIZE-mDiskLogCurReadIdx);
					memcpy(pbuf, mpDiskLogBuf, strsize);
					strsize += (LOG_MAX_SIZE-mDiskLogCurReadIdx);
				} else {
					strsize = pbufend-pbuf;
					strsize = MIN(strsize, LOG_MAX_ITEM);
					memcpy(log_buf_uart, pbuf, strsize);
				}
				if (mUartDev.write(log_buf_uart, strsize) > 0) {
					strsize += 2;
					mDiskLogCurReadIdx += strsize;
					if (mDiskLogCurReadIdx >= LOG_MAX_SIZE ) {
						mDiskLogCurReadIdx -= LOG_MAX_SIZE;
					}
					if (mDiskLogCurReadIdx >= mDiskLogCurWriteIdx) {
						mDiskLogCurReadIdx = mDiskLogCurWriteIdx;
						mDiskLogFullEmpty = 0;
					} else {
						mDiskLogFullEmpty = 1;
					}
				}
			}
*/
//		}
	}

	// output log to gcs
}

void FCLogManager::printLogToUart(LogLevel_t level, char* mod_name, char* log_data, int data_len)
{
	char lev[2]={0};
	if (level == LOG_L_FAULT) lev[0] = 'F';
	else if (level == LOG_L_ERROR) lev[0] = 'E';
	else if (level == LOG_L_WARN) lev[0] = 'W';
	else if (level == LOG_L_INFO) lev[0] = 'I';
	else if (level == LOG_L_DEBUG) lev[0] = 'D';

	if (mUartDev.open() >= 0) {
		uint64_t timestamp = getTimeStamp();
		char tempbuf[LOG_MAX_ITEM]={0};
		uint8_t buflen = snprintf(tempbuf, LOG_MAX_ITEM, "[%04llu.%04llu %s %s]: %s\n", timestamp>>10, timestamp&0x3FF, lev, mod_name, log_data);
		mUartDev.write(tempbuf, buflen);
	}
}

void FCLogManager::printLogToGcs(LogLevel_t level, char* mod_name, char* log_data, int data_len)
{
	
}

void FCLogManager::printLogToDisk(LogLevel_t level, char* mod_name, char* log_data, int data_len)
{
	char lev[2]={0};
	if (level == LOG_L_FAULT) lev[0] = 'F';
	else if (level == LOG_L_ERROR) lev[0] = 'E';
	else if (level == LOG_L_WARN) lev[0] = 'W';
	else if (level == LOG_L_INFO) lev[0] = 'I';
	else if (level == LOG_L_DEBUG) lev[0] = 'D';

	if (mpDiskLogBuf != NULL) {
		uint64_t timestamp = getTimeStamp();
		char tempbuf[LOG_MAX_ITEM]={0};
		uint8_t buflen = snprintf(tempbuf, LOG_MAX_ITEM, "[%04llu.%04llu %s %s]: %s\n", timestamp>>10, timestamp&0x3FF, lev, mod_name, log_data);
		int round_len = (mDiskLogCurWriteIdx+buflen) - LOG_MAX_SIZE;
		char *pbuf = mpDiskLogBuf+mDiskLogCurWriteIdx;
		if (round_len >= 0) {
			memcpy(pbuf, tempbuf, buflen-round_len);
			char *pbuf = tempbuf+(buflen-round_len);
			memcpy(mpDiskLogBuf, pbuf, round_len);
			mDiskLogCurWriteIdx = round_len;
			if (mDiskLogCurReadIdx < mDiskLogCurWriteIdx) {
				mDiskLogCurReadIdx = mDiskLogCurWriteIdx;
				mDiskLogFullEmpty = 2;
			} else {
				mDiskLogFullEmpty = 1;
			}
		} else {
			memcpy(pbuf, tempbuf, buflen);
			if (mDiskLogCurWriteIdx < mDiskLogCurReadIdx &&
					mDiskLogCurWriteIdx+buflen >= mDiskLogCurReadIdx) {
				mDiskLogFullEmpty = 2;
			} else {
				mDiskLogFullEmpty = 1;
			}
			mDiskLogCurWriteIdx += buflen;
		}
	}
}

void printLogToUart(LogLevel_t level, char* mod_name, char* log_data)
{
#ifdef config_USE_UART_LOG
	FCLogManager::getInstance()->printLogToUart(level, mod_name, log_data, strlen(log_data));
#endif
}

void printLogToDisk(LogLevel_t level, char* mod_name, char* log_data)
{
#ifdef config_USE_DISK_LOG
	FCLogManager::getInstance()->printLogToDisk(level, mod_name, log_data, strlen(log_data));
#endif
}
