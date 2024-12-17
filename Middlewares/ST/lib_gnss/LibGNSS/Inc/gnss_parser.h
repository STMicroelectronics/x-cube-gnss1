/**
  ******************************************************************************
  * @file    gnss_parser.h
  * @author  SRA Application Team
  * @brief   Header file for gnss_parser.c
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
#ifndef GNSS_PARSER_H
#define GNSS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "NMEA_parser.h"
#include "gnss_geofence.h"
#include "gnss_datalog.h"
  
/** @addtogroup MIDDLEWARES
 *  @{
 */

/** @addtogroup ST
 *  @{
 */

/** @addtogroup LIB_GNSS
 *  @{
 */
 
/** @defgroup LibGNSS LibGNSS
 *  @brief Module handling GNSS parsed data and advanced features.
 *  @{
 */

/** @defgroup LibGNSS_CONSTANTS_DEFINITIONS CONSTANTS DEFINITIONS
 * @{
 */

/**
 * @brief Constant that indicates the maximum number of positions that can be stored.
 */
#define MAX_STOR_POS 64
  
/**
 * @brief Constant that indicates the lenght of the buffer that stores the GPS data read by the GPS expansion.
 */
#define MAX_LEN 2*512
  
 
/**
 * @brief Constant the indicates the max duration of the data reading by the GPS expansion.
 */
#define MAX_DURATION  5000
  
/**
 * @brief Constant the indicates the ack value from the TeseoIII device during the FW upgrade process
 */ 
#define FWUPG_DEVICE_ACK   0xCCU

/**
 * @brief Constant that indicates the maximum number of nmea messages to be processed.
 */
#define NMEA_MSGS_NUM 14 //Note: update this constant coherently to eMsg enum type

/**
 * @}
 */

/** @defgroup LibGNSS_EXPORTED_TYPES EXPORTED TYPES
 * @{
 */

/**
 * @brief Enumeration structure that contains the two states of a parsing dispatcher process
 */
typedef enum
{
  GNSS_PARSER_OK = 0,
  GNSS_PARSER_ERROR
} GNSSParser_Status_t;

/**
 * @brief Enumeration structure that contains the two states of a debug process
 */
typedef enum {
  DEBUG_OFF = 0, /**< In this case, nothing will be printed on the console (nmea strings, positions and so on) */
  DEBUG_ON = 1   /**< In this case, nmea strings and just acquired positions will be printed on the console */
} Debug_State;

/**
 * @brief Enumeration structure that contains the NMEA messages types
 */
typedef enum
{
  GPGGA,
  GNS,
  GPGST,
  GPRMC,
  GSA,
  GSV,
  PSTMVER,
  PSTMPASSRTN,
  PSTMAGPSSTATUS,
  PSTMGEOFENCE,
  PSTMODO,
  PSTMDATALOG,
  PSTMSGL,
  PSTMSAVEPAR
} eNMEAMsg;

/**
 * @brief Data structure that contains the GNSS data
 */
typedef struct
{
  Debug_State debug;      /**< Debug status */
  GPGGA_Info_t gpgga_data; /**< $GPGGA Data holder */
  GNS_Info_t   gns_data;   /**< $--GNS Data holder */
  GPGST_Info_t gpgst_data; /**< $GPGST Data holder */
  GPRMC_Info_t gprmc_data; /**< $GPRMC Data holder */
  GSA_Info_t   gsa_data;   /**< $--GSA Data holder */
  GSV_Info_t   gsv_data;   /**< $--GSV Data holder */

  PSTMVER_Info_t pstmver_data;   /**< $PSTMVER Data holder */
  PSTMPASSRTN_Info_t pstmpass_data; /**< $PSTMPASSRTN Data holder */
  PSTMAGPS_Info_t pstmagps_data; /**< $PSTMAGPS Data holder */

  Geofence_Info_t geofence_data; /**< $PSTMGEOFENCE Data holder */
  Odometer_Info_t odo_data; /**< $PSTMODO Data holder */
  Datalog_Info_t datalog_data; /**< $PSTMDATALOG Data holder */
  OpResult_t result;
} GNSSParser_Data_t;

/**
 * @}
 */

/** @defgroup LibGNSS_EXPORTED_FUNCTIONS EXPORTED FUNCTIONS
 * @{
 */

/**
 * @brief Assigns memory in heap based on the different NMEA and ST proprietary 
 *        messages that are sent from the device.
 * 
 * @param pGNSSParser_Data Pointer to data structure containing NMEA and 
 *                         ST Proprietary message.
 * 
 * @return GNSSParser_Status_t If successful, the module returns 0 (GNSS_PARSER_OK). 
 *                             A return of non-zero value indicates an error.
 * 
 * @pre None.
 * @remarks This function must be called before any data is sent to the Teseo module.
 */
GNSSParser_Status_t GNSS_PARSER_Init(GNSSParser_Data_t *pGNSSParser_Data);

/**
 * @brief Computes the checksum of the received string and compares it with the checksum 
 *        received in the NMEA stream.
 * 
 * @param pSentence Address of NMEA message sentence retrieved from the Teseo object queue.
 * @param len Length of the NMEA message.
 * 
 * @return GNSSParser_Status_t If successful, the module returns 0 (GNSS_PARSER_OK). 
 *                             A return of non-zero value indicates an error.
 * 
 * @pre None.
 * @remarks GNSS1A1_GNSS_GetMessage() must be called before calling this function.
 */
GNSSParser_Status_t GNSS_PARSER_CheckSanity(uint8_t *pSentence, uint64_t len);

/**
 * @brief The NMEA ASCII stream received from the GNSS module is parsed into this 
 *        function and converted to integer and character values that can be used 
 *        by the application as needed.
 * 
 * The msg parameter, which is an index, identifies the specific NMEA message
 * that will be parsed into the specific parser function for the message.
 * 
 * @param pGNSSParser_Data Pointer to data structure containing NMEA and 
 *                         ST Proprietary message. The result of the NMEA data parsing will be stored in 
 *                         this data structure.
 * @param msg This is a search index that will index through a predefined NMEA list
 *            identifying the specific data structure to parse the data.
 * @param pBuffer Incoming NMEA message.
 * 
 * @return GNSSParser_Status_t If the NMEA message was able to be parsed into an existing 
 *                             data structure, the GNSS_PARSE_OK is returned. A non-zero value indicates an error.
 * 
 * @remarks GNSS1A1_GNSS_GetMessage() must be called before calling this function.
 *          To ascertain that the incoming NMEA buffer is parsed to an appropriate 
 *          data structure, GNSS_PARSER_ParseMsg() can be called within a loop for 
 *          NMEA_MSGS_NUM (14) times.
 */
GNSSParser_Status_t GNSS_PARSER_ParseMsg(GNSSParser_Data_t *pGNSSParser_Data, uint8_t msg, uint8_t *pBuffer);

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

#endif /* GNSS_PARSER_H */

