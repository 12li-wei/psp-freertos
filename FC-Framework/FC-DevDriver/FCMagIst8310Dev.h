#ifndef _FC_MAG_IST8310_DEV_H_
#define _FC_MAG_IST8310_DEV_H_

#include "FCDev.h"

class FCMagIst8310Dev : public FCDev
{
private:

public:
	FCMagIst8310Dev();
	~FCMagIst8310Dev();

	virtual int open();
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_MAG_IST8310_DEV_H_ */
