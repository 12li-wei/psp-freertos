#ifndef _FC_UART_DEV_H_
#define _FC_UART_DEV_H_

#include "FCDev.h"

class FCUartDev : public FCDev
{
private:

public:
	FCUartDev();
	~FCUartDev();

	virtual int open();
	virtual void close();
	virtual int read(char* buf, uint16_t buf_len);
	virtual int write(char* buf, uint16_t buf_len);
};

#endif /* _FC_UART_DEV_H_ */
