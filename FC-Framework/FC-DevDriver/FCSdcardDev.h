#ifndef _FC_SDCARD_DEV_H_
#define _FC_SDCARD_DEV_H_

#include "FCDev.h"
#include "fatfs.h"

class FCSdcardDev : public FCDev
{
private:
	FIL mFile;
	uint8_t mMode;

private:
	virtual int open();

public:
	FCSdcardDev();
	~FCSdcardDev();

	virtual int open(const TCHAR* path, uint8_t mode);
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_SDCARD_DEV_H_ */
