#include "FCMagIst8310Dev.h"
#include "FCLogManager.h"
#include "topic_sensor_mag.h"
#include "math.h"

static const char NAME[20] = "FCMagIst8310Dev";
static const char VERSION[20] = "V0001";
extern "C" void Error_Handler(void);

#define SUPPORT_FLOAT_TYPE  1
#define IST_CROSS_AXIS_CALI
#define SUPPORT_HW_AVERAGE      //hw average 16-time to lower noise

//Defines a register address of the IST8310
#define IST8310_REG_WIA             0x00    //Who I am
#define IST8310_REG_INFO            0x01    //More Info
#define IST8310_REG_DATAX           0x03    //Output Value x
#define IST8310_REG_DATAY           0x05    //Output Value y
#define IST8310_REG_DATAZ           0x07    //Output Value z
#define IST8310_REG_STAT1           0x02    //Status register
#define IST8310_REG_STAT2           0x09    //Status register
#define IST8310_REG_CNTRL1          0x0A    //Control setting register 1
#define IST8310_REG_CNTRL2          0x0B    //Control setting register 2
#define IST8310_REG_CNTRL3          0x0D    //Control setting register 3
#define IST8310_REG_OFFSET_START    0xDC    //Offset
#define IST8310_REG_SELECTION_REG   0x42    //Sensor Selection register
#define IST8310_REG_TEST_REG        0x40    //Temperature Compensation Enable register
#ifdef SUPPORT_HW_AVERAGE
#define IST8310_REG_HW_AVE_REG      0x41    //Chip Test register
#endif
#define IST8310_REG_LOW_PW_REG       0x42    //Chip Test register:0xc0

#define IST8310_REG_TUNING_REG      0x47    //Bandgap Tuning register
#define IST8310_SLAVE_ADDR          0x18    //IST8310 Slave Address

#define IST8310_ODR_MODE            0x01    //Force mode
#define IST8310_DATA_NUM            6       //XYZ High&Low

#ifdef IST_CROSS_AXIS_CALI
/*---IST8310 cross-axis matrix Address-----------------danny-----*/
#define IST8310_REG_XX_CROSS_L       0x9C   //cross axis xx low byte
#define IST8310_REG_XX_CROSS_H       0x9D   //cross axis xx high byte
#define IST8310_REG_XY_CROSS_L       0x9E   //cross axis xy low byte
#define IST8310_REG_XY_CROSS_H       0x9F   //cross axis xy high byte
#define IST8310_REG_XZ_CROSS_L       0xA0   //cross axis xz low byte
#define IST8310_REG_XZ_CROSS_H       0xA1   //cross axis xz high byte

#define IST8310_REG_YX_CROSS_L       0xA2   //cross axis yx low byte
#define IST8310_REG_YX_CROSS_H       0xA3   //cross axis yx high byte
#define IST8310_REG_YY_CROSS_L       0xA4   //cross axis yy low byte
#define IST8310_REG_YY_CROSS_H       0xA5   //cross axis yy high byte
#define IST8310_REG_YZ_CROSS_L       0xA6   //cross axis yz low byte
#define IST8310_REG_YZ_CROSS_H       0xA7   //cross axis yz high byte

#define IST8310_REG_ZX_CROSS_L       0xA8   //cross axis zx low byte
#define IST8310_REG_ZX_CROSS_H       0xA9   //cross axis zx high byte
#define IST8310_REG_ZY_CROSS_L       0xAA   //cross axis zy low byte
#define IST8310_REG_ZY_CROSS_H       0xAB   //cross axis zy high byte
#define IST8310_REG_ZZ_CROSS_L       0xAC   //cross axis zz low byte
#define IST8310_REG_ZZ_CROSS_H       0xAD   //cross axis zz high byte

#define IST8310_AXIS_X            0
#define IST8310_AXIS_Y            1
#define IST8310_AXIS_Z            2
#define IST8310_AXES_NUM          3
#endif

#ifdef IST_CROSS_AXIS_CALI
#ifdef SUPPORT_FLOAT_TYPE
float crossaxis_inv[3][3];
#define OTPsensitivity (330)
#else
int64_t crossaxis_inv[9];
int32_t crossaxis_det[1];
//crossaxis cali_sensitivity and bit shift setting
#define OTPsensitivity (330)
#define crossaxisinv_bitshift (16)
#endif
#endif

