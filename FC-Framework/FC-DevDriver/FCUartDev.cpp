#include "FCUartDev.h"

static const char NAME[20] = "FCUartDev";
static const char VERSION[20] = "V0001";

extern "C" void Error_Handler(void);

UART_HandleTypeDef huart8;
DMA_HandleTypeDef hdma_uart8_tx;
DMA_HandleTypeDef hdma_uart8_rx;

/* UART8 init function */
static void MX_UART8_Init(void)
{

  huart8.Instance = UART8;
  huart8.Init.BaudRate = 115200;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart8) != HAL_OK)
  {
    Error_Handler();
  }

}

extern "C" void MX_DMA_UART8_TX_IRQ_Callback(void)
{
	huart8.gState = HAL_UART_STATE_READY;
}

FCUartDev::FCUartDev()
	: FCDev(NAME, VERSION, UART)
{

}

FCUartDev::~FCUartDev()
{

}

int FCUartDev::open()
{
	if (mDevOpened)
		return 0;

	MX_UART8_Init();
	mDevOpened = true;
	return 0;
}

void FCUartDev::close()
{

}

int FCUartDev::read(char* buf, uint16_t buf_len)
{
	return 0;
}

static char dma_buf[128]={0};
int FCUartDev::write(char* buf, uint16_t buf_len)
{
	if (!mDevOpened)
		return NO_INIT;

	if (huart8.gState == HAL_UART_STATE_READY) {
		memcpy(dma_buf, buf, buf_len);
		HAL_UART_Transmit_DMA(&huart8, (uint8_t*)dma_buf, buf_len);
		return buf_len;
	}
	return 0;
}
