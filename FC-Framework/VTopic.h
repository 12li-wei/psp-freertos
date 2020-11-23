#ifndef _V_TOPIC_H_
#define _V_TOPIC_H_

/*
 * Include file of VTopic
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VNode.h"

#define TOPIC_UPDATE 0x0001

#define WAIT_UPDATE(timeout) \
	osSignalWait(TOPIC_UPDATE, timeout)

#define NOTIFY_UPDATE(listener_id) \
	osSignalSet(listener_id, TOPIC_UPDATE)

typedef struct _TopicListener {
	void* listener_id;
	unsigned char read_idx;  		// listner read start index
	unsigned char need_notify;	// if need to notify listner when data update
	uint32_t total_read_count;

	struct _TopicListener* next;
} TopicListener_t;

typedef struct _TopicListenerList {
    TopicListener_t* head;
    TopicListener_t* tail;
    int count;			//0 --empty
    osMutexId list_mtx; // protect TopicListenerList_t
} TopicListenerList_t;

class VTopic: public VNode {

protected:
	uint8_t* mpTopicData;
	uint8_t mWriteIdx;
	uint32_t mTotalWriteCount;
	uint8_t mMaxTopicDataCnt;
	uint16_t mItemSize;
	uint8_t mSupportIsr;
	TopicListenerList_t mTopicListenerList;

private:

public:
	VTopic(const char* name, uint16_t item_size, uint16_t item_max_cnt, uint8_t support_isr = 0);
	~VTopic();

	uint8_t getDataRemainCnt(TopicListener_t* listener);
	/**
   * @param: listener_id-> the topic listener thread id
	 * @retval:
   *  return the pointer to TopicListener
	 **/
	TopicListener_t* findListener(osThreadId listener_id);
	/**
   * @param: listener_id-> the topic listener thread id
   * @param: need_notify-> if set to 1, the thread will be notify when data updated;
   *                 if set to 0, the thread will not be notify, just update 
	 *								 the data_updated flag in the listener's node, and the listener
	 *								 should check the flag by itself
	 * @retval:
   *   0-> if success
	 *  -1-> error number
	 **/
	int addTopicListener(osThreadId listener_id, uint8_t need_notify);

	void delTopicListener(osThreadId listener_id);
	/**
	 * @brief:
	 *   return the mpTopicData to caller from 0 to data_cnt
	 * @param: listener_id-> the topic listener thread id
   * @param: data_buf-> the buffer to store fetched data
   * @param: data_cnt-> indicate the count of data to fetch
   *
	 * @retval:
   *   0-> if success
	 *  -1-> error number
	 **/
	int fetchData(osThreadId listener_id, uint8_t* data_buf, uint8_t data_cnt);
	/**
	 * @brief:
	 *   copy the item_buf to mpTopicData at mWriteIdx position
	 *   and increase mWriteIdx
   * @param: item_buf-> the data buf need to update
   * @param: item_size-> data buf size, note that it must be equal to mItemSize;
   *
	 * @retval:
   *   0-> if success
	 *  -1-> error number
	 **/
  int updateData(uint8_t* item_buf, uint16_t item_size);

};

#endif /* _V_TOPIC_H_ */
