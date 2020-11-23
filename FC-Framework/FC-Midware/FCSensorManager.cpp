#include "FCSensorManager.h"
#include "FCLogManager.h"
#include "topic_sensor_imu.h"
#include "topic_sensor_mag.h"

static const char NAME[20] = "FCSensorManager";
static const char VERSION[20] = "V0001";
static FCSensorManager* mInstance = NULL;

extern "C" void Error_Handler(void);

extern FCDev* gSensorList[];
extern const int gSensorCount;

static topic_sensor_imu imu_data = {0};
static topic_sensor_mag mag_data = {0};

static VTopic* p_imu_topic[2] = {0};
static FCDev* p_img_dev[2] = {0};

// TIM5 init values
#define TIM5_COUNTER  115-1
TIM_HandleTypeDef htim5;
/* TIM5 init function */
static void MX_TIM5_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = TIM5_COUNTER;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = TIM5_COUNTER;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

extern "C" void MX_TIM5_IRQ_Callback(void)
{
	// read all imu data and publish them
	for (int i=0; i<2; i++) {
		if (p_img_dev[i] != NULL) {
			if (p_img_dev[i]->read((char*)&imu_data, sizeof(topic_sensor_imu)) > 0) {
				if (p_imu_topic[i] != NULL) {
					publishTopic(p_imu_topic[i], (uint8_t*)&imu_data, sizeof(topic_sensor_imu));
				}
			}
		}
	}
}

FCSensorManager::FCSensorManager():
	VModule(NAME, VERSION)
{
	mImuTopic[0] = NULL;
	mImuTopic[1] = NULL;
	mMagTopic = NULL;
	mBaroTopic = NULL;
}

FCSensorManager::~FCSensorManager()
{
	for(int i=0; i<gSensorCount; i++) {
		(gSensorList[i])->close();
	}
}

FCSensorManager* FCSensorManager::getInstance()
{
	if (mInstance == NULL) {
		mInstance = new FCSensorManager();
	}
	return mInstance;
}

void FCSensorManager::destroyInstance()
{
	if (mInstance != NULL) {
		delete mInstance;
		mInstance = NULL;
	}
}

int FCSensorManager::start(uint16_t durationMs, osPriority priority)
{
	int imucnt=0;
	for(int i=0; i<gSensorCount; i++) {
		switch(gSensorList[i]->getDevType()) {
			case IMU:
				gSensorList[i]->open();
				if (imucnt++ == 0) {
					mImuTopic[0] = adviseTopic("sensor_imu0", sizeof(topic_sensor_imu), MAX_IMU_SIZE, 1);
					p_img_dev[0] = gSensorList[i];
					p_imu_topic[0] = mImuTopic[0];
				} else {
					mImuTopic[1] = adviseTopic("sensor_imu1", sizeof(topic_sensor_imu), MAX_IMU_SIZE, 1);
					p_img_dev[1] = gSensorList[i];
					p_imu_topic[1] = mImuTopic[1];
				}
				break;
			case MAG:
				gSensorList[i]->open();
				mMagTopic = adviseTopic("sensor_mag", sizeof(topic_sensor_mag), MAX_MAG_SIZE);
				break;
			case BARO:
				break;
			case GPS:
				break;
			case SONAR:
				break;
			default:
				break;
		}
	}

	MX_TIM5_Init();
	HAL_TIM_Base_Start_IT(&htim5);

	return VModule::start(durationMs, priority);
}

void FCSensorManager::stop()
{
	HAL_TIM_Base_Stop_IT(&htim5);
	VModule::stop();
}

static uint64_t lasttime = 0;
extern uint32_t imu_total_count;
extern uint32_t mag_total_count;
static char logbuf[80] = {0};
void FCSensorManager::TaskCustomCallback(void const * argument)
{
#if 1
	if ((this->getTimeStamp() - lasttime) > 2000) {
		sprintf(logbuf, "imu freq=[%.3f]hz, mag freq=[%.3f]hz",
						(float)imu_total_count/((this->getTimeStamp() - lasttime)/1000),
						(float)mag_total_count/((this->getTimeStamp() - lasttime)/1000));
		imu_total_count = 0;
		mag_total_count = 0;
		lasttime = this->getTimeStamp();
		printLogToUart(LOG_L_FAULT, "FCSensorManager", logbuf);
	}
#endif

	// read all sensor data and publish them
	for(int i=0; i<gSensorCount; i++) {
		switch(gSensorList[i]->getDevType()) {
			case IMU: // imu data is read in timer callback
				break;
			case MAG: // read mag data and publish it
				if (mMagTopic != NULL) {
					if (gSensorList[i]->read((char*)&mag_data, sizeof(topic_sensor_mag)) > 0) {
						publishTopic(mMagTopic, (uint8_t*)&mag_data, sizeof(topic_sensor_mag));
					}
				}
				break;
			case BARO: // read baro data and publish it
				break;
			case GPS: // read gps data and publish it
				break;
			case SONAR:  // read gps data and publish it
				break;
			default:
				break;
		}
	}

}
