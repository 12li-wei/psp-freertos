#include "FCSysStatusManager.h"
#include "FCLogManager.h"
#include "topic_sys_status.h"

static const char NAME[20] = "FCSysStatusManager";
static const char VERSION[20] = "V0001";
static FCSysStatusManager* mInstance = NULL;

extern "C" void Error_Handler(void);

FCSysStatusManager::FCSysStatusManager():
	VModule(NAME, VERSION)
{
	mSysStatusTopic = NULL;
	mUsbPlugin = 0;
}

FCSysStatusManager::~FCSysStatusManager()
{

}

FCSysStatusManager* FCSysStatusManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSysStatusManager();
	}
	return mInstance;
}

void FCSysStatusManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

int FCSysStatusManager::start(uint16_t durationMs, osPriority priority)
{
	mSysStatusTopic = adviseTopic("sys_status", sizeof(topic_sys_status), MAX_SYS_STATUS_SIZE, 0);

	return VModule::start(durationMs, priority);
}

void FCSysStatusManager::stop()
{
	VModule::stop();
}

void FCSysStatusManager::TaskCustomCallback(void const * argument)
{
	if (mSysStatusTopic == NULL)
		return;
	
	uint8_t needbroad = 0;	
	
	GPIO_PinState bitstatus = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
	if (bitstatus==GPIO_PIN_SET && mUsbPlugin==0) { // usb plug in
		mUsbPlugin = 1;
		needbroad = 1;
	} else if (bitstatus==GPIO_PIN_RESET && mUsbPlugin == 1){ // usb plug out
		mUsbPlugin = 0;
		needbroad = 1;
	}

	if (needbroad) {
		topic_sys_status status = {0};
		status.usb_plugin = mUsbPlugin;
		publishTopic(mSysStatusTopic, (uint8_t*)&status, sizeof(topic_sys_status));
	}
	
	// output led status
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_10);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}
