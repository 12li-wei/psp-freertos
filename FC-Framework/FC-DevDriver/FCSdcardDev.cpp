#include "FCSdcardDev.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCSdcardDev";
static const char VERSION[20] = "V0001";

extern "C" void Error_Handler(void);

static FATFS sdcard_fs;
static int sdcard_mounted = 0;

static char logbuf[50] = {0};	
FCSdcardDev::FCSdcardDev()
	: FCDev(NAME, VERSION, DISK)
{

}

FCSdcardDev::~FCSdcardDev()
{

}

int FCSdcardDev::open()
{
	return 0;
}

int FCSdcardDev::open(const TCHAR* path, uint8_t mode)
{
	if (mDevOpened)
		return 0;

	FRESULT res;
	int try_time = 0;
	
	if (!sdcard_mounted) { // mount the sdcard
		while (1) {
			if (try_time++ > 5)
				break;
			res = f_mount(&sdcard_fs, SD_Path, 1);
			if (res != FR_OK) {
				osDelay(500);
			} else {
				sdcard_mounted = 1;
				break;
			}
		}
	}
	if (!sdcard_mounted) {
		sprintf(logbuf, "can't mount fs [%s], try_time=%d, last error=%d", SD_Path, try_time, res);
		printLogToUart(LOG_L_FAULT, "FCSdcardDev", logbuf);
		return res;
	}

	try_time = 0;
	if (!mDevOpened) { // open the file
		while(1) {
			if (try_time++ > 5)
				break;
			res = f_open(&mFile, path, mode);
			if (res != FR_OK) {
				osDelay(500);
			} else {
				mDevOpened = 1;
				break;
			}
		}
	}
	if (!mDevOpened) {
		sprintf(logbuf, "can't open file [%s], try_time=%d, last error=%d", path, try_time, res);
		printLogToUart(LOG_L_FAULT, "FCSdcardDev", logbuf);		
		return -1;
	}
	mDevOpened = true;
	osDelay(500);

	return 0;
}

void FCSdcardDev::close()
{
	if (!mDevOpened)
		return;

	f_close(&mFile);
}

int FCSdcardDev::read(char* buf, uint16_t buf_len)
{
	if (!mDevOpened)
		return NO_INIT;

	FRESULT res;
	unsigned int bw;
	res = f_read(&mFile, buf, buf_len, &bw);
	if (res != FR_OK) {
		sprintf(logbuf, "read data failed, error=%d", res);
		printLogToUart(LOG_L_FAULT, "FCSdcardDev", logbuf);
		return -1;
	}

	return bw;
}

int FCSdcardDev::write(char* buf, uint16_t buf_len)
{	
	if (!mDevOpened)
		return NO_INIT;

	FRESULT res;
	unsigned int bw;
	mFile.err = 0;
	res = f_write(&mFile, buf, buf_len, &bw);
	if (res != FR_OK) {
		sprintf(logbuf, "write data failed, error=%d", res);
		printLogToUart(LOG_L_FAULT, "FCSdcardDev", logbuf);
		return -1;
	}
	f_sync(&mFile);

	return bw;
}
