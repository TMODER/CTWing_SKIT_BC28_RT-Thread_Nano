#include "main.h"
#include "HTS221.h"
#include "rtthread.h"


extern I2C_HandleTypeDef hi2c1;


//HTS221_Data_TypeDef _HTS221_Data={0};

static HTS221_StatusTypeDef I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HTS221_StatusTypeDef ret;
	
	ret = (HTS221_StatusTypeDef)HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, Timeout);
	
	return ret;
}

static HTS221_StatusTypeDef I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HTS221_StatusTypeDef ret;
	
	ret = (HTS221_StatusTypeDef)HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, Timeout);
	
	return ret;
}

/************************ device address ***************************/

//#define WRITE_ADDRESS 0xBE
//#define READ_ADDRESS  0xBF

#define HTS221_ADDRESS 0xBE

/************************ HTS221 regs ***************************/

#define  WHO_AM_I           0x0f
#define  AV_CONF            0x10

//temp
#define  T0_degC_X8_ADDR   	0x32
#define  T1_degC_X8_ADDR   	0x33
#define  T1_T0_MSB_X8_ADDR 	0x35
#define  T0_OUT_ADDR       	0x3C
#define  T1_OUT_ADDR       	0x3E

//hum
#define  H0_RH_X2_ADDR     	0x30
#define  H1_RH_X2_ADDR     	0x31
#define  H0_T0_OUT_ADDR    	0x36
#define  H1_T0_OUT_ADDR    	0x3A

//CTRL REG
#define  CTRL_REG1_ADDR    	0x20
#define  CTRL_REG2_ADDR    	0x21
#define  CTRL_REG3_ADDR     0x22

//STATUS REG
#define  STATUS_REG_ADDR   	0x27  

//HUM
#define  HUMIDITY_OUT_ADDR 	0x28
#define  TEMP_OUT_ADDR     	0x2A


/*******************REG BIT MASK *******************/
//CTR reg1
#define  HTS221_ODR_MASK        0x03

//CTRL reg2
#define  HTS221_CTRL_REG2_ONE_SHOT_MASK   0x01
#define  HTS221_CTRL_REG2_ONE_SHOT_START  0x01



/****************** variables *********************/

float T0_degC = 0;
float T1_degC = 0;

int16_t T0_out = 0;
int16_t T1_out = 0;

float H0_rh = 0;
float H1_rh = 0;

int16_t H0_T0_out = 0;
int16_t H1_T0_out = 0;


/****************** functios **********************/

/**
* @brief  who am i
* @retval HUM_TEMP_OK in case of success, an error code otherwise
*/
HTS221_StatusTypeDef HTS221_who_am_i(void)
{
	HTS221_StatusTypeDef ret;
	uint8_t tempReg=0;
	
	ret = I2C_Read(HTS221_ADDRESS, WHO_AM_I, &tempReg, 1, 10);
	if( ret != HTS221_OK )
	{
		return ret;
	}
	
	if( tempReg != 0xBC )
	{
		return HTS221_ERR;
	}
	tempReg=0x01;
	
	ret=I2C_Write(HTS221_ADDRESS, CTRL_REG2_ADDR, &tempReg, 1, 10);
	
	if( ret != HTS221_OK )
	{
		return ret;
	}
	
	return ret;
}

/**
* @brief  HTS221 Calibration procedure
* @retval HUM_TEMP_OK in case of success, an error code otherwise
*/

