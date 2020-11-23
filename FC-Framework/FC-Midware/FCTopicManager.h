#ifndef _FC_TOPIC_MANAGER_H_
#define _FC_TOPIC_MANAGER_H_

/*
 * Include file of FCTopicManager
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */

#include "VModule.h"
#include "VTopic.h"

#define MAX_TOPIC_NUM 255

class FCTopicManager: public VModule
{
private:
	VTopic *mTopicList[MAX_TOPIC_NUM];
	osMutexId mTopicListMtx; // protect mTopicList

private:
	FCTopicManager();
	~FCTopicManager();

public:
	virtual int start(uint16_t durationMs, osPriority priority);
	virtual void TaskCustomCallback(void const * argument);

	static FCTopicManager* getInstance();
	static void destroyInstance();
	/**
	 * @brief:
	 *   advise topic to TopicManager so that other modules can subscribe it
	 * @param: topic_name-> specify the topic name
	 * @param: item_size-> buf length need to write to the device
	 * @param: item_max_cnt-> buf length need to write to the device
   * @param: can_notify-> if set to 1, will notify listener when data update
                          if set to 0, do not notify any listener when data update
	 * @retval:
	 *   return the topic handler
	 **/
	VTopic* advTopic(char* topic_name, uint16_t item_size, uint16_t item_max_cnt, uint8_t support_isr = 0);
	/**
	 * @brief:
	 *   publish data to topic
	 * @param: topic-> topic handler which store the data
	 * @param: data-> publish data
	 * @param: data_len-> buf length need to write to the device
	 * @retval:
	 *   return real published count of data,
	 *   if no data return 0
	 **/
	int pubTopic(VTopic* topic, uint8_t* data, uint16_t data_len);
	/**
	 * @brief:
	 *   subscribe a topic
	 * @param: topic_name-> specify the topic name
	 * @param: thread_id-> the thread id of the subscriber module
	 * @param: need_notify-> if set to 1, the subscriber will be notified by signal if the topic is updated
	 *                       if set to 0, the subscriber should check it forwardly
	 * @retval:
	 *   return the topic handler,
	 *   return NULL if no relevant topic
	 **/
	VTopic* subTopic(char* topic_name, osThreadId thread_id, uint8_t need_notify);
	/**
	 * @brief:
	 *   check if the topic have been updated
	 * @param: topic-> topic handler which get from subTopic
	 * @param: thread_id-> the thread id of the subscriber module
	 * @param: timeout-> if need_notify set to 1, checkTopic will block to wait the data, and timeout is the max wait time
	 *                   if need_notify set to 0, timeout is not use
	 * @retval:
	 *   return the updated data count in topic
	 **/
	int checkTopic(VTopic* topic, osThreadId thread_id, uint16_t timeout);
	/**
	 * @brief:
	 *   copy the data of a topic
	 * @param: topic-> topic handler which get from subTopic
	 * @param: thread_id-> the thread id of the subscriber module
	 * @param: ret_buf-> store the data copied from the topic
	 * @param: data_cnt-> how many data need to copy, this should be get from checkTopic
	 * @retval:
	 *   return the topic handler,
	 *   return NULL if no relevant topic
	 **/
	int copyTopicData(VTopic* topic, osThreadId thread_id,  uint8_t* ret_buf, uint8_t data_cnt);
	/**
	 * @brief: dump all topic in topic list
	 *    none
	 **/
	void dumpAllTopics();

};

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
VTopic* adviseTopic(char* topic_name, uint16_t item_size, uint16_t item_max_cnt, uint8_t support_isr = 0);
int publishTopic(VTopic* topic, uint8_t* data, uint16_t data_len);
VTopic* subscribeTopic(char* topic_name, osThreadId thread_id, uint8_t need_notify);
int checkTopic(VTopic* topic, osThreadId thread_id, uint16_t timeout);
int copyTopicData(VTopic* topic, osThreadId thread_id,  uint8_t* ret_buf, uint8_t data_cnt);
void dumpAllTopics();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FC_TOPIC_MANAGER_H_ */
