
#include "bsp.h"
#include "TinyLog.h"
#include "util.h"
#include "bsp_thread.h"

#define MODULE_POWER_ON_PIN   GPIOA,GPIO_PIN_8
#define MODULE_RESET_PIN      GPIOA,GPIO_PIN_5

#define BSP_KEY1              GPIOA,GPIO_PIN_6
#define BSP_KEY2              GPIOA,GPIO_PIN_7
#define BSP_IR                GPIOA,GPIO_PIN_11

int8_t read_key_value(uint8_t id)
{
	GPIO_PinState value;
	
	if( id == KEY1 )
	{
		value = HAL_GPIO_ReadPin(BSP_KEY1);
		return value;
	}
	else if( id == KEY2 )
	{
		value = HAL_GPIO_ReadPin(BSP_KEY2);
		return value;
	}
	
	return -1;
}

int8_t read_IR_value(uint8_t id)
{
	GPIO_PinState value;
	
	value = HAL_GPIO_ReadPin(BSP_IR);
		
	return value;
}

/* 
----       ------
		|______|


*/
void module_power_on(void)
{
	HAL_GPIO_WritePin(MODULE_POWER_ON_PIN, GPIO_PIN_RESET);
	delay_ms(500);
	HAL_GPIO_WritePin(MODULE_POWER_ON_PIN, GPIO_PIN_SET);
	delay_ms(1000);
	HAL_GPIO_WritePin(MODULE_POWER_ON_PIN, GPIO_PIN_RESET);
}

/* 
----       ------
		|______|


*/
void module_reset(void)
{
	HAL_GPIO_WritePin(MODULE_RESET_PIN, GPIO_PIN_RESET);
	delay_ms(500);
	HAL_GPIO_WritePin(MODULE_RESET_PIN, GPIO_PIN_SET);
	delay_ms(1000);
	HAL_GPIO_WritePin(MODULE_RESET_PIN, GPIO_PIN_RESET);
}

void setStatus(BSP_DEVICE_ID device_id,unsigned char status)
{
	switch(device_id)
	{
		case MOTOR1:
			os_log("MOTOR:%d\r\n",status);
			motor_write(MOTOR1,status);
			break;

		case LED0_DEVICE_ID:
			led_write(LED0_DEVICE_ID,status);
			break;

		case LED1_DEVICE_ID:
			led_write(LED1_DEVICE_ID,status);
			break;

		default:
			break;
	}
}
uint8_t getStatus(BSP_DEVICE_ID device_id, BSP_status_t *bsp_status)
{
  
	switch(device_id)
	{
		case MOTOR1:
			motor_read(MOTOR1, &bsp_status->motor_status);
			break;

		case LED0_DEVICE_ID:
			led_read(LED0_DEVICE_ID, &bsp_status->led0_status);
			break;

		case LED1_DEVICE_ID:
			led_read(LED1_DEVICE_ID, &bsp_status->led1_status);
			break;
		
		case HUM_TEMP:
			get_hum_temp(&bsp_status->temp, &bsp_status->hump);
			break;
		
		case KEY1:
		case KEY2:
			bsp_status->key_value = read_key_value(device_id);
			break;
		
		case IR:
			bsp_status->IR_status = read_IR_value(device_id);
			break;
		
		case BSP_ALL:
			motor_read(MOTOR1, &bsp_status->motor_status);
		  led_read(LED0_DEVICE_ID, &bsp_status->led0_status);
		  led_read(LED1_DEVICE_ID, &bsp_status->led1_status);
		  bsp_status->IR_status = read_IR_value(device_id);
		  get_hum_temp(&bsp_status->temp, &bsp_status->hump);
			break;

		default:
			break;
	}
	
	return 0;
}


void bsp_init(void)
{
	uint8_t ret;
	
  setStatus(LED0_DEVICE_ID, LED_BLUE);
  setStatus(LED1_DEVICE_ID, LED_GREEN);
	//motor_write(MOTOR1, MOTOR_BACK_UP);
	//delay_ms(1000);
	//motor_write(MOTOR1, MOTOR_OFF);
	
	app_uart_init(9600, 30);
	delay_ms(500);
	
	ret = HTS221_who_am_i();
	if( ret != HTS221_OK )
	{
		os_log("HTS221 init fail\r\n");
	}
}

void platform_init(void)
{
	os_log("***********system start**********\r\n");
	os_log("MCU:STM32F103\r\n");
	os_log("\r\nHCLK=systemclock:%dHZ\r\n",SystemCoreClock);
	os_log("\r\nPCLK1=%d\r\n",SystemCoreClock>>1);
	os_log("\r\nPCLK2=%d\r\n",SystemCoreClock);
	
	if(isLittleEndian() == LITTLE_ENDIAN)
	{
		os_log("\r\nEndian: Little Endian\r\n");
	}
	else
	{
		os_log("\r\nEndian: Big Endian\r\n");
	}
	
	os_log("\r\nBuild time:%s,%s\r\n",__DATE__,__TIME__);
}
