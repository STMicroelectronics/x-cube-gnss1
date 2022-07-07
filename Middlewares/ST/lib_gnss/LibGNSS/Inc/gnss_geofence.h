/**
  *******************************************************************************
  * @file    gnss_geofence.h
  * @author  SRA Application Team
  * @brief   Define structure for Geofence data
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
#ifndef GNSS_GEOFENCE_H
#define GNSS_GEOFENCE_H

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

/** @defgroup GNSSGeofence_EXPORTED_TYPES GNSSGeofence EXPORTED TYPES
 *  @brief Types related to Geofence
 *  @{
 */

/**
 * @brief Data structure that contains the Geofence data
 */
typedef struct
{
  int id;
  uint8_t enabled;
  int tolerance;
  double lat;
  double lon;
  double radius;
  int status;
} GNSSGeofence_t;
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

#endif /* GNSS_GEOFENCE_H */

