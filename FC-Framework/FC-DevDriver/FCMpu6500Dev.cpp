#include "FCMpu6500Dev.h"
#include "FCLogManager.h"
#include "topic_sensor_imu.h"

static const char NAME[20] = "FCMpu6500Dev";
static const char VERSION[20] = "V0001";

extern "C" void Error_Handler(void);

enum MPU6500_REG_ADDR {
   MPU6500_REG_SMPLRT_DIV     = 25,
   MPU6500_REG_CONFIG         = 26,
   MPU6500_REG_GYRO_CONFIG    = 27,
   MPU6500_REG_ACCEL_CONFIG   = 28,
   MPU6500_REG_ACCEL_CONFIG2  = 29,
   MPU6500_REG_FIFO_CTL       = 35,
   MPU6500_REG_I2C_MST_CTRL   = 36,
   MPU6500_REG_I2C_SLV0_ADDR  = 37,
   MPU6500_REG_I2C_SLV0_REG   = 38,
   MPU6500_REG_I2C_SLV0_CTRL  = 39,
   MPU6500_REG_I2C_SLV4_ADDR  = 49,
   MPU6500_REG_I2C_SLV4_REG   = 50,
   MPU6500_REG_I2C_SLV4_DO    = 51,
   MPU6500_REG_I2C_SLV4_CTRL  = 52,
   MPU6500_REG_I2C_SLV4_DI    = 53,
   MPU6500_REG_I2C_MST_STATUS = 54,
   MPU6500_REG_INT_EN         = 56,
   MPU6500_REG_INT_STATUS     = 58,
   MPU6500_REG_I2C_MST_DELAY_CTRL = 103,
   MPU6500_REG_USER_CTRL      = 106,
   MPU6500_REG_PWR_MGMT1    = 107,
   MPU6500_REG_WHOAMI         = 117,
};

/* SPI 1 init values */
SPI_HandleTypeDef hspi1;
/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}
static int MPU_SPI_Read(int reg, uint8_t* buf, int size)
{
	uint8_t spiTxBuf[4] = {0};
	uint8_t spiRxBuf[4] = {0};

	spiTxBuf[0] = reg;
	HAL_SPI_TransmitReceive(&hspi1, spiTxBuf, spiRxBuf, 1, 100);

	buf[0] = spiRxBuf[0];

	return 0;
}

/* DMA init values */
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;
/* DMA nit function */
extern "C" void MX_DMA_SPI1_TX_IRQ_Callback(void)
{
	uint8_t data[20]={0};
	uint16_t data_len = 10;

}

extern "C" void MX_DMA_SPI1_RX_IRQ_Callback(void)
{
	uint8_t data[20]={0};
	uint16_t data_len = 10;
}

FCMpu6500Dev::FCMpu6500Dev()
	: FCDev(NAME, VERSION, IMU)
{

}

FCMpu6500Dev::~FCMpu6500Dev()
{

}

int FCMpu6500Dev::open()
{
	if (mDevOpened)
		return 0;

	// spi1 bus init
	MX_SPI1_Init();	
	uint8_t buf[10] = {0};
	MPU_SPI_Read(MPU6500_REG_WHOAMI, buf, 10);

	char uartlog[20] = {0};
	sprintf(uartlog, "value=%x\n", buf[0]);
	printLogToUart(LOG_L_DEBUG, "FCMpu6500Dev", uartlog);

	mDevOpened = true;

	return 0;
}

void FCMpu6500Dev::close()
{
	if (!mDevOpened)
		return;

	mDevOpened = false;
}

static uint64_t ts = 0;

int FCMpu6500Dev::read(char* buf, uint16_t buf_len)
{
	if (!mDevOpened)
		return NO_INIT;

	topic_sensor_imu* p_img = (topic_sensor_imu*)buf;

	p_img->acc[0] = 212;
	p_img->acc[1] = 23;
	p_img->acc[2] = 254;
	p_img->gyro[0] = 311;
	p_img->gyro[1] = 323;
	p_img->gyro[2] = 322;
	p_img->timestamp = getTimeStampUs();
	ts = p_img->timestamp;

	return buf_len;
}

int FCMpu6500Dev::write(char* buf, uint16_t buf_len)
{
	if (!mDevOpened)
		return NO_INIT;

	return 0;
}
