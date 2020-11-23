#include "VTopic.h"

#include "FCLogManager.h"

static const char VERSION[20] = "V0001";

static void TopicListenerListInit(TopicListenerList_t* myroot)
{
	myroot->count = 0;
	myroot->head = NULL;
	myroot->tail = NULL;

	osMutexDef_t mutexdef = {0};
	myroot->list_mtx = osMutexCreate(&mutexdef);
}

//insert node at the tail
static void TopicListenerListInsert(TopicListenerList_t* myroot, TopicListener_t* mylistnode)
{
	mylistnode->next = NULL;

	myroot->count++;
	if(myroot->head == NULL) {
		myroot->head = mylistnode;
		myroot->tail = mylistnode;
	} else {
		myroot->tail->next = mylistnode;
		myroot->tail = mylistnode;
	}
}

#if 0
static int TopicListenerListDelete(TopicListenerList_t* myroot, TopicListener_t* mylistnode)
{
	// add mutex lock to protect contex
	TopicListener_t* pListenerNode = myroot->head;
	TopicListener_t* pre_listnode;

	//myroot is empty
	if(pListenerNode == NULL) {
		return 0;
	}

	//delete head node   
	if(pListenerNode == mylistnode)
	{
		myroot->count--;
		//myroot has only one node
		if(myroot->tail == mylistnode) {
			myroot->head = NULL;
			myroot->tail = NULL;
		}
		else {
			myroot->head = pListenerNode->next;
		}
		return 1;
	}

	while(pListenerNode != NULL) {
		if(pListenerNode == mylistnode) {
				break;
		}

		pre_listnode = pListenerNode;
		pListenerNode = pListenerNode->next;
	}
	if(pListenerNode == NULL) {
			return 0;
	}
	else
	{
		pre_listnode->next = pListenerNode->next;
		if(myroot->tail == pListenerNode) {
				myroot->tail = pre_listnode;
		}

		myroot->count--;
		return 1;
	}
	// end of contex protection
}
#endif

static TopicListener_t* newTopicListenerListNode(const void* listener_id, unsigned char need_notify)
{
	TopicListener_t* pListenerNode = (TopicListener_t*)pvPortMalloc(sizeof(TopicListener_t));
	if (pListenerNode == NULL)
		return pListenerNode;

	pListenerNode->listener_id = (void*)listener_id;
	pListenerNode->need_notify = need_notify;
	pListenerNode->read_idx = 0;
	pListenerNode->total_read_count = 0;

	return pListenerNode;
}

static void deleteTopicListenerListNode(TopicListener_t* mylistnode)
{
	mylistnode->next = NULL;
	vPortFree(mylistnode);
	mylistnode = NULL;
}

static TopicListener_t* findTopicListenerListNodeByListenerId(TopicListenerList_t* myroot, const void* listener_id)
{
	TopicListener_t* pListenerNode = myroot->head;
	while(pListenerNode != NULL) {
		if(pListenerNode->listener_id == listener_id) {
				break;
		}
		pListenerNode = pListenerNode->next;
	}

	return pListenerNode;
}

VTopic::VTopic(const char* name, uint16_t item_size, uint16_t item_max_cnt, uint8_t support_isr) 
	: VNode(name, VERSION)
{
	char myname[50]={0};
	strcpy(myname, "/topic/");
	strcat(myname, mName);
	strcpy(mName, myname);

	TopicListenerListInit(&mTopicListenerList);

	mWriteIdx = 0;
	mTotalWriteCount = 0;
	mMaxTopicDataCnt = item_max_cnt;
	mItemSize = item_size;
	mSupportIsr = support_isr;

	mpTopicData = (uint8_t*)pvPortMalloc(mItemSize*mMaxTopicDataCnt);

}

VTopic::~VTopic()
{
	if (mpTopicData != NULL) {
    vPortFree(mpTopicData);
		mpTopicData = NULL;
	}
}

TopicListener_t* VTopic::findListener(osThreadId listener_id)
{
	return findTopicListenerListNodeByListenerId(&mTopicListenerList, listener_id);
}

