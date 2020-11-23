#ifndef _V_NODE_H_
#define _V_NODE_H_

/*
 * Include file of VNode
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include <stdlib.h>
#include <string.h>

#include "FCCommon.h"

class VNode {
protected:
	char mName[50];
	char mVersion[20];

public:
	VNode(const char* name, const char* version);
	char* getName();
	char* getVersion();
	uint64_t getTimeStamp();
	uint64_t getTimeStampUs();
};

#endif /* _V_NODE_H_ */
