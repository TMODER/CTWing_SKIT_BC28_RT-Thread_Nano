
#ifndef __HTS221__H_
#define __HTS221__H_

#include <stdint.h>

typedef enum
{
  HTS221_OK       = 0x00U,
  HTS221_ERR      = 0x01U,
  HTS221_BUSY     = 0x02U,
  HTS221_TIMEOUT  = 0x03U
} HTS221_StatusTypeDef;



typedef __packed struct HTS221_Data
{
	float Temperature;
	
	uint16_t Humidity;
	
}HTS221_Data_TypeDef;

HTS221_StatusTypeDef HTS221_who_am_i(void);
HTS221_StatusTypeDef HTS221_Calibration(void);
HTS221_StatusTypeDef HTS221_GetHumidity(uint16_t *pfData);
HTS221_StatusTypeDef HTS221_GetTemperature(float *pfData);
HTS221_StatusTypeDef print_all_reg(void);

#endif /* __LIS3DH_H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
