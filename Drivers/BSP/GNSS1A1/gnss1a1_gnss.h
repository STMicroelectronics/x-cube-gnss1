/**
  ******************************************************************************
  * @file    gnss1a1_gnss.h
  * @author  SRA
  * @brief   Header file for gnss1a1_gnss.c
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
#ifndef GNSS1A1_GNSS_H
#define GNSS1A1_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gnss1a1_conf.h"

#ifndef USE_GNSS1A1_GNSS_TESEO_LIV3F
#define USE_GNSS1A1_GNSS_TESEO_LIV3F    1U
#endif

#if (USE_GNSS1A1_GNSS_TESEO_LIV3F == 1)
#include "teseo_liv3f.h"
#endif

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup GNSS1A1_GNSS GNSS1A1 GNSS
 * @{
 */

/** @defgroup GNSS1A1_GNSS_Exported_Types GNSS1A1 GNSS Exported Types
 * @{
 */

/**
 * @brief GNSS Message
 */
typedef struct
{
  uint8_t *buf;
  uint16_t len;
} GNSS1A1_GNSS_Msg_t;

/**
 * @brief GNSS Capabilities
 */
typedef struct
{
  uint8_t   Geofence;
  uint8_t   Datalog;
  uint8_t   Odemeter;
  uint8_t   AssistedGNSS;
} GNSS1A1_GNSS_Capabilities_t;

/**
 * @}
 */

/** @defgroup GNSS1A1_GNSS_Exported_Constants GNSS1A1 GNSS Exported Constants
 * @{
 */
#if (USE_GNSS1A1_GNSS_TESEO_LIV3F == 1)
#define GNSS1A1_TESEO_LIV3F 0
#endif

#define GNSS1A1_GNSS_INSTANCES_NBR      (USE_GNSS1A1_GNSS_TESEO_LIV3F)

#if (GNSS1A1_GNSS_INSTANCES_NBR == 0)
#error "No gnss instance has been selected"
#endif

/**
 * @}
 */

/** @addtogroup GNSS1A1_GNSS_Exported_Functions GNSS1A1 GNSS Exported Functions
 * @{
 */

/**
 * @brief  Initializes GNSS
 * @param  Instance GNSS instance
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_Init(uint32_t Instance);

/**
 * @brief  Deinitialize GNSS
 * @param  Instance GNSS instance
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_DeInit(uint32_t Instance);

/**
 * @brief  Start (or resume after a given timeout) communication via I2C.
 * @param  Instance GNSS instance
 * @retval none
 */
void    GNSS1A1_GNSS_BackgroundProcess(uint32_t Instance);

/**
 * @brief  Get the buffer containing a message from GNSS
 * @param  Instance GNSS instance
 * @retval The message buffer
 */
const   GNSS1A1_GNSS_Msg_t* GNSS1A1_GNSS_GetMessage(uint32_t Instance);

/**
 * @brief  Release the NMEA message buffer
 * @param  Instance GNSS instance
 * @param  Message The message buffer
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_ReleaseMessage(uint32_t Instance, const GNSS1A1_GNSS_Msg_t *Message);

/**
 * @brief  Send a command to the GNSS
 * @param  Instance GNSS instance
 * @param  Message The message buffer
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_Send(uint32_t Instance, const GNSS1A1_GNSS_Msg_t *Message);

/**
 * @brief  Get the wake-up status
 * @param  Instance GNSS instance
 * @param  Message The message buffer
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_Wakeup_Status(uint32_t Instance, uint8_t *status);

/**
 * @brief  Reset the GNSS and reinit the FSM
 * @param  Instance GNSS instance
 * @param  Message The message buffer
 * @retval BSP status
 */
int32_t GNSS1A1_GNSS_Reset(uint32_t Instance);

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

#endif /* GNSS1A1_GNSS_H */

