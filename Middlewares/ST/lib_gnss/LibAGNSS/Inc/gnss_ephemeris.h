/**
  ******************************************************************************
  * @file    gnss_ephemeris.h
  * @author  SRA
  * @brief   Header file for gnss_ephemeris.c
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
#ifndef GNSS_EPHEMERIS_H
#define GNSS_EPHEMERIS_H

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

/** @defgroup GNSS_EPHEMERIS_EXPORTED_TYPES GNSS EPHEMERIS EXPORTED TYPES
 *  @brief Types related to Ephemeris
 *  @{
 */

/**
 * @brief gps_ephemeris_raw_tag
 */
typedef struct gps_ephemeris_raw_tag
{
  UINT4 week                 : 16;
  UINT4 toe                  : 16;

  UINT4 toc                  : 16;
  UINT4 iode1                : 8;
  UINT4 iode2                : 8;

  UINT4 iodc                 : 10;
  UINT4 i_dot                : 14;
  UINT4 spare1               : 8;

  UINT4 omega_dot            : 24;
  UINT4 ephems_n             : 2;
  UINT4 time_distance_h      : 6;

  UINT4 crs                  : 16;
  UINT4 crc                  : 16;

  UINT4 cus                  : 16;
  UINT4 cuc                  : 16;

  UINT4 cis                  : 16;
  UINT4 cic                  : 16;

  UINT4 motion_difference    : 16;
  UINT4 age_h                : 10;
  UINT4 spare3               : 6;

  UINT4 inclination          : 32;
  UINT4 eccentricity         : 32;
  UINT4 root_a               : 32;
  UINT4 mean_anomaly         : 32;
  UINT4 omega_zero           : 32;
  UINT4 perigee              : 32;

  UINT4 time_group_delay     : 8;
  UINT4 af2                  : 8;
  UINT4 af1                  : 16;

  UINT4 af0                  : 22;
  UINT4 subframe1_available  : 1;
  UINT4 subframe2_available  : 1;
  UINT4 subframe3_available  : 1;
  UINT4 available            : 1;
  UINT4 health               : 1;
  UINT4 predicted            : 1;
  UINT4 accuracy             : 4;

} gps_ephemeris_raw_t;

/**
 * @brief glo_ephemeris_raw_tag
 */
typedef struct glonass_ephemris_raw_tag
{
  UINT4 week                  : 16;
  UINT4 toe                   : 16;

  UINT4 toe_lsb              : 4;
  UINT4 NA                   : 11;
  UINT4 tb                   : 7;
  UINT4 M                    : 2;
  UINT4 P1                   : 2;
  UINT4 P3                   : 1;
  UINT4 P2                   : 1;
  UINT4 P4                   : 1;
  UINT4 KP                   : 2; //not present in RT-Data provided by RXN server
  UINT4 spare0               : 1;

  INT4 xn                    : 27;
  INT4 xn_dot_dot            : 5;
  INT4 xn_dot                : 24;
  UINT4 n                    : 5;
  UINT4 Bn                   : 3; //not present in RT-Data provided by RXN server

  INT4 yn                    : 27;
  INT4 yn_dot_dot            : 5;
  INT4 yn_dot                : 24;
  UINT4 spare4               : 8;

  INT4 zn                    : 27;
  INT4 zn_dot_dot            : 5;
  INT4 zn_dot                : 24;
  UINT4 ephems_n             : 2; //not required for RT-Data 
  UINT4 time_distance_h      : 6; //not required for RT-Data
  INT4 gamma_n               : 11;
  UINT4 E_n                  : 5;
  UINT4 freq_id              : 4; //not present in RT-Data 
  UINT4 spare3               : 12;

  INT4 tau_n                 : 22;
  UINT4 age_h                : 10; //not required for RT-Data 

  UINT4 tau_c                : 32; //not present in RT-Data provided by RXN server

  UINT4 tau_GPS              : 22; //not present in RT-Data provided by RXN server
  UINT4 spare5               : 10;

  UINT4 NT                   : 11;
  UINT4 N4                   : 5;
  UINT4 tk                   : 12; //not present in RT-Data provided by RXN server
  UINT4 FT                   : 4;  //not present in RT-Data provided by RXN server

  UINT4 spare7               : 32;

  UINT4 m_available          : 5;
  UINT4 nvm_reliable         : 1;
  UINT4 spare8               : 26;

  UINT4 spare9               : 25;
  UINT4 available            : 1;
  UINT4 health               : 1; 
  UINT4 predicted            : 1;
  UINT4 spare10              : 4;
} glonass_ephemeris_raw_t;

/**
 * @brief bei_ephemeris_raw_tag
 */
