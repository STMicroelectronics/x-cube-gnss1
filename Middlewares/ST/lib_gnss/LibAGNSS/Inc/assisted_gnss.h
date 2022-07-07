/**
  ******************************************************************************
  * @file    assisted_gnss.h
  * @author  SRA
  * @brief   Header file for assisted_gnss.c
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
#ifndef ASSISTED_GNSS_H
#define ASSISTED_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gnss_parser.h"
#include "gnss_lib_config.h"

#define AGNSS_DEBUG
#ifdef AGNSS_DEBUG
#define AGNSS_PRINT_OUT(...) (void)printf(__VA_ARGS__)
#else
#define AGNSS_PRINT_OUT(...)
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
 
/** @addtogroup LibAGNSS
 *  @{
 */


/** @defgroup ASSISTED_GNSS_CONSTANTS_DEFINITIONS AGNSS CONSTANTS DEFINITIONS
 *  @brief Constants related to Assisted GNSS
 *  @{
 */

/* Defines -------------------------------------------------------------------*/
#define AGNSS_OK        0
#define AGNSS_ERR       -1

#define BUF_SIZE_1K     (1024)
#define BUF_SIZE_2K     (2*1024)
#define BUF_SIZE_3K     (3*1024)
#define BUF_SIZE_4K     (4*1024)
#define BUF_SIZE_5K     (5*1024)
#define BUF_SIZE_6K     (6*1024)
#define BUF_SIZE_8K     (8*1024)
#define BUF_SIZE_9K     (9*1024)
#define BUF_SIZE_12K    (12*1024)
#define BUF_SIZE_15K    (15*1024)
#define BUF_SIZE_16K    (16*1024)
#define BUF_SIZE_18K    (18*1024)

#define TX_TIMEOUT (5000)
#define TX_DELAY (500)

#define GPS_CONSTELLATION (0x01U)
#define GLO_CONSTELLATION (0x02U)
#define GAL_CONSTELLATION (0x04U)
#define BEI_CONSTELLATION (0x08U)
#define DEFAULT_CONSTELLATION \
   (GPS_CONSTELLATION | \
     GLO_CONSTELLATION | \
       GAL_CONSTELLATION)

#define MAX_BLOCK_TYPE_SIZE     (128U)
#define MAX_SLOT_FREQ_SIZE      (128U)
#define MAX_TIME_MODEL_SIZE     (128U)

#define SINGLE_SEED_BIT_LEN                (1240U)
#define SINGLE_SEED_FIRST_BLOCK_BIT_LEN    (171U)

#define MAX_NUM_GPS_PRNS        (32U)
#define MAX_NUM_GLO_PRNS        (24U)
#define MAX_NUM_GAL_PRNS        (30U)
#define MAX_NUM_BEI_PRNS        (37U)
#define MIN_GLO_PRN             (65U)
#define MIN_GAL_PRN             (301U)

#define MIN_GLONASS_SAT_ID      (65U)
#define MAX_GLONASS_SAT_ID      (92U)

#define MIN_GALILEO_SAT_ID      (301U)
#define MAX_GALILEO_SAT_ID      (330U)

#define SECS_PER_MIN            (60U)
#define SECS_PER_HOUR           (60U * SECS_PER_MIN)
#define SECS_PER_DAY            (24U * SECS_PER_HOUR)
#define SECS_PER_WEEK           (7U * 24U * SECS_PER_HOUR)

#define GPS_WEEK_ROLLOVER       (2048U)
#define MAX_GLO_SLOTS           (24U)
#define COMPASS_INIT_WEEK       (1356U)
#define MIN_COMPASS_SAT_ID      (141U)

#define MAX_VENDOR_ID           20
#define MAX_MODEL_ID            8

/* Exported variables --------------------------------------------------------*/

extern const uint8_t vendorId[MAX_VENDOR_ID];
extern const uint8_t modelId[MAX_MODEL_ID];

extern uint8_t prn2slot[MAX_NUM_GLO_PRNS];
extern uint32_t N4;

/** @defgroup ASSISTED_GNSS_EXPORTED_TYPES AGNSS EXPORTED TYPES
 *  @brief Types related to Assisted GNSS
 *  @{
 */

/* Exported types ------------------------------------------------------------*/

typedef uint8_t BYTE;
typedef int16_t INT2;
typedef int32_t INT4;
typedef uint16_t UINT2;
typedef uint32_t UINT4;

/**
 * @brief Current time date
 */
typedef struct
{
  uint32_t Day;
  uint32_t Month;
  uint32_t Year;
  int8_t Hours;
  int8_t Minutes;
  int8_t Seconds;
} current_timedate_t;

/**
 * @brief Seed request type (Predictive/Real-time)
 */
typedef enum
{
  PR_SEED,
  RT_SEED,
} seed_req_t;

/**
 * @brief Seed type
 */
typedef enum
{
  GPS_SEED = 1,
  GLO_SEED,
  GAL_SEED,
  BEI_SEED,
} seed_type_t;

/**
 * @brief Data structure that contains all of the information about Seed
 */