static int Ist8310_i2c_write_reg(uint8_t reg, uint8_t reg_val, uint8_t block);
static int Ist8310_i2c_read_reg_block(uint8_t reg, uint8_t* rxbuf, uint8_t rxbuf_len);

/* I2C2 init values */
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_tx;
DMA_HandleTypeDef hdma_i2c2_rx;

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00A0A3F7;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

static uint8_t rx_data_updated = 0;
static uint8_t tx_data_buf[4] = {0};
static uint8_t rx_data_buf[10] = {0};
static uint8_t wait_for_read_len = 0;
static uint8_t cur_operate_reg = 0;

extern "C" void MX_DMA_I2C2_TX_IRQ_Callback(void)
{
	hi2c2.State = HAL_I2C_STATE_READY;
	if (cur_operate_reg == IST8310_REG_CNTRL1) {
		cur_operate_reg = IST8310_REG_DATAX;
		return;
	} else if (cur_operate_reg == IST8310_REG_DATAX) {
		wait_for_read_len = 6;
	}
}

extern "C" void MX_DMA_I2C2_RX_IRQ_Callback(void)
{
	hi2c2.State = HAL_I2C_STATE_READY;
	rx_data_updated = 1;
}

static int Ist8310_i2c_write_reg(uint8_t reg, uint8_t reg_val, uint8_t block)
{
	tx_data_buf[0] = reg;
	tx_data_buf[1] = reg_val;

	if (block == 1) {// block mode
		int try_time = 0;
		uint8_t txbuf[2] = {0};
		txbuf[0] = reg;
		txbuf[1] = reg_val;

		while(HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, (uint8_t*)txbuf, 2, 500) != HAL_OK) {
			if (++try_time > 5)
				break;
		}
		if (try_time > 5)
			return -1;		
	} else { // non-block mode
		if (HAL_DMA_GetState(&hdma_i2c2_tx) == HAL_DMA_STATE_READY) {
			HAL_I2C_Master_Transmit_DMA(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, (uint8_t*)tx_data_buf, 2);
		}
	}

	return 0;
}

static int Ist8310_i2c_read_reg_block(uint8_t reg, uint8_t* rxbuf, uint8_t rxbuf_len)
{
	int try_time = 0;
	uint8_t txbuf[2] = {0};
	txbuf[0] = reg;

	while(HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, (uint8_t*)txbuf, 1, 200) != HAL_OK) {
		if (++try_time > 5)
			break;
	}
	if (try_time > 5)
		return -1;
	try_time = 0;
	while(HAL_I2C_Master_Receive(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, rxbuf, rxbuf_len, 200) != HAL_OK) {
		if (++try_time > 5)
			break;
	}
	if (try_time > 5)
		return -1;

	return 0;
}

