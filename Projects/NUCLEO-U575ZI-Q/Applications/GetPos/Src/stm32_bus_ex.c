/**
  ******************************************************************************
  * @file    stm32_bus_ex.c
  * @author  MCD Application Team
  * @brief   Source file for GNSS1A1 Bus Extension
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_nucleo_bus.h"
#include "stm32_bus_ex.h"

/**
  * @brief  Transmit data to the device through BUS.
  * @param  DevAddr Device address on Bus (I2C only).
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_LPUART1_Send_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  UNUSED(DevAddr);

  if(HAL_UART_Transmit_IT(&hlpuart1, (uint8_t *)pData, Length) == HAL_OK)
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
int32_t BSP_LPUART1_Recv_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  UNUSED(DevAddr);

  if(HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)pData, Length) == HAL_OK)
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
int32_t BSP_LPUART1_RegisterRxCallback(pUART_CallbackTypeDef pCallback)
{
  return HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_RX_COMPLETE_CB_ID, pCallback);
}

int32_t BSP_LPUART1_RegisterErrorCallback(pUART_CallbackTypeDef pCallback)
{
  return HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_ERROR_CB_ID, pCallback);
}
#endif

void BSP_LPUART1_ClearOREF(void)
{
  __HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_ORE);
}

void BSP_LPUART1_IRQHanlder(void)
{
  HAL_UART_IRQHandler(&hlpuart1);
}