typedef struct
{
  uint8_t *jsonBuffer;

  // Predictive section
  int32_t curr_secs;
  int32_t next_gps_time;
  int32_t next_secs;
  uint8_t *seed; /* Seed pointer for Predictive GNSS */
  size_t seed_size;
  uint8_t block_types[MAX_BLOCK_TYPE_SIZE];
  uint8_t slot_freq[MAX_SLOT_FREQ_SIZE];
  uint8_t time_model[MAX_TIME_MODEL_SIZE];
  uint16_t nsat;
  uint16_t max_satid;

  // Real time section
  uint8_t *eph_seed; /* Seed pointer for Real-Time GNSS - EPH */
  size_t eph_seed_size;
  uint8_t *eph_nkc_seed; /* Seed pointer for Real-Time GNSS - EPH (GLO) */
  size_t eph_nkc_seed_size;
  void *stm_ephem;

  uint8_t *alm_seed; /* Seed pointer for Real-Time GNSS - ALM */
  size_t alm_seed_size;
  void *stm_alm;

  uint32_t num_sat_eph;
  uint32_t num_sat_alm;
  uint8_t sat_id[64];

} seed_info_t;

/**
 * @brief Data structure that contains the information about GLONASS Slot Frequency
 */
typedef struct
{
  uint8_t SV_ID;  //slot_num;
  int8_t  ch_num; //freq_num;
  uint8_t signal_available;
} glo_slot_freq_t;

/**
 * @brief Data structure that contains the information about Block Type
 */
typedef struct
{
  uint8_t SV_ID;
  int8_t  block_type;
} block_type_t;

/**
 * @}
 */

/** @defgroup ASSISTED_GNSS_FUNCTIONS ASSISTED GNSS FUNCTIONS
 *  @brief Prototypes of the API for Assisted GNSS support.
 *  @{
 */

/**
 * @brief  Utility for Ephemeris and Almanac.
 * @param  val
 * @param  width
 * @retval Value converted
 */
int32_t sign_extend_1c(const uint32_t val, const uint32_t width);

/**
 * @brief  This function calculates number day of year.
 * @param  utc_time current UTC time
 * @retval Day number
 */
uint32_t day_of_year(current_timedate_t *utc_time);

/**
 * @brief  This function converts timedate structure to GPS time.
 * @param  cur_time current UTC time
 * @retval GPS time
 */
uint32_t UTC_ToGPS(current_timedate_t *utc_time);

/**
 * @brief  This function extracts a byte from a byte array at specified position,
 *         where the position is expressed in bit.
 * @param  byte The byte extracted
 * @param  buffer The byte array
 * @param  pos Position within the buffer
 * @retval None
 */
void extract_byte(uint8_t *byte, uint8_t *buffer, uint32_t pos);

/**
 * @brief  This function extracts a 16-bit word from a byte array at specified position,
 *         where the position is expressed in bit.
 * @param  dword 16-bit word
 * @param  buffer The byte array
 * @param  pos Position within the buffer
 * @retval None
 */
void extract_hword(uint16_t *dword, uint8_t *buffer, uint32_t pos);

/**
 * @brief  This function extracts a 32-bit word from a byte array at specified position,
 *         where the position is expressed in bit.
 * @param  dword 32-bit word
 * @param  buffer The byte array
 * @param  pos Position within the buffer
 * @retval None
 */
void extract_dword(uint32_t *dword, uint8_t *buffer, uint32_t pos);

// Calculates NMEA checksum
/**
 * @brief  This function calculates NMEA checksum.
 * @param  string NMEA string
 * @retval Checksum
 */
uint32_t A_GNSS_NMEA_Checksum(const uint8_t *string);

/**
 * @brief  This function builds the Json request for RXN server.
 * @param  jsonData The string that will contain the request in json format
 * @param  seedMask Constellation mask
 * @param  seedSize Size of the expected seed
 * @param  reqLen Length of the json request
 * @param  seedReq Type of the json request (Predictive/Real-time)
 * @retval AGNSS_OK on success AGNSS_ERR on error
 */
int32_t A_GNSS_BuildJsonReq(uint8_t *jsonData,
                            uint8_t seedMask,
                            int16_t *seedSize,
                            int8_t *reqLen,
                            seed_req_t seedReq);

/**
 * @brief  This function builds the authorization HTTP header for RXN server.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @param  auth_string The string that will contain the auth header
 * @retval AGNSS_OK on success AGNSS_ERR on error
 */
int32_t A_GNSS_BuildAuthHeader(GNSSParser_Data_t *pGNSSParser_Data, uint8_t *auth_string);
 
/**
 * @brief  This function completes decoding and send seed.
 * @param  constellation The seed constellation
 * @param  seed_info Pointer to seed related info
 * @retval None
 */
void A_GNSS_SendSeed(uint8_t constellation,
                     seed_info_t *seed_info);

/**
 * @brief  This function builds and send the inittime command.
 * @param  utc_time UTC time
 * @retval None
 */
void A_GNSS_SendCurrentTime(current_timedate_t *utc_time);

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

#endif /* ASSISTED_GNSS_H */

