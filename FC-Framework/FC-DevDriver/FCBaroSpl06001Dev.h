#ifndef _FC_BARO_SPL06001_DEV_H_
#define _FC_BARO_SPL06001_DEV_H_

#include "FCDev.h"

class FCBaroSpl06001Dev : public FCDev
{
private:

public:
	FCBaroSpl06001Dev();
	~FCBaroSpl06001Dev();

	virtual int open();
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_BARO_SPL06001_DEV_H_ */
