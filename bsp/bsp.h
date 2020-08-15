#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>
#include "stm32f1xx_hal.h"

#include "delay.h"
#include "led.h"
#include "motor.h"
#include "hal_iic.h"
#include "app_uart.h"
#include "log_uart.h"
#include "hal_iic.h"
#include "HTS221.h"

/**********************************************************/

#define NB_STARTER_KIT_HARD_VERSION     "V1.0"

#define NB_STARTER_KIT_SOFT_VERSION     "V1.1"

/**********************************************************/

typedef struct
{
	uint8_t  led0_status;
	uint8_t  led1_status;
	uint8_t  motor_status;
	float  temp;
	uint16_t hump;
	uint8_t  key_value;
	uint8_t  IR_status;
}BSP_status_t;

typedef enum
{
  LED0_DEVICE_ID =0,
  LED1_DEVICE_ID = 1,
	MOTOR1         = 2,
	HUM_TEMP       = 3,
	KEY1           = 4,
	KEY2           = 5,
	IR             = 6,
	BSP_ALL
}BSP_DEVICE_ID;


/*************************************************************************/
void module_reset(void);
void module_power_on(void);

void setStatus(BSP_DEVICE_ID device_id ,unsigned char status);
uint8_t getStatus(BSP_DEVICE_ID device_id, BSP_status_t *bsp_status);

void bsp_init(void);
void platform_init(void);

#endif
