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
#include "stm32u5xx_nucleo_bus.h"
#include "stm32_bus_ex.h"

/**
  * @brief  Transmit data to the device through BUS.
  * @param  DevAddr Device address on Bus (I2C only).
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C1_Send_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit_IT(&hi2c1, (uint8_t)DevAddr,
                                         (uint8_t *)pData, Length) == HAL_OK)
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
int32_t BSP_I2C1_Recv_IT(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive_IT(&hi2c1, (uint8_t)DevAddr,
                                        (uint8_t *)pData, Length) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    ret =  BSP_ERROR_PERIPH_FAILURE;
  }
  return ret;
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C1_RegisterRxCallback(pI2C_CallbackTypeDef pCallback)
{
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }
  return HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MASTER_RX_COMPLETE_CB_ID, pCallback);
}

int32_t BSP_I2C1_RegisterErrorCallback(pI2C_CallbackTypeDef pCallback)
{
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }
  return HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_ERROR_CB_ID, pCallback);
}

int32_t BSP_I2C1_RegisterAbortCallback(pI2C_CallbackTypeDef pCallback)
{
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }
  return HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_ABORT_CB_ID, pCallback);
}
#endif

void BSP_EV_I2C1_IRQHanlder(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

void BSP_ER_I2C1_IRQHanlder(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}

