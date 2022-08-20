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
#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_DC_Pin LL_GPIO_PIN_13
#define LCD_DC_GPIO_Port GPIOC
#define LED0_Pin LL_GPIO_PIN_0
#define LED0_GPIO_Port GPIOA
#define LED1_Pin LL_GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define LED2_Pin LL_GPIO_PIN_2
#define LED2_GPIO_Port GPIOA
#define LED3_Pin LL_GPIO_PIN_3
#define LED3_GPIO_Port GPIOA
#define LED4_Pin LL_GPIO_PIN_4
#define LED4_GPIO_Port GPIOA
#define DB0_Pin LL_GPIO_PIN_0
#define DB0_GPIO_Port GPIOB
#define DB1_Pin LL_GPIO_PIN_1
#define DB1_GPIO_Port GPIOB
#define DB2_Pin LL_GPIO_PIN_2
#define DB2_GPIO_Port GPIOB
#define LCD_EN1_Pin LL_GPIO_PIN_7
#define LCD_EN1_GPIO_Port GPIOF
#define LCD_RW_Pin LL_GPIO_PIN_15
#define LCD_RW_GPIO_Port GPIOA
#define DB3_Pin LL_GPIO_PIN_3
#define DB3_GPIO_Port GPIOB
#define DB4_Pin LL_GPIO_PIN_4
#define DB4_GPIO_Port GPIOB
#define DB5_Pin LL_GPIO_PIN_5
#define DB5_GPIO_Port GPIOB
#define DB6_Pin LL_GPIO_PIN_6
#define DB6_GPIO_Port GPIOB
#define DB7_Pin LL_GPIO_PIN_7
#define DB7_GPIO_Port GPIOB
#define LCD_RST_Pin LL_GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOB
#define LCD_EN2_Pin LL_GPIO_PIN_9
#define LCD_EN2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
