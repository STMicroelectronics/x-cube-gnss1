/**
  ******************************************************************************
  * @file    stm32_bus_ex.h
  * @author  MCD Application Team
  * @brief   Header file for stm32_bus_ex.c
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
#ifndef STM32_BUS_EX_H
#define STM32_BUS_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_nucleo_conf.h"
#include "stm32f4xx_nucleo_errno.h"
#include "stm32f4xx_nucleo.h"

/* Defines -------------------------------------------------------------------*/

#define BUS_USART1_IRQn           USART1_IRQn

int32_t BSP_USART1_Send_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_USART1_Recv_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_USART1_RegisterRxCallback(pUART_CallbackTypeDef pCallback);
int32_t BSP_USART1_RegisterErrorCallback(pUART_CallbackTypeDef pCallback);
#endif

void BSP_USART1_ClearOREF(void);
void BSP_USART1_IRQHanlder(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32_BUS_EX_H */

