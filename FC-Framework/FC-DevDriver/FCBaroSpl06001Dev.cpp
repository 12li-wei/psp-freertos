#include "FCBaroSpl06001Dev.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCBaroSpl06001Dev";
static const char VERSION[20] = "V0001";

extern "C" void Error_Handler(void);

SPI_HandleTypeDef hspi4;
DMA_HandleTypeDef hdma_spi4_tx;
DMA_HandleTypeDef hdma_spi4_rx;

/* SPI4 init function */
static void MX_SPI4_Init(void)
{
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 7;
  hspi4.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
}

static int mpu_spi_read(int reg, uint8_t* buf, int size)
{
	uint8_t spiTxBuf[4] = {0};
	uint8_t spiRxBuf[4] = {0};
	
	spiTxBuf[0] = reg;
	HAL_SPI_TransmitReceive(&hspi4, spiTxBuf, spiRxBuf, 1, 100);

	buf[0] = spiRxBuf[0];

	return 0;
}

FCBaroSpl06001Dev::FCBaroSpl06001Dev()
	: FCDev(NAME, VERSION, BARO)
{

}

FCBaroSpl06001Dev::~FCBaroSpl06001Dev()
{

}

int FCBaroSpl06001Dev::open()
{
	if (mDevOpened)
		return 0;

	// spi3 bus init
	MX_SPI4_Init();
	
//	mDevOpened = true;
	return 0;
}

void FCBaroSpl06001Dev::close()
{

}

int FCBaroSpl06001Dev::read(char* buf, uint16_t buf_len)
{
	return 0;
}

int FCBaroSpl06001Dev::write(char* buf, uint16_t buf_len)
{
	if (mDevOpened) {

		return SUCCESS;
	}

	return NO_INIT;
}
