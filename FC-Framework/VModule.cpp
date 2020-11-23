#include "VModule.h"
#include "FCLogManager.h"

#ifdef config_STAT_MODULE_FREQ
	#define FREQ_STAT_TIME 3 //s
#endif

void MyTaskCallback(void const * argument)
{
	VModule* ctx = (VModule*)argument;
	uint64_t duration=0, startstamp=0, endstamp=0;
	int64_t remain_delay_time=0;

#ifdef config_STAT_MODULE_FREQ
	char logbuf[80] = {0};
#endif
	if (ctx != NULL) {
		for(;;) {
			// call custome module callback func
			duration = ctx->getDuration();
			startstamp = ctx->getTimeStamp();
			ctx->TaskCustomCallback(argument);
			endstamp = ctx->getTimeStamp();

			// sleep remain time or yield the cpu to other thread
			if (duration >= 1) {
				remain_delay_time = duration-(endstamp-startstamp);
				if (remain_delay_time > 0) {
					osDelay(remain_delay_time);
				}
			} else {
				osThreadYield();
			}
#ifdef config_STAT_MODULE_FREQ
			// stat module freq
			if (ctx->mRealHzCounter > 0) {
				if ((ctx->getTimeStamp() - ctx->mLastHzRecordTs) >= FREQ_STAT_TIME*1000) {
					ctx->mRealHz = (float)ctx->mRealHzCounter / (float)FREQ_STAT_TIME;
					ctx->mRealHzCounter = 0;
					ctx->mLastHzRecordTs = ctx->getTimeStamp();
					sprintf(logbuf, "module[%s], freq=[%.3f]hz", ctx->getName(), ctx->mRealHz);
					printLogToUart(LOG_L_FAULT, "VModule", logbuf);
				}
			} else {
				ctx->mLastHzRecordTs = ctx->getTimeStamp();
			}
			ctx->mRealHzCounter++;
#endif
		}
	}
}

static VModule *mModuleList[MAX_MODULE_NUM] = {0};

VModule::VModule(const char* name, const char* version) :
	VNode(name, version)
{
	char myname[50]={0};
	strcpy(myname, "/module/");
	strcat(myname, mName);
	strcpy(mName, myname);

	mThreadId = NULL;
	mModuleRunning = 0;
	mDurationMs = 0;
#ifdef config_STAT_MODULE_FREQ
	mRealHzCounter = 0;
#endif
	int i=0;
	for(i=0; i<MAX_MODULE_NUM; i++) {
		if (mModuleList[i] == NULL)
			break;
	}
	mModuleList[i] = this;
}

VModule::~VModule()
{
	stop();
}

int VModule::start(uint16_t durationMs, osPriority priority)
{
	if (mModuleRunning == 1)
		return 0;

	mModuleRunning = 1;
	mDurationMs = durationMs;
	osThreadDef(myThreadDef, MyTaskCallback, priority, 0, 512);
	mThreadId = osThreadCreate(osThread(myThreadDef), this);

	return 0;
}

void VModule::stop()
{
	if (mModuleRunning == 0)
		return;

	if (mThreadId != NULL) {
		osThreadTerminate(mThreadId);
	}
	mModuleRunning = 0;
}

void dumpAllModules()
{
	int i=0;
	char log[80];
	printLogToUart(LOG_L_DEBUG, "VModule", "====================================================");
	printLogToUart(LOG_L_DEBUG, "VModule", "Dump all modules info:");
	while(mModuleList[i]!=NULL && i<MAX_MODULE_NUM) {
		sprintf(log, "|--%s, \t ver:[%s]", mModuleList[i]->getName(), mModuleList[i]->getVersion());
		printLogToUart(LOG_L_DEBUG, "VModule", log);
		i++;
	}
	printLogToUart(LOG_L_DEBUG, "VModule", "====================================================");
}
