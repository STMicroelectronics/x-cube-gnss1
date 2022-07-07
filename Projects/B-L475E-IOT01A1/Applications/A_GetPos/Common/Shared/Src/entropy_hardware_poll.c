/**
  ******************************************************************************
  * @file    entropy_hardware_poll.c
  * @author  MCD Application Team
  * @brief   This file provides code for the entropy collector.
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
#include <string.h>
#include "main.h"

int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen );


int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t random_number = 0;
  
  status = HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
  ((void) data);
  *olen = 0;
  
  if ((len < sizeof(uint32_t)) || (HAL_OK != status))
  {
    return 0;
  }
  
  memcpy(output, &random_number, sizeof(uint32_t));
  *olen = sizeof(uint32_t);
  
  return 0;
}

