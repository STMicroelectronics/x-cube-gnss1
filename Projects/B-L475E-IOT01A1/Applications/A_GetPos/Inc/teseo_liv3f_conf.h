/**
  ******************************************************************************
  * @file    teseo_liv3f_conf.h
  * @author  SRA
  * @brief   This file contains definitions for the TESEO LIV3F Component settings
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
#endif /* __weak */
#endif /* __GNUC__ */

/* Defines -------------------------------------------------------------------*/
//#define USE_FREE_RTOS /* defined in the preprocessor options */

#define GNSS_DEBUG 0 /* Debug off */

#define CONFIG_USE_FEATURE 0 /* Feature off */

#define CONFIG_USE_ODOMETER 0 /* Odometer off */

#define CONFIG_USE_GEOFENCE 0 /* Geofence off */

#define CONFIG_USE_DATALOG 0 /* Datalog off */

#if (GNSS_DEBUG == 1)
#define PRINT_DBG(pBuffer)  GNSS_PRINT(pBuffer)
#else
#define PRINT_DBG(pBuffer)
#endif /* GNSS_DEBUG */

#define PRINT_INFO(pBuffer) GNSS_PRINT(pBuffer)
#define PRINT_OUT(pBuffer)  GNSS_PRINT(pBuffer)
#define PUTC_OUT(pChar)     GNSS_PUTC(pChar)

/* Exported functions prototypes ---------------------------------------------*/
uint8_t GNSS_PRINT(char *pBuffer);
uint8_t GNSS_PUTC(char pChar);

#ifdef __cplusplus
}
#endif

#endif /* TESEO_LIV3F_CONF_H*/

