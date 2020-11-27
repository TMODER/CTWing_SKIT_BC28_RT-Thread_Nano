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



extern NET_STATUS net_status;

extern uint8_t  at_response_buff[512];

//const char Days[] = {1,2,3,4,5,6,7,8,9,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30,0x31};

//const char Mon[] = {1,2,3,4,5,6,7,8,9,0x10,0x11,0x12};



//static time_t mon_yday[2][12] =
//{
//	{0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
//	{0,31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
//};


uint8_t key_scan(void)
{

    uint8_t keycode = 0;
    uint8_t wait_time = 100;

    if (!HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin))
    {
        rt_thread_mdelay(wait_time);
        if (!HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin))
        {
            keycode = (1 << 1);
        }

    }

    if (!HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin))
    {
        rt_thread_mdelay(wait_time);
        if (!HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin))
        {
            keycode |= 1;
        }
    }
    // keycode=(!(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))<<1)|(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin));

    return keycode;
}


void SetMotorSpeed(int speed)
{
    if (speed > 0)
    {
        TIM3->CCR3 = speed * 10;
        TIM3->CCR4 = 0;
    }
    else
    {
        TIM3->CCR3 = 0;
        TIM3->CCR4 = abs(speed) * 10;
    }
}


uint8_t  at_send_cmd(uint8_t *cmd, uint8_t *resp, uint16_t readlen, uint16_t waittime)
{
//    uint16_t readsize = 0;

   // uint8_t time = 10;
    uint8_t  result=0;
    uint8_t  buff[] = "\r\n";
	  

    HAL_UART_Transmit(&huart1, cmd, strlen((char *)cmd), 0XFFFFFFF);
    HAL_UART_Transmit(&huart1, buff, sizeof(buff), 0XFFFFFFF);
	  rt_thread_mdelay(waittime);
    memcpy(resp,at_response_buff, readlen);
	
    if (strstr((char *)resp, "OK"))
    {

        result= AT_RESPONSE_OK;
    }
		else
		{
				if (strstr((char *)resp, "ERROR"))
				{
						rt_kprintf("at response error:%s\r\n", resp);

						result= AT_RESPONSE_ERROR;
				}
				else 
				{
				rt_kprintf("send at cmd:%s no response\r\n", cmd);

				result= AT_RESPONSE_NULL;
				}
	  }
		
		memset(at_response_buff,0,sizeof(at_response_buff));
		
		return result;

}

uint8_t crc_sum_check(uint8_t *buff, uint16_t len)
{
    uint8_t crc = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        crc += buff[i];
    }
    return crc;
}


////////////////   eg:   50  return 0x50
//uint8_t dec2hex(uint8_t dec)
//{
//	uint8_t hex=0;
//
//	if(dec<10)
//	{
//		return dec;
//	}
//
//	else
//  {
//		hex=(dec/10)*6+dec;
//	}
//	return hex;
//}

void reboot_module(void)
{
    uint8_t sendbuff[] = "AT+NRB\r\n";
    HAL_UART_Transmit(&huart1, sendbuff, sizeof(sendbuff), 0XFFFFFFF);
}
FINSH_FUNCTION_EXPORT_ALIAS(reboot_module, __cmd_module_reboot, Reboot Module);  //重启模块

//返回从模组 指令集中获取到的时间戳   入参＋ 时区
time_t get_module_time(uint8_t time_zone)
{

    //+CCLK:18/11/09,05:36:42+32

    time_t timestamp = 0;
    uint8_t recvbuff[50] = {0};
    uint8_t databuff[5] = {0};
    char *index = 0;
    struct tm module_time = {0};

    if (AT_RESPONSE_OK == at_send_cmd((uint8_t  *)"AT+CCLK?", recvbuff, sizeof(recvbuff), 300))
    {
			
       rt_kprintf("time:%s\r\n",recvbuff);
			  index =  (char*)at_response_buff+strlen("AT+CCLK?");
        index = strstr((char *)recvbuff, "+CCLK:");
        if (index)
        {
            index += 6;
            memcpy(databuff, index, 2);
            module_time.tm_year = atoi((char *)databuff) + 100;
            memset(databuff, 0, sizeof(databuff));

            index += 3;
            memcpy(databuff, index, 2);
            module_time.tm_mon = atoi((char *)databuff) - 1;
            memset(databuff, 0, sizeof(databuff));

            index += 3;
            memcpy(databuff, index, 2);
            module_time.tm_mday = atoi((char *)databuff);
            memset(databuff, 0, sizeof(databuff));

            index += 3;
            memcpy(databuff, index, 2);
            module_time.tm_hour = atoi((char *)databuff);
            memset(databuff, 0, sizeof(databuff));

            index += 3;
            memcpy(databuff, index, 2);
            module_time.tm_min = atoi((char *)databuff);
            memset(databuff, 0, sizeof(databuff));

            index += 3;
            memcpy(databuff, index, 2);
            module_time.tm_sec = atoi((char *)databuff);
            memset(databuff, 0, sizeof(databuff));

            timestamp = mktime(&module_time) + time_zone * 3600;

            return timestamp;

        }

        else
        {
				//	rt_kprintf("time:%s\r\n",recvbuff);
            return RT_NULL;
        }
    }
    else
    {
			//rt_kprintf("time:%s\r\n",recvbuff);
        return RT_NULL;
    }

}


