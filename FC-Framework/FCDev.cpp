#include "FCDev.h"
#include "FCLogManager.h"

static FCDev *mDevList[MAX_DEV_NUM] = {0};

FCDev::FCDev(const char* name, const char* version, DevType_t dev_type)
	:VNode(name, version)
{
	char myname[50]={0};
	strcpy(myname, "/dev/");
	strcat(myname, mName);
	strcpy(mName, myname);

	mDevOpened = false;
	mDevType = dev_type;

	int i=0;
	for(i=0; i<MAX_DEV_NUM; i++) {
		if (mDevList[i] == NULL)
			break;
	}
	mDevList[i] = this;
}

FCDev::~FCDev()
{
}

void dumpAllDevs()
{
	int i=0;
	char log[80];
	printLogToUart(LOG_L_DEBUG, "FCDev", "====================================================");
	printLogToUart(LOG_L_DEBUG, "FCDev", "Dump all devices info:");
	while(mDevList[i]!=NULL && i<MAX_DEV_NUM) {
		sprintf(log, "|--%s, \t ver:[%s]", mDevList[i]->getName(), mDevList[i]->getVersion());
		printLogToUart(LOG_L_DEBUG, "FCDev", log);
		i++;
	}
	printLogToUart(LOG_L_DEBUG, "FCDev", "====================================================");
}