HTS221_StatusTypeDef HTS221_Calibration(void)
{
    /* Temperature Calibration */
    /* Temperature in degree for calibration ( "/8" to obtain float) */
    uint16_t T0_degC_x8_L, T0_degC_x8_H, T1_degC_x8_L, T1_degC_x8_H;
    uint8_t H0_rh_x2, H1_rh_x2;
    uint8_t tempReg[2] = {0, 0};
		HTS221_StatusTypeDef ret;
		
		ret = I2C_Read(HTS221_ADDRESS, T0_degC_X8_ADDR, tempReg, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T0_degC_x8_L = (uint16_t)tempReg[0];
		
		ret = I2C_Read(HTS221_ADDRESS, T1_T0_MSB_X8_ADDR, tempReg, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T0_degC_x8_H = (uint16_t) (tempReg[0] & 0x03);
		
		ret = I2C_Read(HTS221_ADDRESS, T1_degC_X8_ADDR, tempReg, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T1_degC_x8_L = (uint16_t)tempReg[0];
		
		ret = I2C_Read(HTS221_ADDRESS, T1_T0_MSB_X8_ADDR, tempReg, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T1_degC_x8_H = (uint16_t) (tempReg[0] & 0x0C);
    T1_degC_x8_H = T1_degC_x8_H >> 2;
		
		T0_degC = ((float)((T0_degC_x8_H << 8) | (T0_degC_x8_L))) / 8;
    T1_degC = ((float)((T1_degC_x8_H << 8) | (T1_degC_x8_L))) / 8;
		
		ret = I2C_Read(HTS221_ADDRESS, T0_OUT_ADDR, &tempReg[0], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		ret = I2C_Read(HTS221_ADDRESS, T0_OUT_ADDR+1, &tempReg[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T0_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
		
		ret = I2C_Read(HTS221_ADDRESS, T1_OUT_ADDR, &tempReg[0], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		ret = I2C_Read(HTS221_ADDRESS, T1_OUT_ADDR+1, &tempReg[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T1_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
		
    /* Humidity Calibration */
    /* Humidity in degree for calibration ( "/2" to obtain float) */
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR, &H0_rh_x2, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		ret = I2C_Read(HTS221_ADDRESS, H1_RH_X2_ADDR, &H1_rh_x2, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}

    ret = I2C_Read(HTS221_ADDRESS, H0_T0_OUT_ADDR, &tempReg[0], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		ret = I2C_Read(HTS221_ADDRESS, H0_T0_OUT_ADDR+1, &tempReg[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    H0_T0_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
		
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR, &tempReg[0], 1, 10);	
		if( ret != HTS221_OK )
		{
			return ret;
		}	
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+1, &tempReg[1], 1, 10);	
		if( ret != HTS221_OK )
		{
			return ret;
		}			
    H1_T0_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
		
    H0_rh = ((float)H0_rh_x2) / 2;
    H1_rh = ((float)H1_rh_x2) / 2;
		
		return HTS221_OK;
}


/*******************************************************************
* @brief  Read HTS221 output register, and calculate the humidity
* @param  pfData the pointer to data output
* @retval HUM_TEMP_OK in case of success, an error code otherwise
********************************************************************/
HTS221_StatusTypeDef HTS221_GetHumidity(uint16_t *pfData)
{
    int16_t H_T_out, humidity_t;
    float H_rh;
	
	  uint8_t tempReg[2] = {0, 0};
    uint8_t tmp = 0x00;
		HTS221_StatusTypeDef ret;
		uint16_t time = 10;
		
		ret = I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}	
		
    /* Output Data Rate selection */
    tmp &= (HTS221_ODR_MASK);
    if(tmp == 0x00)
    {  
					
						
				tmp=0x80;  //Active Sensor
				ret=I2C_Write(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
				if( ret != HTS221_OK )
				{
						return ret;
					}	
			
				ret = I2C_Read(HTS221_ADDRESS, CTRL_REG2_ADDR, &tmp, 1, 10);
			  if( ret != HTS221_OK )
				{
					return ret;
				}	
				
        /* Serial InteRFace Mode selection */
        tmp &= ~(HTS221_CTRL_REG2_ONE_SHOT_MASK);
        tmp |= HTS221_CTRL_REG2_ONE_SHOT_START;
	
			
				
			  ret = I2C_Write(HTS221_ADDRESS, CTRL_REG2_ADDR, &tmp, 1, 10);
				if( ret != HTS221_OK )
				{
					return ret;
				}	
     
				
				ret = I2C_Read(HTS221_ADDRESS, CTRL_REG2_ADDR, &tmp, 1, 10);
				if( ret != HTS221_OK )
				{
					return ret;
				}	
				
			//	ret=HTS221_Calibration();
				
				ret = I2C_Read(HTS221_ADDRESS, AV_CONF, &tmp, 1, 10);
				
				if( ret != HTS221_OK )
				{
					return ret;
				}	
				
        do
        {				
						ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR, &tmp, 1, 10);
            if( ret != HTS221_OK )
						{
							return ret;
						}
						rt_thread_mdelay(10);
						time--;
        }
        while((!(tmp & 0x02)) && time);
    }
		
		if( time == 0 )
		{
			return HTS221_TIMEOUT;
		}
		
		ret = I2C_Read(HTS221_ADDRESS, HUMIDITY_OUT_ADDR, &tempReg[0], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}	
		ret = I2C_Read(HTS221_ADDRESS, HUMIDITY_OUT_ADDR+1, &tempReg[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}			
    H_T_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
		
    H_rh = ( float )(((( H_T_out - H0_T0_out ) * ( H1_rh - H0_rh )) / ( H1_T0_out - H0_T0_out )) + H0_rh );
		
    // Truncate to specific number of decimal digits
    humidity_t = (uint16_t)(H_rh * 10); 
    *pfData = humidity_t/10;
		
    // Prevent data going below 0% and above 100% due to linear interpolation
    if ( *pfData > 100 ) *pfData = 100;

		return HTS221_OK;
}

/*******************************************************************
* @brief  Read HTS221 output register, and calculate the temperature
* @param  pfData the pointer to data output
* @retval HUM_TEMP_OK in case of success, an error code otherwise
********************************************************************/
HTS221_StatusTypeDef HTS221_GetTemperature(float *pfData)
{
    int16_t T_out;
	  float T_degC;
	
    uint8_t tempReg[2] = {0, 0};
    uint8_t tmp = 0x00;
		HTS221_StatusTypeDef ret;
		uint16_t time=10;
		
    ret = I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		
    /* Output Data Rate selection */
    tmp &= (HTS221_ODR_MASK);
    if(tmp == 0x00)
    {
			
				tmp=0x80;  //Active Sensor
				ret=I2C_Write(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
				if( ret != HTS221_OK )
				{
						return ret;
					}				
        ret = I2C_Read(HTS221_ADDRESS, CTRL_REG2_ADDR, &tmp, 1, 10);
			  if( ret != HTS221_OK )
				{
					return ret;
				}
        /* Serial Interface Mode selection */
        tmp &= ~(HTS221_CTRL_REG2_ONE_SHOT_MASK);
        tmp |= HTS221_CTRL_REG2_ONE_SHOT_START;
			
        ret = I2C_Write(HTS221_ADDRESS, CTRL_REG2_ADDR, &tmp, 1, 10);
				if( ret != HTS221_OK )
				{
					return ret;
				}
				
        do
        {
						ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR, &tmp, 1, 10);
						if( ret != HTS221_OK )
						{
							return ret;
						}
						time--;
        }
        while(!(tmp & 0x01) && time );
    }
		
		if( time == 0 )
		{
			return HTS221_TIMEOUT;
		}
		
		ret = I2C_Read(HTS221_ADDRESS, TEMP_OUT_ADDR, &tempReg[0], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
 		ret = I2C_Read(HTS221_ADDRESS, TEMP_OUT_ADDR+1, &tempReg[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
    T_out = ((((int16_t)tempReg[1]) << 8) + (int16_t)tempReg[0]);
    
		T_degC = ((float)(T_out - T0_out)) / (T1_out - T0_out) * (T1_degC - T0_degC) + T0_degC;
		
		#if 0
    temperature_t = (int16_t)(T_degC * 10);
    *pfData = temperature_t/10;
		#endif
		
		*pfData = T_degC;
		
		return HTS221_OK;
}


//HTS221_StatusTypeDef HTS221_Get_Data(HTS221_Data_TypeDef * HTS221_Data)
//{
//	HTS221_StatusTypeDef ret=0;
//	uint8_t tmp=0;
//	
//	ret=I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
//	
//	if( ret != HTS221_OK )
//	{
//			return ret;
//	}
//	if(tmp==0)
//	{
//		  tmp=0x80;  //Active Sensor
//		  ret=I2C_Write(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
//			if( ret != HTS221_OK )
//			{
//					return ret;
//			}	
//			ret=HTS221_GetHumidity(&(HTS221_Data->Humidity));
//			if( ret != HTS221_OK )
//			{
//					return ret;
//			}
//			
//			ret=HTS221_GetTemperature((HTS221_Data->(&)Temperature));
//			
//			if( ret != HTS221_OK )
//			{
//					return ret;
//			}
//	}	
//	tmp=0;
//	ret=I2C_Write(HTS221_ADDRESS, CTRL_REG1_ADDR, &tmp, 1, 10);
//	if( ret != HTS221_OK )
//			{
//					return ret;
//			}	
//			return ret;
//}


/*******************************************************************
* @brief  read all register
* @param  void
* @retval void
********************************************************************/
HTS221_StatusTypeDef print_all_reg(void)
{
		uint8_t data[8]={0};
		HTS221_StatusTypeDef ret;
	
		ret = I2C_Read(HTS221_ADDRESS, WHO_AM_I, data, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
	//	os_log("who am i(0x0f):%02x,%d\r\n",data[0], ret);
		
		ret = I2C_Read(HTS221_ADDRESS, AV_CONF, data, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		//os_log("AV CONF(0x10):%02x,%d\r\n",data[0], ret);
		
		ret = I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR,   &data[0], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR+1, &data[1], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, CTRL_REG1_ADDR+2, &data[2], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
//		os_log("CTRL REG1 (0x20):%02x\r\n",data[0]);
//		os_log("CTRL REG2 (0x21):%02x\r\n",data[1]);
//		os_log("CTRL REG3 (0x22):%02x\r\n",data[2]);
		
		data[0] = 0x80;
		ret = I2C_Write(HTS221_ADDRESS, CTRL_REG1_ADDR, data, 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
		
		ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR,   &data[0], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR+1, &data[1], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR+2, &data[2], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR+3, &data[3], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, STATUS_REG_ADDR+4, &data[4], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
//		os_log("STATUS REG(0x27):%02x\r\n", data[0]);
//		os_log("HUM_OUT_L (0x28):%02x\r\n", data[1]);
//		os_log("HUM_OUT_H (0x29):%02x\r\n", data[2]);
//		os_log("TEMP_OUT_L(0x2A):%02x\r\n", data[3]);
//		os_log("TEMP_OUT_H(0x2B):%02x\r\n", data[4]);
		
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR,   &data[1], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR+1, &data[2], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR+2, &data[3], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR+3, &data[4], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H0_RH_X2_ADDR+4, &data[5], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
//		os_log("H0_rH_x2  (0x30):%02x\r\n",data[0]);
//		os_log("H1_rH_x2  (0x31):%02x\r\n",data[1]);
//		os_log("T0_degC_x8(0x32):%02x\r\n",data[2]);
//		os_log("T1_degC_x8(0x33):%02x\r\n",data[3]);
//		os_log("T1/T0_msb (0x34):%02x\r\n",data[4]);
		
		ret = I2C_Read(HTS221_ADDRESS, H0_T0_OUT_ADDR,   &data[0], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H0_T0_OUT_ADDR+1, &data[1], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
//		os_log("H0_T0_OUT_L(0x36):%02x\r\n",data[0]);
//		os_log("H0_T0_OUT_H(0x37):%02x\r\n",data[1]);
		
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR,   &data[0], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+1, &data[1], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+2, &data[2], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+3, &data[3], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+4, &data[4], 1, 10);
		ret = I2C_Read(HTS221_ADDRESS, H1_T0_OUT_ADDR+5, &data[5], 1, 10);
		if( ret != HTS221_OK )
		{
			return ret;
		}
//		os_log("H1_T0_OUT_L(0x3A):%02x\r\n",data[0]);
//		os_log("H1_T0_OUT_H(0x3B):%02x\r\n",data[1]);
//		os_log("T0_OUT_L   (0x3C):%02x\r\n",data[2]);
//		os_log("T0_OUT_H   (0x3D):%02x\r\n",data[3]);
//		os_log("T1_OUT_L   (0x3E):%02x\r\n",data[4]);
//		os_log("T1_OUT_H   (0x3F):%02x\r\n",data[5]);
		
		return HTS221_OK;
}