char I_InvertMatrix3by3(float *invIn, float *invOut)
{
    float largest;                  // largest element used for pivoting
    float scaling;                  // scaling factor in pivoting
    float recippiv;                 // reciprocal of pivot element
    float ftmp;                     // temporary variable used in swaps
    int i, j, k, l, m;              // index counters
    int iPivotRow, iPivotCol;       // row and column of pivot element
    int iPivot[3]={0};
    int isize=3;
    int iRowInd[3] = {0};
    int iColInd[3] = {0};
    float A[3][3]={0};
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
        {
            A[i][j]= invIn[i+j*3];
        }
    //printf("%f %f %f %f",A[0][0],A[1][1],A[2][2],A[3][3]);
    // to avoid compiler warnings
    iPivotRow = iPivotCol = 0;

    // main loop i over the dimensions of the square matrix A
    for (i = 0; i < isize; i++)
    {
        // zero the largest element found for pivoting
        largest = 0.0F;
        // loop over candidate rows j
        for (j = 0; j < isize; j++)
        {
            // check if row j has been previously pivoted
            if (iPivot[j] != 1)
            {
                // loop over candidate columns k
                for (k = 0; k < isize; k++)
                {
                    // check if column k has previously been pivoted
                    if (iPivot[k] == 0)
                    {
                        // check if the pivot element is the largest found so far
                        if (fabs(A[j][k]) >= largest)
                        {
                            // and store this location as the current best candidate for pivoting
                            iPivotRow = j;
                           iPivotCol = k;
                            largest = (float) fabs(A[iPivotRow][iPivotCol]);
                        }
                    }
                    else if (iPivot[k] > 1)
                    {
                        // zero determinant situation: exit with identity matrix
                        //fmatrixAeqI(A, isize);
                     return -1;
                    }
                }
            }
        }
        // increment the entry in iPivot to denote it has been selected for pivoting
        iPivot[iPivotCol]++;

        // check the pivot rows iPivotRow and iPivotCol are not the same before swapping
        if (iPivotRow != iPivotCol)
        {
            // loop over columns l
            for (l = 0; l < isize; l++)
            {
                // and swap all elements of rows iPivotRow and iPivotCol
                ftmp = A[iPivotRow][l];
                A[iPivotRow][l] = A[iPivotCol][l];
                A[iPivotCol][l] = ftmp;
            }
        }

        // record that on the i-th iteration rows iPivotRow and iPivotCol were swapped
        iRowInd[i] = iPivotRow;
        iColInd[i] = iPivotCol;

        // check for zero on-diagonal element (singular matrix) and return with identity matrix if detected
        if (A[iPivotCol][iPivotCol] == 0.0F)
        {
            // zero determinant situation: exit with identity matrix
            //fmatrixAeqI(A, isize);
             return -1;
        }

        // calculate the reciprocal of the pivot element knowing it's non-zero
        recippiv = 1.0F / A[iPivotCol][iPivotCol];
        // by definition, the diagonal element normalizes to 1
        A[iPivotCol][iPivotCol] = 1.0F;
        // multiply all of row iPivotCol by the reciprocal of the pivot element including the diagonal element
        // the diagonal element A[iPivotCol][iPivotCol] now has value equal to the reciprocal of its previous value
        for (l = 0; l < isize; l++)
        {
            A[iPivotCol][l] *= recippiv;
        }
        // loop over all rows m of A
        for (m = 0; m < isize; m++)
        {
            if (m != iPivotCol)
            {
                // scaling factor for this row m is in column iPivotCol
                scaling = A[m][iPivotCol];
                // zero this element
                A[m][iPivotCol] = 0.0F;
                // loop over all columns l of A and perform elimination
                for (l = 0; l < isize; l++)
                {
                    A[m][l] -= A[iPivotCol][l] * scaling;
                }
            }
        }
    } // end of loop i over the matrix dimensions

    // finally, loop in inverse order to apply the missing column swaps
    for (l = isize - 1; l >= 0; l--)
    {
        // set i and j to the two columns to be swapped
        i = iRowInd[l];
        j = iColInd[l];

        // check that the two columns i and j to be swapped are not the same
        if (i != j)
        {
            // loop over all rows k to swap columns i and j of A
            for (k = 0; k < isize; k++)
            {
                ftmp = A[k][i];
                A[k][i] = A[k][j];
                A[k][j] = ftmp;
            }
        }
    }
//      printf("%f %f %f %f",A[0][0],A[1][1],A[2][2],A[3][3]);
        for(i=0;i<3;i++)
        for(j=0;j<3;j++)
        {
             invOut[i+j*3]=A[i][j];
        }
   return 0;

}

#ifdef IST_CROSS_AXIS_CALI
void IST8310_CrossaxisTransformation(int16_t *xyz){
    int i = 0;
#ifdef SUPPORT_FLOAT_TYPE
    int16_t outputtmp[3];
    outputtmp[0] = (*xyz) * crossaxis_inv[0][0] +
                   (*(xyz+1)) * crossaxis_inv[1][0] +
                   (*(xyz+2)) * crossaxis_inv[2][0];

    outputtmp[1] = (*xyz) * crossaxis_inv[0][1] +
                   (*(xyz+1)) * crossaxis_inv[1][1] +
                   (*(xyz+2)) * crossaxis_inv[2][1];

    outputtmp[2] = (*xyz) * crossaxis_inv[0][2] +
                   (*(xyz+1)) * crossaxis_inv[1][2] +
                   (*(xyz+2)) * crossaxis_inv[2][2];

    for (i=0; i<IST8310_AXES_NUM; i++)
        *(xyz+i) = (short)outputtmp[i];
#else
    int64_t outputtmp[3];
    for( i = 0; i < 9; i++){
        if(crossaxis_inv[i]!=0)
          break;
        if(i == 8)
          IST8310_Init();
    }
    outputtmp[0] = xyz[0] * crossaxis_inv[0] +
                   xyz[1] * crossaxis_inv[1] +
                   xyz[2] * crossaxis_inv[2];

    outputtmp[1] = xyz[0] * crossaxis_inv[3] +
                   xyz[1] * crossaxis_inv[4] +
                   xyz[2] * crossaxis_inv[5];

    outputtmp[2] = xyz[0] * crossaxis_inv[6] +
                   xyz[1] * crossaxis_inv[7] +
                   xyz[2] * crossaxis_inv[8];

    for (i=0; i<IST8310_AXES_NUM; i++)
    {
        outputtmp[i] = outputtmp[i] / (*crossaxis_det);
    }
    xyz[0]= (short)(outputtmp[0] >> crossaxisinv_bitshift);
    xyz[1]= (short)(outputtmp[1] >> crossaxisinv_bitshift);
    xyz[2]= (short)(outputtmp[2] >> crossaxisinv_bitshift);
#endif
}
#endif

