/**
  *******************************************************************************
  * @file    gnss_parser.c
  * @author  SRA Application Team
  * @brief   Implements API for NMEA message parsing - LibGNSS module middleware
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "gnss_parser.h"



/* Exported functions --------------------------------------------------------*/

GNSSParser_Status_t GNSS_PARSER_Init(GNSSParser_Data_t *pGNSSParser_Data)
{
  GNSSParser_Status_t ret;

  if (pGNSSParser_Data != NULL)
  {
    pGNSSParser_Data->debug = DEBUG_ON;
    (void)memset((void *)(&pGNSSParser_Data->gpgga_data), 0, sizeof(GPGGA_Info_t));
    pGNSSParser_Data->gpgga_data.xyz.ew = (uint8_t)' ';
    pGNSSParser_Data->gpgga_data.xyz.ns = (uint8_t)' ';
    pGNSSParser_Data->gpgga_data.xyz.mis = (uint8_t)' ';
    
    (void)memset((void *)(&pGNSSParser_Data->gns_data), 0, sizeof(GNS_Info_t));
    pGNSSParser_Data->gns_data.xyz.ew = (uint8_t)' ';
    pGNSSParser_Data->gns_data.xyz.ns = (uint8_t)' ';
    
    (void)memset((void *)(&pGNSSParser_Data->gpgst_data), 0, sizeof(GPGST_Info_t));
    
    (void)memset((void *)(&pGNSSParser_Data->gprmc_data), 0, sizeof(GPRMC_Info_t));
    pGNSSParser_Data->gprmc_data.xyz.ew = (uint8_t)' ';
    pGNSSParser_Data->gprmc_data.xyz.ns = (uint8_t)' ';
    
    (void)memset((void *)(&pGNSSParser_Data->gsa_data), 0, sizeof(GSA_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->gsv_data), 0, sizeof(GSV_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->pstmver_data), 0, sizeof(PSTMVER_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->pstmpass_data), 0, sizeof(PSTMPASSRTN_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->pstmagps_data), 0, sizeof(PSTMAGPS_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->geofence_data), 0, sizeof(Geofence_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->odo_data), 0, sizeof(Odometer_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->datalog_data), 0, sizeof(Datalog_Info_t));
    (void)memset((void *)(&pGNSSParser_Data->result), 0, sizeof(OpResult_t));

    ret = GNSS_PARSER_OK;
  }
  else
  {
    ret = GNSS_PARSER_ERROR;
  }

  return ret;
}

GNSSParser_Status_t GNSS_PARSER_CheckSanity(uint8_t *pSentence, uint64_t len)
{
  GNSSParser_Status_t ret;
  uint32_t checksum, check = 0U;
  
  if((len > 0U) && (len < 5U))
  {
    ret = GNSS_PARSER_ERROR;
  }
  else if(len == 0U)
  {
    ret = GNSS_PARSER_OK;
  }
  else
  {
    checksum = (char2int(pSentence[len-4U]) << 4) | char2int(pSentence[len-3U]);
    
    for(uint64_t c = 1U; c < (len-5U); c++) {
      check = (check ^ pSentence[c]);
    }

    ret = (check == checksum) ? GNSS_PARSER_OK : GNSS_PARSER_ERROR;
  }
    
  return ret;
}

GNSSParser_Status_t GNSS_PARSER_ParseMsg(GNSSParser_Data_t *pGNSSParser_Data, uint8_t msg, uint8_t *pBuffer)
{
  ParseStatus_t status = PARSE_FAIL;

  switch(msg) {
  case GPGGA:
    status = NMEA_ParseGPGGA(&pGNSSParser_Data->gpgga_data, pBuffer);
    break;
  case GNS:
    status = NMEA_ParseGNS(&pGNSSParser_Data->gns_data, pBuffer);
    break;
  case GPGST:
    status = NMEA_ParseGPGST(&pGNSSParser_Data->gpgst_data, pBuffer);
    break;
  case GPRMC:
    status = NMEA_ParseGPRMC(&pGNSSParser_Data->gprmc_data, pBuffer);
    break;
  case GSA:
    status = NMEA_ParseGSA(&pGNSSParser_Data->gsa_data, pBuffer);
    break;
  case GSV:
    status = NMEA_ParseGSV(&pGNSSParser_Data->gsv_data, pBuffer);
    break;
  case PSTMVER:
    status = NMEA_ParsePSTMVER(&pGNSSParser_Data->pstmver_data, pBuffer);
    break;
  case PSTMPASSRTN:
    status = NMEA_ParsePSTMPASSRTN(&pGNSSParser_Data->pstmpass_data, pBuffer);
    break;
  case PSTMAGPSSTATUS:
    status = NMEA_ParsePSTMAGPS(&pGNSSParser_Data->pstmagps_data, pBuffer);
    break;
  case PSTMGEOFENCE:
    status = NMEA_ParsePSTMGeofence(&pGNSSParser_Data->geofence_data, pBuffer);
    break;
  case PSTMODO:
    status = NMEA_ParsePSTMOdo(&pGNSSParser_Data->odo_data, pBuffer);
    break;
  case PSTMDATALOG:
    status = NMEA_ParsePSTMDatalog(&pGNSSParser_Data->datalog_data, pBuffer);
    break;
  case PSTMSGL:
    status = NMEA_ParsePSTMsgl(&pGNSSParser_Data->result, pBuffer);
    break;
  case PSTMSAVEPAR:
    status = NMEA_ParsePSTMSavePar(&pGNSSParser_Data->result, pBuffer);
    break;
  default:
    break;
  }
    
  return ((status == PARSE_FAIL) ? GNSS_PARSER_ERROR : GNSS_PARSER_OK);
}

