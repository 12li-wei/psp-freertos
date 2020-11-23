#include "VNode.h"

VNode::VNode(const char* name, const char* version)
{
	int size = 0;
	size = strlen(name);
	size = MIN(size, 50);
	memset(mName, 0, 50);
	strncpy(mName, name, size);

	size = strlen(version);
	size = MIN(size, 20);
	memset(mVersion, 0, 20);
	strncpy(mVersion, version, size);
}

char* VNode::getName()
{
	return mName;
}

char* VNode::getVersion()
{
	return mVersion;
}

uint64_t VNode::getTimeStamp()
{
	uint32_t cur_sys_tick = osKernelSysTick();
	return (uint64_t)cur_sys_tick * portTICK_PERIOD_MS;	
}

uint64_t VNode::getTimeStampUs()
{
	uint64_t ts_us = 0;
	ts_us = (SystemCoreClock/1000) - SysTick->VAL;
	ts_us /= 216;
	ts_us += getTimeStamp()*1000;

	return ts_us;
}
