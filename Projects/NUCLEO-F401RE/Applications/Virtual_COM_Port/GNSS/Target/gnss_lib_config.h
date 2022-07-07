/**
  ******************************************************************************
  * @file    gnss_lib_config.h
  * @author  SRA
  * @brief   Header file for gnss_lib_config.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GNSS_LIB_CONFIG_H
#define GNSS_LIB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "teseo_liv3f_conf.h"
#if (USE_FREE_RTOS_NATIVE_API) /* native FreeRTOS API */
  #include "FreeRTOS.h"
  #include "task.h"
  #define OS_Delay(a) vTaskDelay(a)
#else
#if (USE_AZRTOS_NATIVE_API) /* native AZRTOS API */
  #include "tx_api.h"
  #define OS_Delay(a) tx_thread_sleep(a)
#else
#if (USE_osCMSIS) /* CMSIS RTOS wrapper API */
  #include "cmsis_os.h"
  #define OS_Delay osDelay
#endif /* USE_osCMSIS */
#endif /* USE_AZRTOS_NATIVE_API */
#endif /* USE_FREE_RTOS_NATIVE_API */

/** @addtogroup MIDDLEWARES
 *  @{
 */

/** @addtogroup ST
 *  @{
 */

/** @addtogroup LIB_GNSS
 *  @{
 */

/** @addtogroup LibGNSS
 *  @{
 */

/** @defgroup GNSS_DATA_FUNCTIONS GNSS DATA FUNCTIONS
 *  @brief Prototypes of the API allowing the application to interface the driver
 *  and interact with GNSS module (sending commands, retrieving parsed NMEA info, etc.).
 *  The implementation is up to the application according to specific needs.
 *  @{
 */

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief  This function puts a string on the console (via UART).
 * @param  pBuffer The string that contains the data to be written on the console
 * @retval None
 */
int32_t GNSS_Wrapper_Send(uint8_t *buffer, uint16_t length);
int32_t GNSS_Wrapper_Reset(void);
void    GNSS_Wrapper_Delay(uint32_t Delay);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* GNSS_LIB_CONFIG_H */

