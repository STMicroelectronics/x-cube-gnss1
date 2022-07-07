/**
  ******************************************************************************
  * @file    gnss1a1_conf.h
  * @author  SRA
  * @brief   This file contains definitions for the GNSS components bus interfaces
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GNSS1A1_CONF_H
#define GNSS1A1_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "stm32l4xx_nucleo_bus.h"
#include "stm32_bus_ex.h"
#include "stm32l4xx_nucleo_errno.h"

#define USE_I2C 0U

#define USE_GNSS1A1_GNSS_TESEO_LIV3F	1U

#if (USE_I2C == 1)
#define GNSS1A1_GNSS_I2C_Init BSP_I2C1_Init
#define GNSS1A1_GNSS_I2C_DeInit BSP_I2C1_DeInit
#define GNSS1A1_GNSS_I2C_Transmit_IT BSP_I2C1_Send_IT
#define GNSS1A1_GNSS_I2C_Receive_IT BSP_I2C1_Recv_IT
#else
#define GNSS1A1_GNSS_UART_Init BSP_UART4_Init
#define GNSS1A1_GNSS_UART_DeInit BSP_UART4_DeInit
#define GNSS1A1_GNSS_UART_Transmit_IT BSP_UART4_Send_IT
#define GNSS1A1_GNSS_UART_Receive_IT BSP_UART4_Recv_IT
#define GNSS1A1_GNSS_UART_ClearOREF BSP_UART4_ClearOREF
#endif /* USE_I2C */

#define GNSS1A1_GNSS_GetTick BSP_GetTick

#define GNSS1A1_RST_PORT                        GPIOA
#define GNSS1A1_RST_PIN                         GPIO_PIN_4

#define GNSS1A1_WAKEUP_PORT                     GPIOA
#define GNSS1A1_WAKEUP_PIN                      GPIO_PIN_5

#if (USE_I2C == 1)
#define GNSS1A1_RegisterRxCb                    BSP_I2C1_RegisterRxCallback
#define GNSS1A1_RegisterErrorCb                 BSP_I2C1_RegisterErrorCallback
#define GNSS1A1_RegisterAbortCb                 BSP_I2C1_RegisterAbortCallback
#else
#define GNSS1A1_RegisterRxCb                    BSP_UART4_RegisterRxCallback
#define GNSS1A1_RegisterErrorCb                 BSP_UART4_RegisterErrorCallback
#endif /* USE_I2C */

#ifdef __cplusplus
}
#endif

#endif /* GNSS1A1_CONF_H */