#ifdef IST_CROSS_AXIS_CALI
#ifdef SUPPORT_FLOAT_TYPE
void IST8310_Crossaxis_Matrix(int enable)
{
    int i = 0, j;
    uint8_t crossxbuf[6];
    uint8_t crossybuf[6];
    uint8_t crosszbuf[6];
    float OTPcrossaxis[3][3];

    if (enable == 0)
    {
    memset(crossaxis_inv, 0 ,9);
    crossaxis_inv[0][0] = crossaxis_inv[1][1] = crossaxis_inv[2][2] = 1;
        return;
    }
    else
    {
        Ist8310_i2c_read_reg_block(IST8310_REG_XX_CROSS_L, crossxbuf, 6);
        Ist8310_i2c_read_reg_block(IST8310_REG_YX_CROSS_L, crossybuf, 6);
        Ist8310_i2c_read_reg_block(IST8310_REG_ZX_CROSS_L, crosszbuf, 6);

        OTPcrossaxis[0][0] = (float)(short)(((crossxbuf[1] << 8) | (crossxbuf[0]))); //328
        OTPcrossaxis[0][1] = (float)(short)(((crossxbuf[3] << 8) | (crossxbuf[2]))); //9
        OTPcrossaxis[0][2] = (float)(short)(((crossxbuf[5] << 8) | (crossxbuf[4]))); //19
        OTPcrossaxis[1][0] = (float)(short)(((crossybuf[1] << 8) | (crossybuf[0]))); //-7
        OTPcrossaxis[1][1] = (float)(short)(((crossybuf[3] << 8) | (crossybuf[2]))); //326
        OTPcrossaxis[1][2] = (float)(short)(((crossybuf[5] << 8) | (crossybuf[4]))); //2
        OTPcrossaxis[2][0] = (float)(short)(((crosszbuf[1] << 8) | (crosszbuf[0])));    //36
        OTPcrossaxis[2][1] = (float)(short)(((crosszbuf[3] << 8) | (crosszbuf[2])));    //27
        OTPcrossaxis[2][2] = (float)(short)(((crosszbuf[5] << 8) | (crosszbuf[4])));   //335
        I_InvertMatrix3by3(&OTPcrossaxis[0][0], &crossaxis_inv[0][0]);
        for(i=0;i<3;i++)
            for(j=0;j<3;j++)
                crossaxis_inv[i][j] = crossaxis_inv[i][j]*OTPsensitivity;
    }
    return;
}
#else
void IST8310_Crossaxis_Matrix(int bitshift, int enable)
{
//    int ret;
    int i = 0;
    uint8_t crossxbuf[6];
    uint8_t crossybuf[6];
    uint8_t crosszbuf[6];
    short OTPcrossaxis[9] = {0};
    int64_t inv[9] = {0};

    if (enable == 0)
    {
DET_eql_0:
        *crossaxis_inv = (1<<bitshift);
        *(crossaxis_inv+1) = 0;
        *(crossaxis_inv+2) = 0;
        *(crossaxis_inv+3) = 0;
        *(crossaxis_inv+4) = (1<<bitshift);
        *(crossaxis_inv+5) = 0;
        *(crossaxis_inv+6) = 0;
        *(crossaxis_inv+7) = 0;
        *(crossaxis_inv+8) = (1<<bitshift);
        *crossaxis_det = 1;

//        for (i=0; i<9; i++)
//        {
//            printf("*(crossaxis_inv + %d) = %lld\n", i, *(crossaxis_inv+i));
//        }
//        printf("det = %d\n",*crossaxis_det);
        return;
    }
    else
    {
        i2c_get_bulk_reg(_i2c_fd, IST8310_REG_XX_CROSS_L, crossxbuf, 6);
        i2c_get_bulk_reg(_i2c_fd, IST8310_REG_YX_CROSS_L, crossybuf, 6);
        i2c_get_bulk_reg(_i2c_fd, IST8310_REG_ZX_CROSS_L, crosszbuf, 6);

        OTPcrossaxis[0] = ((int) crossxbuf[1]) << 8 | ((int) crossxbuf[0]);
        OTPcrossaxis[3] = ((int) crossxbuf[3]) << 8 | ((int) crossxbuf[2]);
        OTPcrossaxis[6] = ((int) crossxbuf[5]) << 8 | ((int) crossxbuf[4]);
        OTPcrossaxis[1] = ((int) crossybuf[1]) << 8 | ((int) crossybuf[0]);
        OTPcrossaxis[4] = ((int) crossybuf[3]) << 8 | ((int) crossybuf[2]);
        OTPcrossaxis[7] = ((int) crossybuf[5]) << 8 | ((int) crossybuf[4]);
        OTPcrossaxis[2] = ((int) crosszbuf[1]) << 8 | ((int) crosszbuf[0]);
        OTPcrossaxis[5] = ((int) crosszbuf[3]) << 8 | ((int) crosszbuf[2]);
        OTPcrossaxis[8] = ((int) crosszbuf[5]) << 8 | ((int) crosszbuf[4]);
        *crossaxis_det = ((int32_t)OTPcrossaxis[0])*OTPcrossaxis[4]*OTPcrossaxis[8] +
               ((int32_t)OTPcrossaxis[1])*OTPcrossaxis[5]*OTPcrossaxis[6] +
               ((int32_t)OTPcrossaxis[2])*OTPcrossaxis[3]*OTPcrossaxis[7] -
               ((int32_t)OTPcrossaxis[0])*OTPcrossaxis[5]*OTPcrossaxis[7] -
               ((int32_t)OTPcrossaxis[2])*OTPcrossaxis[4]*OTPcrossaxis[6] -
               ((int32_t)OTPcrossaxis[1])*OTPcrossaxis[3]*OTPcrossaxis[8];

        if (*crossaxis_det == 0) {
            goto DET_eql_0;
        }

        inv[0] = (int64_t)OTPcrossaxis[4] * OTPcrossaxis[8] - (int64_t)OTPcrossaxis[5] * OTPcrossaxis[7];
        inv[1] = (int64_t)OTPcrossaxis[2] * OTPcrossaxis[7] - (int64_t)OTPcrossaxis[1] * OTPcrossaxis[8];
        inv[2] = (int64_t)OTPcrossaxis[1] * OTPcrossaxis[5] - (int64_t)OTPcrossaxis[2] * OTPcrossaxis[4];
        inv[3] = (int64_t)OTPcrossaxis[5] * OTPcrossaxis[6] - (int64_t)OTPcrossaxis[3] * OTPcrossaxis[8];
        inv[4] = (int64_t)OTPcrossaxis[0] * OTPcrossaxis[8] - (int64_t)OTPcrossaxis[2] * OTPcrossaxis[6];
        inv[5] = (int64_t)OTPcrossaxis[2] * OTPcrossaxis[3] - (int64_t)OTPcrossaxis[0] * OTPcrossaxis[5];
        inv[6] = (int64_t)OTPcrossaxis[3] * OTPcrossaxis[7] - (int64_t)OTPcrossaxis[4] * OTPcrossaxis[6];
        inv[7] = (int64_t)OTPcrossaxis[1] * OTPcrossaxis[6] - (int64_t)OTPcrossaxis[0] * OTPcrossaxis[7];
        inv[8] = (int64_t)OTPcrossaxis[0] * OTPcrossaxis[4] - (int64_t)OTPcrossaxis[1] * OTPcrossaxis[3];

        for (i=0; i<9; i++) {
            crossaxis_inv[i] = (inv[i] << bitshift) * OTPsensitivity;
        }
    }

    return;
}
#endif
#endif

