#include "FCBaroMs5611Dev.h"
#include "FCLogManager.h"

static const char NAME[20] = "FCBaroMs5611Dev";
static const char VERSION[20] = "V0001";

extern "C" void Error_Handler(void);

SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_tx;
DMA_HandleTypeDef hdma_spi3_rx;

/* SPI3 init function */
static void MX_SPI3_Init(void)
{
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }

}

static int mpu_spi_read(int reg, uint8_t* buf, int size)
{
	uint8_t spiTxBuf[4] = {0};
	uint8_t spiRxBuf[4] = {0};
	
	spiTxBuf[0] = reg;
	HAL_SPI_TransmitReceive(&hspi3, spiTxBuf, spiRxBuf, 1, 100);

	buf[0] = spiRxBuf[0];

	return 0;
}

FCBaroMs5611Dev::FCBaroMs5611Dev()
	: FCDev(NAME, VERSION, BARO)
{

}

FCBaroMs5611Dev::~FCBaroMs5611Dev()
{

}

int FCBaroMs5611Dev::open()
{
	if (mDevOpened)
		return 0;

	// spi3 bus init
	MX_SPI3_Init();
	
//	mDevOpened = true;
	return 0;
}

void FCBaroMs5611Dev::close()
{

}

int FCBaroMs5611Dev::read(char* buf, uint16_t buf_len)
{
	return 0;
}

int FCBaroMs5611Dev::write(char* buf, uint16_t buf_len)
{
	if (mDevOpened) {

		return SUCCESS;
	}

	return NO_INIT;
}
