/**
  ******************************************************************************
  * @file    gnss_lib_config.c
  * @author  SRA
  * @brief   Configure how the libGNSS accesses the GNSS module
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
#include "gnss_lib_config.h"

int32_t GNSS_Wrapper_Send(uint8_t *buffer, uint16_t length)
{
  int32_t status = 0;

  /* USER CODE BEGIN NDEF_Wrapper_ReadData */
  
  /* USER CODE END NDEF_Wrapper_ReadData */

  return status;
}

int32_t GNSS_Wrapper_Reset(void)
{
  int32_t status = 0;

  /* USER CODE BEGIN NDEF_Wrapper_ReadData */
  
  /* USER CODE END NDEF_Wrapper_ReadData */

  return status;
}

void GNSS_Wrapper_Delay(uint32_t Delay)
{
  /* USER CODE BEGIN NDEF_Wrapper_ReadData */
  
  /* USER CODE END NDEF_Wrapper_ReadData */
}