void IST8310_Init()
{
		// Switch to stand-by mode (0x00: stand-by mode, 0x01: Singel measure mode)
    Ist8310_i2c_write_reg(IST8310_REG_CNTRL1, 0x00, 1);
#ifdef SUPPORT_HW_AVERAGE
    Ist8310_i2c_write_reg(IST8310_REG_HW_AVE_REG,0x24, 1);
#endif
    Ist8310_i2c_write_reg(IST8310_REG_LOW_PW_REG,0xC0, 1);
#ifdef IST_CROSS_AXIS_CALI
    int crossaxis_enable = 0;
    char cross_mask[1];
    uint8_t wbuffer[2];

    cross_mask[0]= 0xFF;
    Ist8310_i2c_read_reg_block(IST8310_REG_XX_CROSS_L, wbuffer, 2);
    if((wbuffer[0] == cross_mask[0]) && (wbuffer[1] == cross_mask[0]))
        crossaxis_enable = 0;
    else
        crossaxis_enable = 1;
#ifdef SUPPORT_FLOAT_TYPE
    IST8310_Crossaxis_Matrix(crossaxis_enable);
#else
    IST8310_Crossaxis_Matrix(crossaxisinv_bitshift, crossaxis_enable);
#endif
#endif
}

FCMagIst8310Dev::FCMagIst8310Dev()
	: FCDev(NAME, VERSION, MAG)
{

}