typedef struct compass_ephemeris_raw_tag
{
  UINT4 inclination       : 32;
  UINT4 eccentricity      : 32;
  UINT4 root_a            : 32;
  UINT4 mean_anomaly      : 32;
  UINT4 omega_zero        : 32;
  UINT4 perigee           : 32;

  UINT4 toe               : 17;
  UINT4 time_group_delay  : 10;
  UINT4 aode              : 5;

  UINT4 omega_dot         : 24;
  UINT4 A0                : 8;  //not present in RT-Data provided by RXN server

  UINT4 af0               : 24;
  UINT4 A1                : 8;  //not present in RT-Data provided by RXN server

  UINT4 sow               : 20; //not present in RT-Data provided by RXN server
  UINT4 af2               : 11;
  UINT4 is_geo            : 1;

  UINT4 af1               : 22;
  UINT4 subframe_avail    : 10;

  UINT4 motion_difference : 16;
  UINT4 A2                : 8; //not present in RT-Data provided by RXN server
  UINT4 A3                : 8; //not present in RT-Data provided by RXN server

  UINT4 crs               : 18;
  UINT4 B2                : 8; //not present in RT-Data provided by RXN server
  UINT4 urai              : 4; //not present in RT-Data provided by RXN server
  UINT4 ephems_n          : 2; //not required for RT-Data

  UINT4 crc               : 18;
  UINT4 B3                : 8; //not present in RT-Data provided by RXN server
  UINT4 aodc              : 5;
  UINT4 spare0            : 1;

  UINT4 cus               : 18;
  UINT4 i_dot             : 14;

  UINT4 cuc               : 18;
  UINT4 B0                : 8; //not present in RT-Data provided by RXN server
  UINT4 spare1            : 6;

  UINT4 cis               : 18;
  UINT4 B1                : 8; //not present in RT-Data provided by RXN server
  UINT4 time_distance_h   : 6;

  UINT4 cic               : 18;
  UINT4 nvm_reliable      : 1;
  UINT4 predicted         : 1;
  UINT4 age_h             : 10; //not required for RT-Data
  UINT4 spare4            : 2;

  UINT4 toc               : 17;
  UINT4 week              : 13;
  UINT4 available         : 1;
  UINT4 health            : 1;

} compass_ephemeris_raw_t;

/**
 * @brief gal_ephemeris_raw_tag
 */
typedef struct galileo_ephemeris_raw_tag
{
  UINT4 week : 16;
  UINT4 toe : 14;
  UINT4 reserved1 : 2; // must be 0
  UINT4 toc : 14;
  UINT4 iod_nav : 10;
  UINT4 SISA : 8;
  UINT4 reserved2 : 10; // must be 0
  UINT4 BGD_E1_E5a : 10;
  UINT4 BGD_E1_E5b : 10;
  UINT4 E1BHS : 2;
  UINT4 inclination : 32;
  UINT4 eccentricity : 32;
  UINT4 root_a : 32;
  UINT4 mean_anomaly : 32;
  UINT4 omega_zero : 32;
  UINT4 perigee : 32;
  UINT4 i_dot : 14;
  UINT4 available : 1;
  UINT4 health : 1;
  UINT4 motion_difference : 16;
  UINT4 crs : 16;
  UINT4 crc : 16;
  UINT4 cus : 16;
  UINT4 cuc : 16;
  UINT4 cis : 16;
  UINT4 cic : 16;
  UINT4 omega_dot : 24;
  UINT4 SVID : 6;
  UINT4 E1BDVS : 1;
  UINT4 reserved3 : 1; // must be 0
  UINT4 af2 : 6;
  UINT4 af1 : 21;
  UINT4 word_available : 5;
  UINT4 af0 : 31;
  UINT4 spare0 : 1;
  UINT4 reserved4 : 6; // must be 0
  UINT4 spare1 : 26;

} galileo_ephemeris_raw_t;

/**
 * @brief ephemeris_raw_tag
 */
typedef union ephemris_raw_tag
{
  gps_ephemeris_raw_t       gps;
  glonass_ephemeris_raw_t   glonass;
  compass_ephemeris_raw_t   compass;
  galileo_ephemeris_raw_t   galileo;
} ephemeris_raw_t;

/**
 * @brief ephemeris header
 */
typedef struct assist_hdr_s
{
  BYTE  num_sats;
  BYTE  non_brc_ind;
} assist_hdr_t;

/**
 * @}
 */
 
/** @defgroup GNSS_EPHEMERIS_FUNCTIONS GNSS EPHEMERIS FUNCTIONS
 *  @brief Prototypes of the API for RT Assisted GNSS support.
 *  @{
 */

/**
 * @brief  This function decodes EPH seed.
 * @param  seed_type Constellation
 * @param  seed_info Downloaded seed info
 * @param  cur_time current UTC time
 * @retval The week number
 */
uint16_t A_GNSS_DecodeEphemRT(seed_type_t seed_type,
                              seed_info_t *seed_info,
                              current_timedate_t *utc_time);

/**
 * @brief  This function sends EPH seed.
 * @param  seed_type Constellation
 * @param  seed_info Downloaded seed info
 * @retval None
 */
void A_GNSS_SendEphemRT(seed_type_t seed_type,
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

#endif /* GNSS_EPHEMERIS_H */