int VTopic::addTopicListener(osThreadId listener_id, uint8_t need_notify)
{
	osMutexWait(mTopicListenerList.list_mtx, osWaitForever);
	TopicListener_t* pListenerNode = findTopicListenerListNodeByListenerId(&mTopicListenerList, listener_id);
	if (pListenerNode != NULL) {
		osMutexRelease(mTopicListenerList.list_mtx);
		return SUCCESS;
	}

	pListenerNode = newTopicListenerListNode(listener_id, need_notify);
	if (pListenerNode == NULL) {
		osMutexRelease(mTopicListenerList.list_mtx);
		return -1;
	}
	TopicListenerListInsert(&mTopicListenerList, pListenerNode);

	osMutexRelease(mTopicListenerList.list_mtx);

	return SUCCESS;
}

void VTopic::delTopicListener(osThreadId listener_id)
{
	TopicListener_t* pListenerNode = findTopicListenerListNodeByListenerId(&mTopicListenerList, listener_id);
	if (pListenerNode != NULL) {
		deleteTopicListenerListNode(pListenerNode);
	}
}

uint8_t VTopic::getDataRemainCnt(TopicListener_t* listener)
{
	int32_t remain_cnt = mTotalWriteCount - listener->total_read_count;
	if (remain_cnt >= mMaxTopicDataCnt) {
		return mMaxTopicDataCnt;
	} else if (remain_cnt > 0) {
		return remain_cnt;
	} else {
		return 0;
	}
}

int VTopic::fetchData(osThreadId listener_id, uint8_t* data_buf, uint8_t data_cnt)
{
	TopicListener_t* pListenerNode = findTopicListenerListNodeByListenerId(&mTopicListenerList, listener_id);
	if (pListenerNode == NULL)
		return 0;

	if (data_cnt <= 0)
		return 0;

	// read count should not be greater than write count
	if (mTotalWriteCount <= pListenerNode->total_read_count)
		return 0;

	// check the remain count to avoid overread
	uint8_t remain_cnt = getDataRemainCnt(pListenerNode);
	if (data_cnt > remain_cnt)
		data_cnt = remain_cnt;

	// check if ring buffer is full, move read_idx to mWriteIdx
	if ((mTotalWriteCount-pListenerNode->total_read_count) >=  mMaxTopicDataCnt) {
		// if mTotalWriteCount is more than 1 round greater than pListenerNode->total_read_count, reduce it into 1 round
		pListenerNode->total_read_count = mTotalWriteCount - mMaxTopicDataCnt;
		pListenerNode->read_idx = mWriteIdx;
	}
	pListenerNode->total_read_count += data_cnt;

	// copy topic data to user buffer
	int round_cnt = (int)(pListenerNode->read_idx+data_cnt) - (int)mMaxTopicDataCnt;
	if (round_cnt < 0) { // copy it directly if data_cnt do not overstep mMaxTopicDataCnt
		memcpy(data_buf, mpTopicData+(pListenerNode->read_idx*mItemSize), mItemSize*data_cnt);
		pListenerNode->read_idx += data_cnt;
	} else { // copy it in two steps if data_cnt overstep mMaxTopicDataCnt
		memcpy(data_buf, mpTopicData+(pListenerNode->read_idx*mItemSize), mItemSize*(mMaxTopicDataCnt-pListenerNode->read_idx));
		uint8_t *pNextCpBuf = data_buf+mItemSize*(mMaxTopicDataCnt-pListenerNode->read_idx);
		pListenerNode->read_idx = 0;
		if (round_cnt > 0) {
			memcpy(pNextCpBuf, mpTopicData+(pListenerNode->read_idx*mItemSize), mItemSize*round_cnt);
			pListenerNode->read_idx += round_cnt;
		}
	}

	return data_cnt;
}

int VTopic::updateData(uint8_t* item_buf, uint16_t item_size)
{
	if (item_size != mItemSize)
		return ITEM_SIZE_NOT_RIGHT;

	TopicListener_t* pListenerNode;

	// copy data to topic buf
	memcpy(mpTopicData+(mWriteIdx*mItemSize), item_buf, mItemSize);
	// increate mWriteIdx
	if (++mWriteIdx >= mMaxTopicDataCnt)
		mWriteIdx = 0;

	// increase total count of write data
	mTotalWriteCount++;

	// notify all listeners
	if (!mSupportIsr) {
		pListenerNode = mTopicListenerList.head;
		while(pListenerNode != NULL) {
			if (pListenerNode->need_notify == 1 && pListenerNode->listener_id != NULL) {
				NOTIFY_UPDATE(pListenerNode->listener_id);
			}
			pListenerNode = pListenerNode->next;
		}
	}
	return 0;
}