FCMagIst8310Dev::~FCMagIst8310Dev()
{

}

int FCMagIst8310Dev::open()
{
	if (mDevOpened)
		return 0;

	MX_I2C2_Init();

	// read chipd id
	uint8_t chipid=0;
	int ret = Ist8310_i2c_read_reg_block(IST8310_REG_WIA, &chipid, 1);
	if (ret < 0)
		return OPEN_FAIL;

	// check chip id
	if (chipid != 0x07)
		return OPEN_FAIL;

	// init ist8310
	IST8310_Init();

	mDevOpened = true;
	return 0;
}

void FCMagIst8310Dev::close()
{

}

int FCMagIst8310Dev::read(char* buf, uint16_t buf_len)
{
	if (!mDevOpened)
		return NO_INIT;

	if (rx_data_updated == 0) {
		if(cur_operate_reg == 0) {// trigger a new mag capture
			cur_operate_reg = IST8310_REG_CNTRL1;
			Ist8310_i2c_write_reg(cur_operate_reg, IST8310_ODR_MODE, 0);
		} else if (cur_operate_reg == IST8310_REG_DATAX) {
			if (wait_for_read_len <= 0) {
				tx_data_buf[0] = cur_operate_reg;
				HAL_I2C_Master_Transmit_DMA(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, (uint8_t*)tx_data_buf, 1);
			} else {
				if (HAL_DMA_GetState(&hdma_i2c2_rx) == HAL_DMA_STATE_READY) {
					HAL_I2C_Master_Receive_DMA(&hi2c2, (uint16_t)IST8310_SLAVE_ADDR, rx_data_buf, wait_for_read_len);
					wait_for_read_len = 0;
				}
			}
		}
	}

	if (rx_data_updated) {
		int16_t xyz[3]={0};
		xyz[0] = (int16_t)(((uint16_t)rx_data_buf[1]<<8) | (uint16_t)rx_data_buf[0]);
		xyz[1] = (int16_t)(((uint16_t)rx_data_buf[3]<<8) | (uint16_t)rx_data_buf[2]);
		xyz[2] = (int16_t)(((uint16_t)rx_data_buf[5]<<8) | (uint16_t)rx_data_buf[4]);

#ifdef IST_CROSS_AXIS_CALI
	IST8310_CrossaxisTransformation(xyz);
#endif
		topic_sensor_mag* p_mag = (topic_sensor_mag*)buf;
		p_mag->timestamp = getTimeStamp();
		p_mag->x_raw = xyz[0];
		p_mag->y_raw = xyz[1];
		p_mag->z_raw = xyz[2];

		rx_data_updated = 0;
		cur_operate_reg = 0;

		return buf_len;
	}

	return 0;
}

int FCMagIst8310Dev::write(char* buf, uint16_t buf_len)
{
	if (!mDevOpened) {
		return NO_INIT;
	}

	return 0;
}
