#include "com.h"
#include "string.h"
#include "stdio.h"
#include "rtthread.h"
#include <stdlib.h>
#include "main.h"
#include "rtc.h"
#include <time.h>


extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;

extern struct rt_ringbuffer uart_bc28_rxcb;

extern  rt_sem_t  at_rx_sem ; 
extern uint8_t  at_sending_flag;


extern NET_STATUS net_status;

//const char Days[] = {1,2,3,4,5,6,7,8,9,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31};

//const char Mon[] = {1,2,3,4,5,6,7,8,9,0x10,0x11,0x12};


	
//static time_t mon_yday[2][12] =
//{
//	{0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
//	{0,31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
//};


uint8_t key_scan(void)
{
	
	uint8_t keycode=0;
	uint8_t wait_time=100;
	
	if(!HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))
	{
	  rt_thread_mdelay(wait_time);
		if(!HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))
		{
			keycode=(1<<1);
		}
		
	}
	
  if(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin))
	{
	  rt_thread_mdelay(wait_time);
		if(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin))
		{
			keycode|=1;
		}		
	}
 // keycode=(!(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))<<1)|(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin));
		
	return keycode;
}


void SetMotorSpeed(int speed)
{
	if(speed>0)
	{
		TIM3->CCR3=speed*10;
    TIM3->CCR4=0;
	}
	else
	{
		TIM3->CCR3=0;
    TIM3->CCR4=abs(speed)*10;
	}
}

extern uint8_t  at_sending_flag;

uint8_t  at_send_cmd(uint8_t *cmd,uint8_t * resp,uint16_t readlen, uint16_t waittime)
{
	uint16_t readsize=0;
	
	uint8_t time=10;
	
	uint8_t  buff[]="\r\n";
	
	at_sending_flag=1;
	
	memset(resp,0,readlen);
  
	HAL_UART_Transmit(&huart1,cmd,strlen((char *)cmd),0XFFFFFFF);
	HAL_UART_Transmit(&huart1,buff,sizeof(buff),0XFFFFFFF);
		
  do
  {
		readsize =rt_ringbuffer_data_len(&uart_bc28_rxcb);		
    if(readsize)
		{			
			rt_thread_mdelay(waittime);
			rt_ringbuffer_get(&uart_bc28_rxcb,resp,readlen);
			break;
		}
   rt_thread_mdelay(5);
	}		
	while(time--);
		
		if(strstr((char *)resp,"OK"))
		{
      at_sending_flag=0;
		  return AT_RESPONSE_OK;
		}	
	
	if(strstr((char *)resp,"ERROR"))
		{			
			rt_kprintf("at response error:%s\r\n",resp);
      at_sending_flag=0;
		  return AT_RESPONSE_ERROR;
		}			
	 rt_kprintf("send at cmd:%s no response\r\n",cmd);
   at_sending_flag=0;
		return AT_RESPONSE_NULL;		

}

uint8_t crc_sum_check(uint8_t *buff,uint16_t len)
{
	uint8_t crc=0;
	for(uint16_t i=0;i<len;i++)
	{
		crc+=buff[i];
	}
	return crc;
}


////////////////   eg:   50  return 0x50
uint8_t dec2hex(uint8_t dec)
{
	uint8_t hex=0;
	
	if(dec<10)
	{
		return dec;
	}
	
	else
  {
		hex=(dec/10)*6+dec;		
	}
	return hex;
}

void reboot_module(void)
{
	uint8_t sendbuff[]="AT+NRB\r\n";
	HAL_UART_Transmit(&huart1,sendbuff,sizeof(sendbuff),0XFFFFFFF);	
}
FINSH_FUNCTION_EXPORT_ALIAS(reboot_module, __cmd_module_reboot, Reboot Module);  //重启模块

