/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @author  SRA
  * @brief   Interrupt Service Routines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "cmsis_os.h"

#include "gnss1a1_conf.h"

/* Private variables ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void SysTick_Handler(void)
{
  HAL_IncTick();
  osSystickHandler();
}

#if (USE_I2C == 0)
void UART4_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart4);
}

#else

void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}
#endif /* USE_I2C */


void EXTI1_IRQHandler(void)
{
 HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);		
}

void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

