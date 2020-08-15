/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Print_TX_Pin GPIO_PIN_2
#define Print_TX_GPIO_Port GPIOA
#define Print_RX_Pin GPIO_PIN_3
#define Print_RX_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_6
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOA
#define MOTOR_A_Pin GPIO_PIN_0
#define MOTOR_A_GPIO_Port GPIOB
#define MOTOR_B_Pin GPIO_PIN_1
#define MOTOR_B_GPIO_Port GPIOB
#define RGB1_R_Pin GPIO_PIN_2
#define RGB1_R_GPIO_Port GPIOB
#define RGB1_G_Pin GPIO_PIN_10
#define RGB1_G_GPIO_Port GPIOB
#define RGB1_B_Pin GPIO_PIN_11
#define RGB1_B_GPIO_Port GPIOB
#define RGB2_R_Pin GPIO_PIN_12
#define RGB2_R_GPIO_Port GPIOB
#define RGB2_G_Pin GPIO_PIN_13
#define RGB2_G_GPIO_Port GPIOB
#define RGB2_B_Pin GPIO_PIN_14
#define RGB2_B_GPIO_Port GPIOB
#define VBAT_Pin GPIO_PIN_15
#define VBAT_GPIO_Port GPIOB
#define VBAT_EXTI_IRQn EXTI15_10_IRQn
#define Module_TX_Pin GPIO_PIN_9
#define Module_TX_GPIO_Port GPIOA
#define Module_RX_Pin GPIO_PIN_10
#define Module_RX_GPIO_Port GPIOA
#define IR_DATA_Pin GPIO_PIN_11
#define IR_DATA_GPIO_Port GPIOA
#define I2C_SCL_Pin GPIO_PIN_6
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_7
#define I2C_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
