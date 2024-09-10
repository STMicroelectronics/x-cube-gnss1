/**
  ******************************************************************************
  * @file    teseo_liv3f_conf.h
  * @author  SRA Application Team
  * @brief   This file contains definitions for the TESEO LIV3F Component
  *          settings
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
#ifndef TESEO_LIV3F_CONF_H
#define TESEO_LIV3F_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#ifdef __GNUC__
  #ifndef __weak
    #define __weak __attribute__((weak))
  #endif
#endif

/* Defines -------------------------------------------------------------------*/
#define USE_osCMSIS 0 /* Do not use CMSIS RTOS wrapper API */

#if !(USE_osCMSIS)
  #define USE_FREE_RTOS_NATIVE_API  0 /* native FreeRTOS API (not supported at application level) */
  #if !(USE_FREE_RTOS_NATIVE_API)
    #define USE_AZRTOS_NATIVE_API  1  /* native AZRTOS API */
  #endif /* USE_FREE_RTOS_NATIVE_API */
#endif /* USE_osCMSIS */

#define TESEO_LIV3F_DEVICE /* ST GNSS device: can be TESEO_LIV3F_DEVICE or TESEO_VIC3DA_DEVICE */

#define GNSS_DEBUG 0 /* Debug off */

#define configUSE_FEATURE 0 /* Feature off */

#define configUSE_ODOMETER 0 /* Odometer off */

#define configUSE_GEOFENCE 0 /* Geofence off */

#define configUSE_DATALOG 0 /* Datalog off */

#if (GNSS_DEBUG == 1)
  #define PRINT_DBG(pBuffer)  GNSS_PRINT(pBuffer)
#else
  #define PRINT_DBG(pBuffer)
#endif

#define PRINT_INFO(pBuffer) GNSS_PRINT(pBuffer)
#define PRINT_OUT(pBuffer)  GNSS_PRINT(pBuffer)
#define PUTC_OUT(pChar)     GNSS_PUTC(pChar)

/* Exported functions prototypes ---------------------------------------------*/
int GNSS_PRINT(char *pBuffer);
int GNSS_PUTC(char pChar);

#ifdef __cplusplus
}
#endif

#endif /* TESEO_LIV3F_CONF_H*/