//返回从模组 指令集中获取到的时间戳   入参＋ 时区
time_t get_module_time(uint8_t time_zone)
{
	
	//+CCLK:18/11/09,05:36:42+32
	
	time_t timestamp=0;
	uint8_t recvbuff[50]={0};
	uint8_t databuff[5]={0};
	char * index=0;
	struct tm module_time={0};
	
	if(AT_RESPONSE_OK==at_send_cmd("AT+CCLK?",recvbuff,sizeof(recvbuff),300))
	{
		index=strstr((char *)recvbuff,"+CCLK:");
		if(index)
		{
			index+=6;
		  memcpy(databuff,index,2);
			module_time.tm_year=atoi((char *)databuff)+100;
			memset(databuff,0,sizeof(databuff));
			
			index+=3;
			memcpy(databuff,index,2);
			module_time.tm_mon=atoi((char *)databuff)-1;
			memset(databuff,0,sizeof(databuff));
			
			index+=3;
			memcpy(databuff,index,2);
			module_time.tm_mday=atoi((char *)databuff);
			memset(databuff,0,sizeof(databuff));
			
			index+=3;
			memcpy(databuff,index,2);
			module_time.tm_hour=atoi((char *)databuff);
			memset(databuff,0,sizeof(databuff));
			
			index+=3;
			memcpy(databuff,index,2);
			module_time.tm_min=atoi((char *)databuff);
			memset(databuff,0,sizeof(databuff));
			
			index+=3;
			memcpy(databuff,index,2);
			module_time.tm_sec=atoi((char *)databuff);
			memset(databuff,0,sizeof(databuff));
			
			timestamp=mktime(&module_time)+time_zone*3600;
			
			return timestamp;
					
		}
		
		else
		{
			return RT_NULL;
		}
	}
	else
	{
			return RT_NULL;
	}
	
}


static void datetime(int argc, char**argv)
{
		
	 RTC_TimeTypeDef sTime = {0};
   RTC_DateTypeDef DateToUpdate = {0};
	 if(argc<2)
	 {
	 HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BCD);
	 HAL_RTC_GetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BCD);
	 rt_kprintf("20%02x-%02x-%02x %02x:%02x:%02x\r\n",DateToUpdate.Year,DateToUpdate.Month,DateToUpdate.Date,sTime.Hours,sTime.Minutes,sTime.Seconds);
	 }	
   else
	{
			
		}			
}
MSH_CMD_EXPORT(datetime,get(set) time);


uint8_t rt_rtc_set_time(time_t timestamp,uint32_t Format)
{
	   RTC_TimeTypeDef sTime = {0};
     RTC_DateTypeDef DateToUpdate = {0};
		 struct tm * tm_localtime;
     char * data=0;
    // HAL_PWR_EnableBkUpAccess();
     tm_localtime =localtime(&timestamp);
				 
		// rt_kprintf("localtime %s\r\n", asctime(test_tm));
		 //localtime(time,&test_tm);
		 
		 
		sTime.Hours = dec2hex(tm_localtime->tm_hour);
		sTime.Minutes = dec2hex(tm_localtime->tm_min);
		sTime.Seconds = dec2hex(tm_localtime->tm_sec);

		 
		if (HAL_RTC_SetTime(&hrtc, &sTime, Format) != HAL_OK)
		{
			Error_Handler();
		}
		
		
		DateToUpdate.WeekDay =dec2hex(tm_localtime->tm_wday);   //周	

		
		DateToUpdate.Month = dec2hex(tm_localtime->tm_mon+1);   //月
		
		DateToUpdate.Date = dec2hex(tm_localtime->tm_mday);    //天
		
		DateToUpdate.Year = dec2hex(tm_localtime->tm_year+1900-2000);

		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, Format) != HAL_OK)
		{
			Error_Handler();
		}
	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BKUP_REG_DATA);
		
		//print_out_rtc_time();
		rt_kprintf("update device time success..\r\n");
	 
}


uint8_t  calibration_rtc_from_iot_module(void)
{
	time_t timestamp=0;
	if(net_status==NET_DOWN)
	{
		rt_kprintf("device net status is down..\r\n");
		return RT_FALSE;
	}
	else
	{
		timestamp=get_module_time(8);
		if(timestamp)
		{
			
			rt_rtc_set_time(timestamp,RTC_FORMAT_BCD);
			return RT_TRUE;
		}
		else
		 {
			 
			 rt_kprintf("get module time error..\r\n");
			 return RT_FALSE;
		 }		
	}
}
FINSH_FUNCTION_EXPORT_ALIAS(calibration_rtc_from_iot_module, __cmd_ntp_time,get module online time set to device RTC);  //重启模块
 



