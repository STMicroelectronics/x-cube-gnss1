/**
  ******************************************************************************
  * @file    gnss1a1_conf.h
  * @author  SRA Application Team
  * @brief   This file contains definitions for the GNSS components bus
  *          interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#define USE_GNSS1A1_GNSS_TESEO_LIV3F 1U

#define GNSS1A1_GNSS_UART_INIT       BSP_USART1_Init
#define GNSS1A1_GNSS_UART_DEINIT     BSP_USART1_DeInit
#define GNSS1A1_GNSS_UART_TRANSMIT_IT BSP_USART1_Send_IT
#define GNSS1A1_GNSS_UART_RECEIVE_IT  BSP_USART1_Recv_IT
#define GNSS1A1_GNSS_UART_CLEAR_OREF   BSP_USART1_ClearOREF
#define GNSS1A1_GNSS_GET_TICK         BSP_GetTick

#define GNSS1A1_GNSS_RST                      GNSS1A1_GNSS_Rst

#define GNSS1A1_RST_PORT                        GPIOA
#define GNSS1A1_RST_PIN                         GPIO_PIN_8

#define GNSS1A1_WAKEUP_PORT                     GPIOA
#define GNSS1A1_WAKEUP_PIN                      GPIO_PIN_5

/* #define GNSS1A1_RegisterDefaultMspCallbacks     BSP_USART1_RegisterDefaultMspCallbacks */
#define GNSS1A1_REGISTER_RX_CB                    BSP_USART1_RegisterRxCallback
#define GNSS1A1_REGISTER_ERROR_CB                 BSP_USART1_RegisterErrorCallback

/* To be checked */
#define GNSS1A1_UART_IRQ_HANDLER                 BSP_USART1_IRQHanlder

#ifdef __cplusplus
}
#endif

#endif /* GNSS1A1_CONF_H */

