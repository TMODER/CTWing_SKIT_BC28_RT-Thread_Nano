/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include "rtthread.h"
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
	
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
   HAL_PWR_EnableBkUpAccess();
	 //uint32_t  bc=HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
	
	 if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != BKUP_REG_DATA)
    {
         rt_kprintf("RTC hasn't been configured, please use <date> command to config.\r\n");
			
			   RTC_TimeTypeDef sTime = {0};
         RTC_DateTypeDef DateToUpdate = {0};

		 
				sTime.Hours =0;
				sTime.Minutes = 0;
				sTime.Seconds =0;

				if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
				{
					Error_Handler();
				}
				DateToUpdate.WeekDay = 0;
				DateToUpdate.Month = 1;
				DateToUpdate.Date = 1;
				DateToUpdate.Year = 20;

				if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
				{
					Error_Handler();
				}
			
		}
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
}



void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
   __HAL_RTC_SECOND_ENABLE_IT(&hrtc,ENABLE);
    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}


 void rt_rtc_f1_bkp_update(void)
{
    RTC_DateTypeDef RTC_DateStruct = {0};

    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKP_CLK_ENABLE();

    RTC_DateStruct.Year    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    RTC_DateStruct.Month   = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    RTC_DateStruct.Date    = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    RTC_DateStruct.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    if (HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4) != RTC_DateStruct.Date)
    {
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, BKUP_REG_DATA);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, RTC_DateStruct.Year);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, RTC_DateStruct.Month);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, RTC_DateStruct.Date);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, RTC_DateStruct.WeekDay);
    }
}




//void rt_rtc_config(void)
//{
//    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

//    HAL_PWR_EnableBkUpAccess();
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//#ifdef BSP_RTC_USING_LSI
//    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
//#else
//    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
//#endif
//    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

//    /* Enable RTC Clock */
//    __HAL_RCC_RTC_ENABLE();

//    RTC_Handler.Instance = RTC;
//    if (HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR1) != BKUP_REG_DATA)
//    {
//        LOG_I("RTC hasn't been configured, please use <date> command to config.");

//#if defined(SOC_SERIES_STM32F1)
//        RTC_Handler.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
//        RTC_Handler.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
//#elif defined(SOC_SERIES_STM32F0)

//        /* set the frequency division */
//#ifdef BSP_RTC_USING_LSI
//        RTC_Handler.Init.AsynchPrediv = 0XA0;
//        RTC_Handler.Init.SynchPrediv = 0xFA;
//#else
//        RTC_Handler.Init.AsynchPrediv = 0X7F;
//        RTC_Handler.Init.SynchPrediv = 0x0130;
//#endif /* BSP_RTC_USING_LSI */

//        RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;
//        RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
//        RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//        RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//#elif defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32H7)

//        /* set the frequency division */
//#ifdef BSP_RTC_USING_LSI
//        RTC_Handler.Init.AsynchPrediv = 0X7D;
//#else
//        RTC_Handler.Init.AsynchPrediv = 0X7F;
//#endif /* BSP_RTC_USING_LSI */
//        RTC_Handler.Init.SynchPrediv = 0XFF;

//        RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;
//        RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
//        RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//        RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//#endif
//        if (HAL_RTC_Init(&RTC_Handler) != HAL_OK)
//        {
//            return -RT_ERROR;
//        }
//    }
//#ifdef SOC_SERIES_STM32F1
//    else
//    {
//        /* F1 series need update by bkp reg datas */
//        rt_rtc_f1_bkp_update();
//    }
//#endif

//    return RT_EOK;
//}


void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
