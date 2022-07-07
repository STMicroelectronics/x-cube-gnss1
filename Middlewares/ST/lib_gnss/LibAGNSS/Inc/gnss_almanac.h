/**
  ******************************************************************************
  * @file    gnss_almanac.h
  * @author  SRA
  * @brief   Header file for gnss_almanac.c
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
#ifndef GNSS_ALMANAC_H
#define GNSS_ALMANAC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "assisted_gnss.h"

/** @addtogroup MIDDLEWARES
 *  @{
 */

/** @addtogroup ST
 *  @{
 */

/** @addtogroup LIB_GNSS
 *  @{
 */
 
/** @addtogroup LibAGNSS
 *  @{
 */

/** @defgroup GNSS_ALMANAC_EXPORTED_TYPES GNSS ALMANAC EXPORTED TYPES
 *  @brief Types related to Almanac
 *  @{
 */

/**
 * @brief gps_almanac_raw_tag
 */
typedef struct gps_almanac_raw_tag
{
  UINT4 satid           : 8;
  UINT4 week            : 16;
  UINT4 toa             : 8;
  UINT4 eccentricity    : 16;
  UINT4 delta_i         : 16;
  UINT4 omega_dot       : 16;
  UINT4 spare0          : 16;
  UINT4 root_a          : 24;
  UINT4 spare1          : 8;
  UINT4 omega_zero      : 24;
  UINT4 spare2          : 8;
  UINT4 perigee         : 24;
  UINT4 spare3          : 8;
  UINT4 mean_anomaly    : 24;
  UINT4 spare4          : 8;
  UINT4 af0             : 11;
  UINT4 af1             : 11;
  UINT4 health          : 1;
  UINT4 available       : 1;
  UINT4 spare5          : 8;
  UINT4 spare6          : 32;
  UINT4 spare7          : 32;
} gps_almanac_raw_t;

/**
 * @brief glonass_almanac_raw_tag
 */
typedef struct glonass_almanac_raw_tag
{
  UINT4 satid           : 8;
  UINT4 week            : 16;
  UINT4 spare0          : 8;
  UINT4 toa             : 20; 
  UINT4 n_A             : 5;  // orbital slot
  UINT4 H_n_A           : 5;  // channel
  UINT4 M_n_A           : 2;  // 00=GLONASS 01=GLONASS-M
  INT4 tau_n_A          : 10;
  UINT4 epsilon_n_A     : 15;
  UINT4 spare1          : 7;
  UINT4 t_lambda_n_A    : 21;
  UINT4 spare2          : 11;
  INT4 lambda_n_A       : 21;
  UINT4 spare3          : 11;
  INT4 delta_i_n_A      : 18;
  INT4 delta_T_n_dot_A  : 7;
  UINT4 spare4          : 7;
  INT4 delta_T_n_A      : 22;
  UINT4 spare5          : 10;
  INT4 omega_n_A        : 16;
  UINT4 health          : 1;
  UINT4 available       : 1;
  UINT4 spare6          : 14;
  UINT4 tau_c           : 32; //not present in RT-Data provided by RXN server
  UINT4 NA              : 11;
  UINT4 N4              : 5;
  UINT4 spare7          : 16;
} glonass_almanac_raw_t;

/**
 * @brief beidou_almanac_raw_tag
 */
typedef struct compass_almanac_raw_tag
{
  UINT4 prn           : 8;
  UINT4 week          : 16;
  UINT4 toa           : 8;

  UINT4 eccentricity  : 17;
  UINT4 af0           : 11;
  UINT4 is_geo        : 1;
  UINT4 spare0        : 3;
  
  UINT4 omega_dot     : 17;
  UINT4 af1           : 11;
  UINT4 spare1        : 4;
  
  UINT4 root_a        : 24;
  UINT4 spare2        : 8;
  
  UINT4 omega_zero    : 24;
  UINT4 spare3        : 8;
  
  UINT4 perigee       : 24;
  UINT4 spare4        : 8;
  
  UINT4 mean_anomaly  : 24;
  UINT4 spare5        : 8;
  
  UINT4 delta_i       : 16;
  UINT4 health        : 1;
  UINT4 available     : 1;
  UINT4 spare6        : 14;

} compass_almanac_raw_t;

/**
 * @brief galileo_almanac_raw_tag
 */
typedef struct galileo_almanac_raw_tag
{
  UINT4 satid : 16;
  UINT4 svid : 6;
  UINT4 spare0 : 10;
  UINT4 week : 16;
  UINT4 spare1 : 16;
  UINT4 toa : 20;
  UINT4 spare2 : 12;
  UINT4 delta_a : 13;
  UINT4 eccentricity : 11;
  UINT4 spare3 : 8;
  UINT4 perigee : 16;
  UINT4 delta_i : 11;
  UINT4 spare4 : 5;
  UINT4 omega_zero : 16;
  UINT4 omega_dot : 11;
  UINT4 spare5 : 5;
  UINT4 mean_anomaly : 16;
  UINT4 af0 : 16;
  UINT4 af1 : 13;
  UINT4 E5b_HS : 2;
  UINT4 E1B_HS : 2;
  UINT4 ioda_1 : 4;
  UINT4 ioda_2 : 4;
  UINT4 word_available : 2;
  UINT4 health : 1;
  UINT4 available : 1;
  UINT4 spare6 : 2;
  UINT4 spare7 : 32;
  UINT4 spare8 : 32;
}galileo_almanac_raw_t;

/**
 * @brief almanac_raw_tag
 */
typedef union almanac_raw_tag
{
  gps_almanac_raw_t       gps;
  glonass_almanac_raw_t   glo;
  compass_almanac_raw_t   compass;
  galileo_almanac_raw_t   galileo;
} almanac_raw_t;

/**
 * @brief GPS almanac header
 */
typedef struct assist_almgps_hdr_s 
{
  BYTE  num_sats;
  UINT2 week_n;
} assist_almgps_hdr_t;

/**
 * @brief GLO almanac header
 */
typedef struct assist_almglo_hdr_s 
{
  BYTE  num_sats;
  BYTE  week_n;
  BYTE  sToa;
  BYTE  ioda;
} assist_almglo_hdr_t;

/**
 * @brief GAL almanac header
 */
typedef struct assist_almgal_hdr_s 
{
  BYTE  num_sats;
  UINT4 sToa;
  BYTE  ioda;
} assist_almgal_hdr_t;

/**
 * @brief BEI almanac header
 */
typedef struct assist_almbei_hdr_s 
{
  BYTE  num_sats;
  BYTE  week_n;
  BYTE  sToa;
  BYTE  ioda;
} assist_almbei_hdr_t;

/**
 * @}
 */
 
/** @defgroup GNSS_ALAMNAC_FUNCTIONS GNSS EPHEMERIS FUNCTIONS
 *  @brief Prototypes of the API for RT Assisted GNSS support.
 *  @{
 */

/**
 * @brief  This function decodes ALM seed.
 * @param  seed_type Constellation
 * @param  seed_info Downloaded seed info
 * @param  week Week number
 * @param  cur_time current UTC time
 * @retval The week number
 */
uint16_t A_GNSS_DecodeAlmRT(seed_type_t seed_type,
                            seed_info_t *seed_info,
                            uint16_t week,
                            current_timedate_t *utc_time);

/**
 * @brief  This function sends ALM seed.
 * @param  seed_type Constellation
 * @param  seed_info Downloaded seed info
 * @retval None
 */
void A_GNSS_SendAlmRT(seed_type_t seed_type,
                      seed_info_t *seed_info);
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

#endif /* GNSS_ALMANAC_H */

