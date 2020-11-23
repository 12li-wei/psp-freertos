#ifndef _FC_DEV_H_
#define _FC_DEV_H_

/*
 * Include file of FCDev
 *
 * Copyright (C) 2017 Beijing Fimi Ltd.
 * Author: Wei Li <liwei@fimi.cn>
 *
 */
typedef enum _Dev_Type {
	IMU = 0x01,
	MAG,
	BARO,
	GPS,
	SONAR,
	DISK,
	UART
} DevType_t;

#include "VNode.h"
#define MAX_DEV_NUM 255

class FCDev: public VNode {
	
protected:
	bool mDevOpened;
	DevType_t mDevType;

public:
	FCDev(const char* name, const char* version, DevType_t dev_type);
	~FCDev();
	inline DevType_t getDevType() {return mDevType;}
	/**
	 * @brief:
	 *   interface to open the device
	 * @param: timer_callback-> timer handler callback, if set to NULL, dev driver should not use timer
	 * @retval:
	 *   0: open success
	 *  <0: error code
	 **/
	virtual int open() = 0;
	/**
	 * @brief:
	 *   interface to close the device
	 * @retval:
	 *   none
	 **/
	virtual void close() = 0;
	/**
	 * @brief:
	 *   interface to read data from device
	 * @param: buf-> buf to store the data
	 * @param: buf_len-> buf max len of buf,
	 *				 make sure it is enough to store the data from device
	 * @param: dma_callback-> dma callback, if set to NULL, dev driver will not use dma reader
	 * @retval:
	 *   return the real data len readed
	 **/
	virtual int read(char* buf, uint16_t buf_len) = 0;
	/**
	 * @brief:
	 *   interface to write data to device
	 * @param: buf-> buf to store the data
	 * @param: buf_len-> buf length need to write to the device
	 * @param: dma_callback-> dma callback, if set to NULL, dev driver will not use dma writer
	 * @retval:
	 *   return the real data len be written
	 **/
	virtual int write(char* buf, uint16_t buf_len) = 0;
};
#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
void dumpAllDevs();
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FC_DEV_H_ */