static void datetime(int argc, char **argv)
{

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef DateToUpdate = {0};
    char buff[2] = {0};
    char index = 0;
	 HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
    if (argc < 2)
    {

        rt_kprintf("%02d-%02d-%02d %02d:%02d:%02d\r\n", 2000 + DateToUpdate.Year, DateToUpdate.Month, DateToUpdate.Date,
                   sTime.Hours, sTime.Minutes, sTime.Seconds);
        return;
    }

    if (strlen(argv[1]) == 6)
    {
        // rt_kprintf("%s\n",argv[1]);
        memcpy(buff, argv[1], 2);
        sTime.Hours = (atoi(buff));
        index += 2;

        memcpy(buff, argv[1] + index, 2);
        sTime.Minutes = (atoi(buff));
        index += 2;

        memcpy(buff, argv[1] + index, 2);
        sTime.Seconds = (atoi(buff));
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);


		if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4) != DateToUpdate.Date)
		{
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BKUP_REG_DATA);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, DateToUpdate.Year);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, DateToUpdate.Month);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, DateToUpdate.Date);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, DateToUpdate.WeekDay);
		}
		
       // HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BKUP_REG_DATA);
        return;
    }

    if (strlen(argv[1]) == 8)
    {
        //rt_kprintf("AT server!\n");

        return;
    }


}
MSH_CMD_EXPORT(datetime, get(set) datetime(eg: 103154));


uint8_t rt_rtc_set_time(time_t timestamp, uint32_t Format)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef DateToUpdate = {0};
    struct tm *tm_localtime;
	
    HAL_PWR_EnableBkUpAccess();
    tm_localtime = localtime(&timestamp);

    // rt_kprintf("localtime %s\r\n", asctime(test_tm));
    //localtime(time,&test_tm);


    sTime.Hours = (tm_localtime->tm_hour);
    sTime.Minutes = (tm_localtime->tm_min);
    sTime.Seconds = (tm_localtime->tm_sec);


    if (HAL_RTC_SetTime(&hrtc, &sTime, Format) != HAL_OK)
    {
        Error_Handler();
    }


    DateToUpdate.WeekDay = (tm_localtime->tm_wday);  //周


    DateToUpdate.Month = (tm_localtime->tm_mon + 1); //月

    DateToUpdate.Date = (tm_localtime->tm_mday);    //天

    DateToUpdate.Year = (tm_localtime->tm_year + 1900 - 2000);

    if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, Format) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN)!=HAL_OK)
    	{
    	Error_Handler();
    	}

	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4) != DateToUpdate.Date)
    {
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BKUP_REG_DATA);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, DateToUpdate.Year);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, DateToUpdate.Month);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, DateToUpdate.Date);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, DateToUpdate.WeekDay);
    }

    rt_kprintf("update device time success..\r\n");
    return RT_TRUE;

}

//模块附网后 at指令对时
uint8_t  calibration_rtc_from_iot_module(void)
{
    time_t timestamp = 0;
    if (net_status == NET_DOWN)
    {
        rt_kprintf("device net status is down..\r\n");
        return RT_FALSE;
    }
    else
    {
        timestamp = get_module_time(8);
        if (timestamp)
        {

            rt_rtc_set_time(timestamp, RTC_FORMAT_BIN);
            return RT_TRUE;
        }
        else
        {

            rt_kprintf("get module time error..\r\n");
            return RT_FALSE;
        }
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(calibration_rtc_from_iot_module, __cmd_ntp_time,
                            get module online time set to device RTC_Clock);  //


void System_Enter_Standby(void)
{
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);  //PWR_FLAG_SB
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnterSTANDBYMode();
}
FINSH_FUNCTION_EXPORT_ALIAS(System_Enter_Standby, __cmd_enter_standby, system enter standby mode); //





