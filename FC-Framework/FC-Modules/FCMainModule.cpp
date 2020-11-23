#include "FCMainModule.h"
#include "FCTopicManager.h"
#include "FCLogManager.h"
#include "topic_sensor_imu.h"
#include "topic_sensor_mag.h"

static const char NAME[20] = "FCMainModule";
static const char VERSION[20] = "V0001";
static FCMainModule* mInstance = NULL;

FCMainModule::FCMainModule():
	VModule(NAME, VERSION)
{
}

FCMainModule::~FCMainModule()
{
}

FCMainModule* FCMainModule::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCMainModule();
	}
	return mInstance;
}

void FCMainModule::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

uint32_t imu_total_count = 0;
uint32_t mag_total_count = 0;
static topic_sensor_imu imu_data[MAX_IMU_SIZE];
static topic_sensor_mag mag_data[MAX_MAG_SIZE];
static int log_switch = 0;
static char uartlog[100]={0};
void FCMainModule::TaskCustomCallback(void const * argument)
{
	VTopic* topic1 = subscribeTopic("sensor_imu0", mThreadId, 0);
	if (topic1 != NULL) {
		int checkrst = checkTopic(topic1, mThreadId, 100);
		if (checkrst > 0) {
			imu_total_count += checkrst;
			copyTopicData(topic1, mThreadId, (uint8_t*)imu_data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data acc=[%d,%d,%d]",
							"sensor_imu0", checkrst,
							imu_data[0].acc[0], imu_data[1].acc[1], imu_data[2].acc[2]);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sensor_imu0");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sensor_imu0");
	}
	if (log_switch++ < 5) {
//		printLogToUart(LOG_L_DEBUG, "FCMainModule", uartlog);
	}

	VTopic* topic2 = subscribeTopic("sensor_imu1", mThreadId, 0);
	if (topic2 != NULL) {
		int checkrst = checkTopic(topic2, mThreadId, 100);
		if (checkrst > 0) {
			copyTopicData(topic2, mThreadId, (uint8_t*)imu_data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data acc=[%d,%d,%d]",
							"sensor_imu1", checkrst,
							imu_data[0].acc[0], imu_data[1].acc[1], imu_data[2].acc[2]);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sensor_imu1");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sensor_imu1");
	}
//	printLogToUart(LOG_L_DEBUG, "FCMainModule", uartlog);

	VTopic* topic3 = subscribeTopic("sensor_mag", mThreadId, 0);
	if (topic3 != NULL) {
		int checkrst = checkTopic(topic3, mThreadId, 100);
		if (checkrst > 0) {
			mag_total_count += checkrst;
			copyTopicData(topic3, mThreadId, (uint8_t*)mag_data, checkrst);

			sprintf(uartlog, "[%s], checkrst=%d, data mag_data=[%d,%d,%d]",
							"sensor_mag", checkrst,
							mag_data[0].x_raw, mag_data[0].y_raw, mag_data[0].z_raw);
		} else {
			sprintf(uartlog, "[%s], no nodata", "sensor_mag");
		}
	} else {
		sprintf(uartlog, "[%s], no topic", "sensor_mag");
	}

	if (log_switch++ < 10) {
//		printLogToUart(LOG_L_DEBUG, "FCMainModule", uartlog);
	} else {
		log_switch = 0;
	}

//	dumpAllTopics();
//	dumpAllModules();
//	dumpAllDevs();
}
