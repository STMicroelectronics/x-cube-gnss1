/**
  ******************************************************************************
  * @file    stm32_bus_ex.c
  * @author  MCD Application Team
  * @brief   Source file for GNSS1A1 Bus Extension
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_nucleo_bus.h"
#include "stm32_bus_ex.h"

/**
  * @brief  Transmit data to the device through BUS.
  * @param  DevAddr Device address on Bus (I2C only).
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_USART1_Send_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  UNUSED(DevAddr);

  if(HAL_UART_Transmit_IT(&huart1, (uint8_t *)pData, Length) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    ret =  BSP_ERROR_PERIPH_FAILURE;
  }
  return ret;
}

/**
  * @brief  Receive data from the device through BUS.
  * @param  DevAddr Device address on Bus (I2C only).
  * @param  pData Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_USART1_Recv_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  UNUSED(DevAddr);

  if(HAL_UART_Receive_IT(&huart1, (uint8_t *)pData, Length) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    ret =  BSP_ERROR_PERIPH_FAILURE;
  }
  return ret;
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_USART1_RegisterRxCallback(pUART_CallbackTypeDef pCallback)
{
  return HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_COMPLETE_CB_ID, pCallback);
}

int32_t BSP_USART1_RegisterErrorCallback(pUART_CallbackTypeDef pCallback)
{
  return HAL_UART_RegisterCallback(&huart1, HAL_UART_ERROR_CB_ID, pCallback);
}
#endif

void BSP_USART1_ClearOREF(void)
{
  __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_ORE);
}

void BSP_USART1_IRQHanlder(void)
{
  HAL_UART_IRQHandler(&huart1);
}

