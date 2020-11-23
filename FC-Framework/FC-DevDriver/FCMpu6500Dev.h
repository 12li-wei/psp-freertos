#ifndef _FC_MPU6500_DEV_H_
#define _FC_MPU6500_DEV_H_

#include "FCDev.h"

class FCMpu6500Dev : public FCDev
{
private:

public:
	FCMpu6500Dev();
	~FCMpu6500Dev();

	virtual int open();
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_MPU6500_DEV_H_ */
