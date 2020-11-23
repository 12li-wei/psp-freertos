#ifndef _FC_BARO_MS5611_DEV_H_
#define _FC_BARO_MS5611_DEV_H_

#include "FCDev.h"

class FCBaroMs5611Dev : public FCDev
{
private:

public:
	FCBaroMs5611Dev();
	~FCBaroMs5611Dev();

	virtual int open();
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_BARO_MS5611_DEV_H_ */
