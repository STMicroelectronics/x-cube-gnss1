/**
  *******************************************************************************
  * @file    gnss_data.h
  * @author  SRA Application Team
  * @brief   Header file for gnss_data.h
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
#ifndef GNSS_DATA_H
#define GNSS_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gnss_parser.h"

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

/**
 * @brief  This function sends a command to the GNSS module.
 * @param  pCommand  The string with NMEA command to be sent to the GNSS module
 * @retval None
 */
void GNSS_DATA_SendCommand(uint8_t *pCommand);

/**
 * @brief  Function that retrieves data from correctly parsed GPGGA sentence.
 * @param  pGNSSParser_Data The parsed GPGGA sentence
 * @retval None
 */
void GNSS_DATA_GetValidInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function retrieves the tracking data using the result of parsed GPGGA sentence.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @param  how_many         The number of the position to track
 * @param  time             Time delay between the tracking of two different positions
 * @retval how_many number of actual acquired positions if the process went good, 0 if didn't
 */
int32_t GNSS_DATA_TrackGotPos(GNSSParser_Data_t *pGNSSParser_Data, uint32_t how_many, uint32_t time);

/**	
 * @brief  This function prints on the console all the position got by a tracking position process.
 * @param  None
 * @retval None
 */
void GNSS_DATA_PrintTrackedPositions(uint32_t how_many);

/**	
 * @brief  This function prints on the console the info about Fix data for single 
 *         or combined satellite navigation system.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGNSInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the info about GPS Pseudorange.
 *         Noise Statistics.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGPGSTInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the info about Recommended Minimum Specific
 *         GPS/Transit data got by the most recent reception process.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGPRMCInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the info about GSA satellites got 
 *         by the most recent reception process.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGSAInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the info about GSV satellites got
 *         by the most recent reception process.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGSVInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the info about FW version.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetPSTMVerInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**	
 * @brief  This function prints on the console the geofence infos each time an alarm is received
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGeofenceInfo(GNSSParser_Data_t *pGNSSParser_Data);

/** 
 * @brief  This function prints on the console the info about Odometer.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetOdometerInfo(const GNSSParser_Data_t *pGNSSParser_Data);

/** 
 * @brief  This function prints on the console the info about Datalog.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetDatalogInfo(const GNSSParser_Data_t *pGNSSParser_Data);

/**
 * @brief  This function gets the ACK from the GPS data.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetMsglistAck(const GNSSParser_Data_t *pGNSSParser_Data);

/**
 * @brief  This function gets the ACK from the GNSS data.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetGNSSAck(const GNSSParser_Data_t *pGNSSParser_Data);

/**
 * @brief  This function configures the message list.
 * @param  lowMask This LOW_BITS Mask for configuring message list
 * @param  highMask This HIGH_BITS Mask for configuring message list
 * @retval None
 */
void GNSS_DATA_CfgMessageList(int lowMask, int highMask);

/**
 * @brief  This function sends a command to enable/disable geofence.
 * @param  toggle  The toggle to enable/disable geofence
 * @retval None
 */
void GNSS_DATA_EnableGeofence(int toggle);

/**	
 * @brief  This function sends a command to config a geofence region
 * @param  gnss_geofence The geofence region to be configured
 * @retval None
 */
void GNSS_DATA_ConfigGeofence(void* gnss_geofence);

/**	
 * @brief  This function sends a command to enable/disable odometer.
 * @param  toggle The toggle to enable/disable odometer
 * @retval None
 */
void GNSS_DATA_EnableOdo(int toggle);

/**
 * @brief  This function sends a command to start odometer.
 * @param  alarmDistance The distance to raise an alarm
 * @retval None
 */
void GNSS_DATA_StartOdo(unsigned alarmDistance);

/**
 * @brief  This function sends a command to stop odometer.
 * @retval None
 */
void GNSS_DATA_StopOdo(void);

/**	
 * @brief  This function sends a command to enable/disable datalogging.
 * @param  toggle The toggle to enable/disable datalogging
 * @retval None
 */
void GNSS_DATA_EnableDatalog(int toggle);

/**	
 * @brief  This function sends a command to config datalogging.
 * @param  gnss_datalog The datalog configuration
 * @retval None
 */
void GNSS_DATA_ConfigDatalog(void *gnss_datalog);

/**	
 * @brief  This function sends a command to start datalogging.
 * @retval None
 */
void GNSS_DATA_StartDatalog(void);

/**	
 * @brief  This function sends a command to stop datalogging.
 * @retval None
 */
void GNSS_DATA_StopDatalog(void);

/**
 * @brief  This function sends a command to erase datalogging.
 * @retval None
 */
void GNSS_DATA_EraseDatalog(void);

/**
 * @brief  This function requests the generation of a password to access the A-GPS server.
 * @param  currentTime GPS time
 * @retval None
 */
void GNSS_DATA_PassGen(uint32_t currentTime);

/**
 * @brief  This function prints on the console the info about DeviceId and Pwd.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetPSTMPassInfo(GNSSParser_Data_t *pGNSSParser_Data);

/**
 * @brief  This function prints on the console the STAGPS related info.
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @retval None
 */
void GNSS_DATA_GetPSTMAGPSInfo(const GNSSParser_Data_t *pGNSSParser_Data);

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

#endif /* GNSS_DATA_H */

