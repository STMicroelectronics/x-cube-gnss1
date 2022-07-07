/**
  ******************************************************************************
  * @file    flash.h
  * @author  MCD Application Team
  * @brief   Management of the internal flash memory.
  *          Header for flash.c
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
#ifndef __flash_H
#define __flash_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size);


#if defined (STM32L475xx) || defined (STM32L496xx)
uint32_t FLASH_Write(uint32_t uDestination, uint32_t *pSource, uint32_t uLength);
uint32_t FLASH_Erase_Size(uint32_t uStart, uint32_t uLength);
#else
int FLASH_write_at(uint32_t address, uint32_t *pData, uint32_t len_bytes);
uint32_t GetSectorMap(void);
#endif



#ifdef __cplusplus
}
#endif

#endif /* __flash_H */

