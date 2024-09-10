/**
  ******************************************************************************
  * @file    gnss1a1_conf.h
  * @author  SRA Application Team
  * @brief   This file contains definitions for the GNSS components bus
  *          interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GNSS1A1_CONF_H
#define GNSS1A1_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_bus.h"
#include "stm32f4xx_nucleo_errno.h"

#define USE_I2C 0U

#define hgnss_uart huart1

#define USE_GNSS1A1_GNSS_TESEO_LIV3F	1U

#define GNSS1A1_GNSS_UART_Init        BSP_USART1_Init
#define GNSS1A1_GNSS_UART_DeInit      BSP_USART1_DeInit

#define GNSS1A1_RST_PORT                        GPIOA
#define GNSS1A1_RST_PIN                         GPIO_PIN_8

#define GNSS1A1_WAKEUP_PORT                     GPIOA
#define GNSS1A1_WAKEUP_PIN                      GPIO_PIN_5

//#define GNSS1A1_RegisterDefaultMspCallbacks     BSP_USART1_RegisterDefaultMspCallbacks

/* To be checked */
#define GNSS1A1_UART_IRQHanlder                 BSP_USART1_IRQHanlder

#ifdef __cplusplus
}
#endif

#endif /* GNSS1A1_CONF_H */

