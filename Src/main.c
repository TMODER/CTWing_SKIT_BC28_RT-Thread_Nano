/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtthread.h"
#include "user_main.h"
#include  "HTS221.h"
#include "com.h"
#include  <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define UART_RX_BUF_LEN 10

rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};  //控制台缓冲区

struct  rt_ringbuffer  uart_rxcb;         /* 定义一个 ringbuffer cb  控制台使用 */

struct rt_semaphore shell_rx_sem; /* 定义一个信号量 控制台使用 */

rt_uint8_t uart_bc28_rx_buff[UART_BC28_RX_BUF_LEN]={0};  //模组串口缓冲区

struct rt_ringbuffer uart_bc28_rxcb;  //模组ringbuff



HTS221_Data_TypeDef _HTS221_Data={0};


extern UART_HandleTypeDef huart1;

uint8_t key_code=0;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static void user_init(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//
//const uint8_t testbuff[14]={0x68, 0xFF ,00, 00, 00 ,00 ,00, 0x68, 0x11, 0x04 ,0x36, 0x34 ,0x39 ,0x35};
//const uint8_t testbuff[10]={0X68, 0xAA ,0xAA ,0xAA ,0xAA ,0xAA, 0xAA ,0x68 ,0x13 ,0x00};
//68 11 11 11 11 11 11 68 1C 10 35 33 33 33（密码） 34 89 67 45 4D 33 47 77 3B 3A 44 3C
//const uint8_t testbuff[]={0x68, 0x5A,0x58,0x4B,0x37,0x53,0x53,0x68 ,0x1C, 0x10 ,0x35, 0x33 ,0x33 ,0x33 ,0x34 ,0x89, 0x67 ,0x45 ,0x4D, 0x33 ,0x47 ,0x77 ,0x3B ,0x3A, 0x44 ,0x3C};
//0x68,0x11,0x11,0x11,0x11,0x11,0x11,0x68,0x04,0x0C,0x34,0x33,0x33,0x33,0x35,0x33,0x33,0x33,0x53,0x81,0x33,0x33

//const uint8_t testbuff[]={0x68,0x5A,0x58,0x4B,0x37,0x53,0x53,0x68,0x04,0x0C,0x34,0x33,0x33,0x33,0x35,0x33,0x33,0x33,0x53,0x81,0x33,0x33};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	 rt_kprintf("----------system clock frequency %d MHz------------\r\n",HAL_RCC_GetHCLKFreq()/1000000);
	 rt_kprintf("-----------------PCLK1 frequency %d MHz------------\r\n",HAL_RCC_GetPCLK1Freq()/1000000);
	 rt_kprintf("-----------------PCLK2 frequency %d MHz------------\r\n",HAL_RCC_GetPCLK2Freq()/1000000);
	
   user_init();
	 creat_user_task();
  /* USER CODE END 1 */
	
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
 // SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */
		 float temp=0;
		 uint16_t humidity=0;
//		 time_t  mytime=1597240650+3600*8;

//   struct tm  *test_tm;
//	
//		// time(&mytime);
//		 
//		 test_tm=localtime(&mytime);
//		
//		 rt_kprintf("localtime %s\r\n", asctime(test_tm));
//		 //localtime(time,&test_tm);
//		 rt_kprintf("year:%d,month:%d,day:%d,hour:%d,min:%d,sec:%d\r\n",test_tm->tm_year,test_tm->tm_mon,test_tm->tm_mday,test_tm->tm_hour,test_tm->tm_min,test_tm->tm_hour);
 
  /* USER CODE END 2 */
	//  rt_kprintf("len: %d \r\n",sizeof(testbuff));
  //  rt_kprintf("crc %02x \r\n..",crc_sum_check((uint8_t *)testbuff,sizeof(testbuff)));
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//print_out_rtc_time();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
     uint8_t printbuff[30]={0};
		 uint16_t j=0;
	//	while(HAL_GPIO_ReadPin(IR_DATA_GPIO_Port,IR_DATA_Pin)
      HAL_GPIO_WritePin(RGB2_B_GPIO_Port,RGB2_R_Pin,HAL_GPIO_ReadPin(IR_DATA_GPIO_Port,IR_DATA_Pin));
	 
//		HAL_UART_Transmit(&huart1,"ATI\r\n",5,100);
    	rt_thread_mdelay(10);
		
		  key_code=key_scan();
		
		if(key_code==1)
		{
			    rt_kprintf("simple start..\r\n");
				
					if(HTS221_GetHumidity(&humidity))
					{
						rt_kprintf("simple humidity failed\r\n");
					}
					if(HTS221_GetTemperature(&temp))
					{
						rt_kprintf("simple temperature failed\r\n");
					}		
     _HTS221_Data.Temperature=temp;
     _HTS221_Data.Humidity=humidity;	
    
					j=sprintf((char *)printbuff,"Temp:%.2f ℃\n",_HTS221_Data.Temperature);
					sprintf((char *)printbuff+j,"Humi:%d %%\n",_HTS221_Data.Humidity);
     					
			   rt_kprintf("%s",printbuff);
		}
		
//HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)
//		
//		readsize=rt_ringbuffer_get(&uart_bc28_rxcb,readbuff,50);
//		
//		if(readsize>0)
//		
//		{
//			rt_kprintf("\r\n");
//			rt_kprintf("read size:%d,read buff:%s",readsize,readbuff);
//			break;
//		}
		
		//rt_memset(readbuff,0,sizeof(readbuff));
		
		//rt_kprintf("123456789\r\n");
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

	
	MX_GPIO_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
	MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	MX_I2C1_Init();
}

/* USER CODE BEGIN 4 */

static void user_init(void)
{
	   
	
	   rt_ringbuffer_init(&uart_bc28_rxcb,uart_bc28_rx_buff,UART_BC28_RX_BUF_LEN);

		/* 初始化串口控制台ringbuff */
      rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);

    /* 初始化串口控制台接收数据的信号量 */
     rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
	
	
//	    at_rx_sem = rt_sem_create("at_rx_dsem", 0, RT_IPC_FLAG_FIFO);
//    if (at_rx_sem == RT_NULL)
//    {
//        rt_kprintf("create dynamic semaphore failed.\n");
//      //  return -1;
//    }
//    if (dynamic_at_mutex == RT_NULL)
//    {
//        rt_kprintf("create dynamic mutex failed.\n");

//    }

	  
	
	   rt_kprintf("HTS221 status:%d\r\n",HTS221_who_am_i());//HTS221_Calibration
	
	  rt_kprintf("HTS221 calibration:%d\r\n",HTS221_Calibration());
	   
	
	   rt_thread_mdelay(3000);
   
	
	 
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
