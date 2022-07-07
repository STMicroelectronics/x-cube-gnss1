/**
  *******************************************************************************
  * @file    gnss_datalog.h
  * @author  SRA Application Team
  * @brief   Define structure for Datalog data
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
#ifndef GNSS_DATALOG_H
#define GNSS_DATALOG_H

#ifdef __cplusplus
extern "C" {
#endif

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

/** @defgroup GNSSDatalog_EXPORTED_TYPES GNSSDatalog EXPORTED TYPES
 *  @brief Types related to Datalog
 *  @{
 */

/**
 * @brief Data structure that contains the Datalog data
 */ 
typedef struct
{      
  uint8_t   enableBufferFullAlarm;
  uint8_t   enableCircularBuffer;
  unsigned  minRate;
  unsigned  minSpeed;
  unsigned  minPosition;
  int       logMask;  
} GNSSDatalog_t;
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

#endif /* GNSS_DATALOG_H */

