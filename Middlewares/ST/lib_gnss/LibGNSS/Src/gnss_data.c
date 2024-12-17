/**
  *******************************************************************************
  * @file    gnss_data.c
  * @author  SRA Application Team
  * @brief   Implements API to access GNSS data - LibGNSS module middleware
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
#include <math.h>
#include <string.h>

#include "gnss_data.h"
#ifdef ASSISTED_GNSS
#include "assisted_gnss.h"
#endif /* ASSISTED_GNSS */
#include "gnss_geofence.h"
#include "gnss_lib_config.h"

#include "teseo_liv3f_conf.h"

/* Private defines -----------------------------------------------------------*/
#define MSG_SZ (256)
#define CMD_SZ (90)

/* Private variables ---------------------------------------------------------*/

/* Variable that holds the values got by the tracking process */
static GPGGA_Info_t stored_positions[MAX_STOR_POS];
static uint8_t msg[MSG_SZ];
static uint8_t gnssCmd[CMD_SZ];

/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
int16_t minute_part(float64_t mod){
	int16_t minute_whole_part;
	minute_whole_part = (int16_t) mod;
	return minute_whole_part;
}
float64_t seconds(float64_t mod, int16_t minute){
	float64_t seconds;
	seconds = (mod-minute)*60.0;
	return seconds;
}
/* Sends a command to the GNSS module. */
void GNSS_DATA_SendCommand(uint8_t *pCommand)
{
  int32_t status;

  if (pCommand[0] != 36U) /* An NMEA command must begin with '$' */
  {
    PRINT_INFO("Invalid command (an NMEA command begins with '$').\n\r");
  }
  else
  {
    (void)snprintf((char *)gnssCmd, CMD_SZ, "%s\r\n", (char *)pCommand);
    do
    {
      status = GNSS_Wrapper_Send(gnssCmd, (uint16_t)(strlen((char *)gnssCmd)));
    }
    while(status != 0);
  }
}

/* Puts to console data of correctly parsed GPGGA sentence */
void GNSS_DATA_GetValidInfo(GNSSParser_Data_t *pGNSSParser_Data)
{

  if(pGNSSParser_Data->gpgga_data.valid > INVALID)
  {
    float64_t lat_mod = fmod(pGNSSParser_Data->gpgga_data.xyz.lat, 100.0);
    float64_t lon_mod = fmod(pGNSSParser_Data->gpgga_data.xyz.lon, 100.0);

    (void)snprintf((char *)msg, MSG_SZ, "UTC:\t\t\t[ %02d:%02d:%02d ]\n\r",
                   pGNSSParser_Data->gpgga_data.utc.hh,
                   pGNSSParser_Data->gpgga_data.utc.mm,
                   pGNSSParser_Data->gpgga_data.utc.ss);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Latitude:\t\t[ %.0f' %d'' %f\" %c ]\n\r",
                   (pGNSSParser_Data->gpgga_data.xyz.lat - lat_mod) / 100.0,
                   minute_part(lat_mod),
				   seconds(lat_mod,minute_part(lat_mod)),
                   pGNSSParser_Data->gpgga_data.xyz.ns);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Longitude:\t\t[ %.0f' %d'' %f\" %c ]\n\r",
                   (pGNSSParser_Data->gpgga_data.xyz.lon - lon_mod) / 100.0,
				   minute_part(lon_mod),
				   seconds(lon_mod,minute_part(lon_mod)),
                   pGNSSParser_Data->gpgga_data.xyz.ew);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Satellites locked:\t[ %d ]\n\r",
                   pGNSSParser_Data->gpgga_data.sats);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "HDOP:\t\t\t[ %.1f ]\n\r",
                   pGNSSParser_Data->gpgga_data.acc);
    PRINT_INFO((char *)msg);
    

    (void)snprintf((char *)msg, MSG_SZ, "Altitude:\t\t[ %.2f%c ]\n\r",
                   pGNSSParser_Data->gpgga_data.xyz.alt,
                   (pGNSSParser_Data->gpgga_data.xyz.mis + 32U));
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Geoid infos:\t\t[ %d%c ]\n\r",
                   pGNSSParser_Data->gpgga_data.geoid.height,
                   pGNSSParser_Data->gpgga_data.geoid.mis);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Diff update:\t\t[ %d ]\n\r",
                   pGNSSParser_Data->gpgga_data.update);
    PRINT_INFO((char *)msg);

  }
  else
  {
    (void)snprintf((char *)msg, MSG_SZ,  "Last position wasn't valid.\n\n\r");
    PRINT_INFO((char *)msg);
  }

  PRINT_INFO("\n\n\r>");
}

/* Puts to console the tracking data using the result of parsed GPGGA sentence */
int32_t GNSS_DATA_TrackGotPos(GNSSParser_Data_t *pGNSSParser_Data, uint32_t how_many, uint32_t time)
{
  int32_t tracked = 0;
  for(uint16_t i = 0; i < (uint16_t)how_many; i++)
  {
    if(pGNSSParser_Data->gpgga_data.valid == INVALID)
    {
      break;
    }

    tracked++;

    (void)snprintf((char *)msg, MSG_SZ,  "Position %d just get.\r\n", i + 1U);

    PRINT_INFO((char *)msg);
    if(pGNSSParser_Data->debug == DEBUG_ON)
    {
      float64_t lat_mod = fmod(pGNSSParser_Data->gpgga_data.xyz.lat, 100.0);
      float64_t lon_mod = fmod(pGNSSParser_Data->gpgga_data.xyz.lon, 100.0);

      PRINT_INFO("Debug ON.\r\n");

      (void)snprintf((char *)msg, MSG_SZ,  "UTC:\t\t\t[ %02d:%02d:%02d ]\n\r",
                     pGNSSParser_Data->gpgga_data.utc.hh,
                     pGNSSParser_Data->gpgga_data.utc.mm,
                     pGNSSParser_Data->gpgga_data.utc.ss);
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "Latitude:\t\t[ %.0f' %d'' %c ]\n\r",
                     (pGNSSParser_Data->gpgga_data.xyz.lat - lat_mod) / 100.0,
                     (int16_t)lat_mod,
                     pGNSSParser_Data->gpgga_data.xyz.ns);       
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "Longitude:\t\t[ %.0f' %d'' %c ]\n\r",
                     (pGNSSParser_Data->gpgga_data.xyz.lon - lon_mod) / 100.0,
                     (int16_t)lon_mod,
                     pGNSSParser_Data->gpgga_data.xyz.ew);
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "Satellites locked:\t[ %d ]\n\r",
                     pGNSSParser_Data->gpgga_data.sats);
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "HDOP:\t\t\t[ %.1f ]\n\r",
                     pGNSSParser_Data->gpgga_data.acc);
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "Altitude:\t\t[ %.2f%c ]\n\r",
                     pGNSSParser_Data->gpgga_data.xyz.alt,
                     (pGNSSParser_Data->gpgga_data.xyz.mis + 32U));
      PRINT_INFO((char *)msg);

      (void)snprintf((char *)msg, MSG_SZ, "Geoid infos:\t\t[ %d%c ]\n\r",
                     pGNSSParser_Data->gpgga_data.geoid.height,
                     pGNSSParser_Data->gpgga_data.geoid.mis);
      PRINT_INFO((char *)msg);

      (void)snprintf((char*)msg, MSG_SZ, "Diff update:\t\t[ %d ]\n\r",
                     pGNSSParser_Data->gpgga_data.update);
      PRINT_INFO((char *)msg);

      PRINT_INFO("\n\n\r");
    }
    NMEA_Copy_Data(&stored_positions[i], pGNSSParser_Data->gpgga_data);
    if(time != 0U)
    {
      (void)OS_Delay(time * 1000U);
    }
  }
  return tracked;
}

/*  Puts to console all the position got by a tracking position process */
void GNSS_DATA_PrintTrackedPositions(uint32_t how_many)
{
  for(uint16_t i = 0; i < (uint16_t)how_many; i++)
  {
    float64_t lat_mod = fmod(stored_positions[i].xyz.lat, 100.0);
    float64_t lon_mod = fmod(stored_positions[i].xyz.lon, 100.0);

    (void)snprintf((char *)msg, MSG_SZ,  "Position n. %d:\r\n", i + 1U);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "UTC:\t\t\t[ %02d:%02d:%02d ]\n\r",
                   stored_positions[i].utc.hh, stored_positions[i].utc.mm, stored_positions[i].utc.ss);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Latitude:\t\t[ %.0f' %d'' %c ]\n\r",
                   (stored_positions[i].xyz.lat - lat_mod) / 100.0,
                   (int16_t)lat_mod,
                   stored_positions[i].xyz.ns);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Longitude:\t\t[ %.0f' %d'' %c ]\n\r",
                   (stored_positions[i].xyz.lon - lon_mod) / 100.0,
                   (int16_t)lon_mod,
                   stored_positions[i].xyz.ew);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Satellites locked:\t[ %d ]\n\r",
                   stored_positions[i].sats);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "HDOP:\t\t\t[ %.1f ]\n\r",
                   stored_positions[i].acc);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Altitude:\t\t[ %.2f%c ]\n\r",
                   stored_positions[i].xyz.alt,
                   (stored_positions[i].xyz.mis + 32U));
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Geoid infos:\t\t[ %d%c ]\n\r",
                   stored_positions[i].geoid.height,
                   stored_positions[i].geoid.mis);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ,  "Diff update:\t\t[ %d ]\n\r",
                   stored_positions[i].update);
    PRINT_INFO((char *)msg);

    PRINT_INFO("\r\n\n>");
  }
}

/* Puts to console the info about Fix data for single or combined satellite navigation system. */
void GNSS_DATA_GetGNSInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  PRINT_INFO("\r\n");

  (void)snprintf((char *)msg, MSG_SZ,  "Constellation:\t\t[ %s ]\n",
                 pGNSSParser_Data->gns_data.constellation);
  PRINT_INFO((char *)msg);

  if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$GPGNS") == 0)
  {
    PRINT_INFO("-- only GPS constellation is enabled\n\r");  
  }
  else if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$GLGNS") == 0)
  {
    PRINT_INFO("-- only GLONASS constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$GAGNS") == 0)
  {
    PRINT_INFO("-- only GALILEO constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$BDGNS") == 0)
  {
    PRINT_INFO("-- only BEIDOU constellation is enabled\n\r");    
  }
  else if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$QZGNS") == 0)
  {
    PRINT_INFO("-- only QZSS constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gns_data.constellation, "$GNGSV") == 0)
  {
    PRINT_INFO("-- message to report all satellites for all enabled constellations\n\r");   
  }
  else
  {
    /* nothing to do */
  }

  float64_t lat_mod = fmod(pGNSSParser_Data->gns_data.xyz.lat, 100.0);
  float64_t lon_mod = fmod(pGNSSParser_Data->gns_data.xyz.lon, 100.0);
  (void)snprintf((char *)msg, MSG_SZ,  "UTC:\t\t\t[ %02d:%02d:%02d ]\n\r",
                 pGNSSParser_Data->gns_data.utc.hh,
                 pGNSSParser_Data->gns_data.utc.mm,
                 pGNSSParser_Data->gns_data.utc.ss);
  PRINT_INFO((char *)msg);
  (void)snprintf((char *)msg, MSG_SZ, "Latitude:\t\t[ %.0f' %d'' %f\" %c ]\n\r",
                 (pGNSSParser_Data->gns_data.xyz.lat - lat_mod) / 100.0,
                 minute_part(lat_mod),
				 seconds(lat_mod,minute_part(lat_mod)),
                 pGNSSParser_Data->gns_data.xyz.ns);

  PRINT_INFO((char *)msg);
  (void)snprintf((char *)msg, MSG_SZ, "Longitude:\t\t[ %.0f' %d'' %f\" %c ]\n\r",
                 (pGNSSParser_Data->gns_data.xyz.lon - lon_mod) / 100.0,
                 minute_part(lon_mod),
				 seconds(lon_mod,minute_part(lon_mod)),
                 pGNSSParser_Data->gns_data.xyz.ew);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Satellites locked:\t[ %d ]\n\r",
                 pGNSSParser_Data->gns_data.sats);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "HDOP:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gns_data.hdop);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Altitude:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gns_data.xyz.alt);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Geoid infos:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gns_data.geo_sep);
  PRINT_INFO((char *)msg);
    
  PRINT_INFO("\n\n\r>");
}


/* Puts to console the info about GPS Pseudorange Noise Statistics */
void GNSS_DATA_GetGPGSTInfo(GNSSParser_Data_t *pGNSSParser_Data)
{  
  PRINT_INFO("\r\n");

  (void)snprintf((char *)msg, MSG_SZ,  "UTC:\t\t\t[ %02d:%02d:%02d ]\n\r",
                 pGNSSParser_Data->gpgst_data.utc.hh,
                 pGNSSParser_Data->gpgst_data.utc.mm,
                 pGNSSParser_Data->gpgst_data.utc.ss);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "EHPE:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.EHPE);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Semi-major Dev:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.semi_major_dev);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Semi-minor Dev:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.semi_minor_dev);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Semi-maior Angle:\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.semi_major_angle);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Lat Err Dev:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.lat_err_dev);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Lon Err Dev:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.lon_err_dev);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Alt Err Dev:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gpgst_data.alt_err_dev);
  PRINT_INFO((char *)msg);

  PRINT_INFO("\n\n\r>");

  return;
}

/* Puts to console the info about Recommended Minimum Specific GPS/Transit data got by the most recent reception process. */
void GNSS_DATA_GetGPRMCInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  PRINT_INFO("\r\n");

  (void)snprintf((char *)msg, MSG_SZ,  "UTC:\t\t\t\t[ %02d:%02d:%02d ]\n\r",
                 pGNSSParser_Data->gprmc_data.utc.hh,
                 pGNSSParser_Data->gprmc_data.utc.mm,
                 pGNSSParser_Data->gprmc_data.utc.ss);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Status:\t\t\t\t[ %c ]\t\t",
                 pGNSSParser_Data->gprmc_data.status);
  PRINT_INFO((char *)msg);
  if (pGNSSParser_Data->gprmc_data.status == (uint8_t)'A')
  {
    PRINT_INFO("-- Valid (reported in 2D and 3D fix conditions)\n\r");
  }
  else if (pGNSSParser_Data->gprmc_data.status == (uint8_t)'V')
  {
    PRINT_INFO("-- Warning (reported in NO FIX conditions)\n\r");
  }
  else
  {
    PRINT_INFO("-- Unknown status\n\r");
  }

  float64_t lat_mod = fmod(pGNSSParser_Data->gprmc_data.xyz.lat, 100.0);
  float64_t lon_mod = fmod(pGNSSParser_Data->gprmc_data.xyz.lon, 100.0);

  (void)snprintf((char *)msg, MSG_SZ, "Latitude:\t\t\t[ %.0f' %02d'' %f\" %c ]\n\r",
                 (pGNSSParser_Data->gprmc_data.xyz.lat - lat_mod) / 100.0,
                 minute_part(lat_mod),
				 seconds(lat_mod,minute_part(lat_mod)),
                 pGNSSParser_Data->gprmc_data.xyz.ns);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Longitude:\t\t\t[ %.0f' %02d'' %f\" %c ]\n\r",
                 (pGNSSParser_Data->gprmc_data.xyz.lon - lon_mod) / 100.0,
                 minute_part(lon_mod),
				 seconds(lon_mod,minute_part(lon_mod)),
                 pGNSSParser_Data->gprmc_data.xyz.ew);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Speed over ground (knots):\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gprmc_data.speed);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Trackgood:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gprmc_data.trackgood);
  PRINT_INFO((char *)msg);

/*
  (void)snprintf((char *)msg, MSG_SZ, "Date (ddmmyy):\t\t\t[ %ld ]\n\r",
                 pGNSSParser_Data->gprmc_data.date);
*/ /* Replaced by the following implementation for compatibility with AC6 and gcc compilers */
  (void)snprintf((char *)msg, MSG_SZ, "Date (ddmmyy):\t\t\t[ %02d%02d%02d ]\n\r",
                 (int16_t)((pGNSSParser_Data->gprmc_data.date/10000)),
                 (int16_t)((pGNSSParser_Data->gprmc_data.date/100) - (100*(pGNSSParser_Data->gprmc_data.date/10000))),
                 (int16_t)(pGNSSParser_Data->gprmc_data.date - (100*(pGNSSParser_Data->gprmc_data.date/100))));
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Magnetic Variation:\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gprmc_data.mag_var);
  PRINT_INFO((char *)msg);

  if ((pGNSSParser_Data->gprmc_data.mag_var_dir != (uint8_t)'E') &&
      (pGNSSParser_Data->gprmc_data.mag_var_dir != (uint8_t)'W')) {
    (void)snprintf((char *)msg, MSG_SZ, "Magnetic Var. Direction:\t[ - ]\n\r");
  }
  else {
    (void)snprintf((char *)msg, MSG_SZ, "Magnetic Var. Direction:\t[ %c ]\n\r",
                   pGNSSParser_Data->gprmc_data.mag_var_dir);
  }
  PRINT_INFO((char *)msg);

  PRINT_INFO("\n\n\r>");

  return;
}

/* Puts to console the info about GSA satellites got by the most recent reception process. */
void GNSS_DATA_GetGSAInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  PRINT_INFO("\r\n");

  (void)snprintf((char *)msg, MSG_SZ,  "Constellation:\t\t[ %s ]\t",
                 pGNSSParser_Data->gsa_data.constellation);
  PRINT_INFO((char *)msg);

  if (strcmp((char*)pGNSSParser_Data->gsa_data.constellation, "$GPGSA") == 0)
  {
    PRINT_INFO("-- only GPS constellation is enabled\n\r");    
  }
  else if (strcmp((char*)pGNSSParser_Data->gsa_data.constellation, "$GLGSA") == 0)
  {
    PRINT_INFO("-- only GLONASS constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsa_data.constellation, "$GAGSA") == 0)
  {
    PRINT_INFO("-- only GALILEO constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsa_data.constellation, "$BDGSA") == 0)
  {
    PRINT_INFO("-- only BEIDOU constellation is enabled\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsa_data.constellation, "$GNGSA") == 0)
  {
     PRINT_INFO("-- more than one constellation is enabled\n\r");   
  }
  else
  {
    /* nothing to do */
  }

  (void)snprintf((char *)msg, MSG_SZ,  "Operating Mode:\t\t[ %c ]\t\t",
                 pGNSSParser_Data->gsa_data.operating_mode);
  PRINT_INFO((char *)msg);

  if (pGNSSParser_Data->gsa_data.operating_mode == (uint8_t)'A')
  {
    PRINT_INFO("-- Auto (2D/3D)\n\r");
  }
  else if (pGNSSParser_Data->gsa_data.operating_mode == (uint8_t)'M')
  {
    PRINT_INFO("-- Manual\n\r");
  }
  else
  {
    PRINT_INFO("-- Unknown op mode\n\r");
  }

  (void)snprintf((char *)msg, MSG_SZ, "Current Mode:\t\t[ %d ]\t\t",
                 pGNSSParser_Data->gsa_data.current_mode);
  PRINT_INFO((char *)msg);

  if (pGNSSParser_Data->gsa_data.current_mode == 1)
  {
    PRINT_INFO("-- no fix available\n\r");
  }
  else if (pGNSSParser_Data->gsa_data.current_mode == 2)
  {
    PRINT_INFO("-- 2D\n\r");
  }
  else if (pGNSSParser_Data->gsa_data.current_mode == 3)
  {
    PRINT_INFO("-- 3D\n\r");
  }
  else
  {
    /* nothing to do */
  }

  int16_t *sat_prn = (int16_t*)(pGNSSParser_Data->gsa_data.sat_prn);
  for (uint8_t i=0; i<12U; i++)
  {  
    (void)snprintf((char *)msg, MSG_SZ, "SatPRN%02d:\t\t[ %d ]\n\r", i+1U,
                   (*(&sat_prn[i])));
    PRINT_INFO((char *)msg);
  }

  (void)snprintf((char *)msg, MSG_SZ, "PDOP:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gsa_data.pdop);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "HDOP:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gsa_data.hdop);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "VDOP:\t\t\t[ %.01f ]\n\r",
                 pGNSSParser_Data->gsa_data.vdop);
  PRINT_INFO((char *)msg);

  PRINT_INFO("\n\n\r>");

  return;
}

/* Puts to console the info about GSV satellites got by the most recent reception process. */
void GNSS_DATA_GetGSVInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  int16_t i;
  int16_t tot_sats = pGNSSParser_Data->gsv_data.tot_sats;
  int16_t current_sats = pGNSSParser_Data->gsv_data.current_sats;
  int16_t amount = pGNSSParser_Data->gsv_data.amount;
  int16_t number = pGNSSParser_Data->gsv_data.number;
  
  uint8_t degree_ext_ASCII_char = 248;
  
  PRINT_INFO("\r\n");
  
  (void)snprintf((char *)msg, MSG_SZ,  "Constellation:\t\t[ %s ]\t",
                 pGNSSParser_Data->gsv_data.constellation);
  PRINT_INFO((char *)msg);
  
  if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$GPGSV") == 0)
  {
    PRINT_INFO("-- message to report all GPS satellites\n\r");    
  }
  else if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$GLGSV") == 0)
  {
    PRINT_INFO("-- message to report all GLONASS satellites\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$GAGSV") == 0)
  {
    PRINT_INFO("-- message to report all GALILEO satellites\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$BDGSV") == 0)
  {
    PRINT_INFO("-- message to report all BEIDOU satellites\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$QZGSV") == 0)
  {
    PRINT_INFO("-- message to report all QZSS satellites\n\r");
  }
  else if (strcmp((char*)pGNSSParser_Data->gsv_data.constellation, "$GNGSV") == 0)
  {
     PRINT_INFO("-- message to report all satellites for all enabled constellations\n\r");   
  }
  else
  {
    /* nothing to do */
  }

  (void)snprintf((char *)msg, MSG_SZ, "GSV message:\t\t[ %d of %d ]\n\r", number, amount);
  PRINT_INFO((char *)msg);

  (void)snprintf((char *)msg, MSG_SZ, "Num of Satellites:\t[ %d of %d ]\n\r", pGNSSParser_Data->gsv_data.current_sats, tot_sats);
  PRINT_INFO((char *)msg);

  PRINT_INFO("\n\r");

  for (i=0; i<current_sats; i++)
  {
    (void)snprintf((char *)msg, MSG_SZ, "Sat%02dPRN:\t\t[ %03d ]\n\r", i+1+((number-1)*GSV_MSG_SATS),
                   pGNSSParser_Data->gsv_data.gsv_sat_i[i].prn);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Sat%02dElev (%c):\t\t[ %03d ]\n\r", i+1+((number-1)*GSV_MSG_SATS), degree_ext_ASCII_char,
                   pGNSSParser_Data->gsv_data.gsv_sat_i[i].elev);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Sat%02dAzim (%c):\t\t[ %03d ]\n\r", i+1+((number-1)*GSV_MSG_SATS), degree_ext_ASCII_char,
                   pGNSSParser_Data->gsv_data.gsv_sat_i[i].azim);
    PRINT_INFO((char *)msg);

    (void)snprintf((char *)msg, MSG_SZ, "Sat%02dCN0 (dB):\t\t[ %03d ]\n\r", i+1+((number-1)*GSV_MSG_SATS),
                   pGNSSParser_Data->gsv_data.gsv_sat_i[i].cn0);
    PRINT_INFO((char *)msg);

    PRINT_INFO("\n\r");
  }

  PRINT_INFO("\r\n>");

}

/* Puts to console the info about FW version. */
void GNSS_DATA_GetPSTMVerInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  if(strlen((char *)pGNSSParser_Data->pstmver_data.pstmver_string) != 0U )
  {
    PRINT_INFO("\r\n");
    (void)snprintf((char *)msg, MSG_SZ,  "Version Info:\t\t[ %s ]\t",
                   pGNSSParser_Data->pstmver_data.pstmver_string);
    PRINT_INFO((char *)msg);

    PRINT_INFO("\n\n\r");
  }
  else
  {
    (void)snprintf((char *)msg, MSG_SZ,  "No version info available.\n\n\r");
    PRINT_INFO((char *)msg);
  }

  PRINT_INFO("\n\r>");
}

/* Puts to console the geofence infos each time an alarm is received. */
void GNSS_DATA_GetGeofenceInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  uint8_t *geofenceCirclePosition[] = {
    (uint8_t *)"Unknown",
    (uint8_t *)"Outside",
    (uint8_t *)"Boundary",
    (uint8_t *)"Inside"
  };

  if(pGNSSParser_Data->geofence_data.op == GNSS_FEATURE_EN_MSG)
  {
    if(pGNSSParser_Data->geofence_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Geofence sub-system reply:\t[ %s ] (Saving params...)\t",
                     "GEOFENCE SUB-SYSTEM OK");

      OS_Delay(500); /* Seems to mitigate error events in case of I2C channel */
      GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Geofence sub-system reply:\t[ %s ]\t",
                     "GEOFENCE SUB-SYSTEM ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->geofence_data.op == GNSS_GEOFENCE_CFG_MSG)
  {
    if(pGNSSParser_Data->geofence_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Geofence Configuration:\t[ %s ]\t",
                     "GEOFENCE CFG OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Geofence Configuration:\t[ %s ]\t",
                     "GEOFENCE CFG ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->geofence_data.op == GNSS_GEOFENCE_STATUS_MSG)
  {
    PRINT_INFO("\r\n");
    (void)snprintf((char *)msg, MSG_SZ,  "Time/Date:\t\t%02d:%02d:%02d %02d/%02d/%04d\n",
                   pGNSSParser_Data->geofence_data.timestamp.hh,
                   pGNSSParser_Data->geofence_data.timestamp.mm,
                   pGNSSParser_Data->geofence_data.timestamp.ss,
                   pGNSSParser_Data->geofence_data.timestamp.day,
                   pGNSSParser_Data->geofence_data.timestamp.month,
                   pGNSSParser_Data->geofence_data.timestamp.year);
    PRINT_INFO((char *)msg);

    int32_t *geofence_status = pGNSSParser_Data->geofence_data.status;
    for(uint8_t i = 0; i<MAX_GEOFENCES_NUM; i++)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Position circle[%d]:\t%s\n",
                     i, geofenceCirclePosition[*(&geofence_status[i])]);
      PRINT_INFO((char *)msg);
    }
  }
  else if(pGNSSParser_Data->geofence_data.op == GNSS_GEOFENCE_ALARM_MSG)
  {
    PRINT_INFO("\r\nGeofence status:\r\n");
    // Print ID - Status
    int16_t idAlarm = pGNSSParser_Data->geofence_data.idAlarm;
    (void)snprintf((char *)msg, MSG_SZ,  "[Circle %d - %s]\t", idAlarm,
                   geofenceCirclePosition[pGNSSParser_Data->geofence_data.status[idAlarm]]);
    PRINT_INFO((char *)msg);

    // Print time date
    (void)snprintf((char *)msg, MSG_SZ,  "%02d:%02d:%02d %02d/%02d/%04d\n",
                   pGNSSParser_Data->geofence_data.timestamp.hh,
                   pGNSSParser_Data->geofence_data.timestamp.mm,
                   pGNSSParser_Data->geofence_data.timestamp.ss,
                   pGNSSParser_Data->geofence_data.timestamp.day,
                   pGNSSParser_Data->geofence_data.timestamp.month,
                   pGNSSParser_Data->geofence_data.timestamp.year);
    PRINT_INFO((char *)msg);
  }
  else
  {
    /* nothing to do */
  }

  PRINT_INFO("\n\r>");
}

/* Puts to console the info about Odometer. */
void GNSS_DATA_GetOdometerInfo(const GNSSParser_Data_t *pGNSSParser_Data)
{ 
  if(pGNSSParser_Data->odo_data.op == GNSS_FEATURE_EN_MSG)
  {
    if(pGNSSParser_Data->odo_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer sub-system reply:\t[ %s ] (Saving params...)\t",
                     "ODOMETER SUB-SYSTEM OK");
      GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer sub-system reply:\t[ %s ]\t",
                     "ODOMETER SUB-SYSTEM ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->odo_data.op == GNSS_ODO_START_MSG)
  {
    if(pGNSSParser_Data->odo_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer Operation:\t[ %s ]\t",
                     "START ODOMETER OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer Operation:\t[ %s ]\t",
                     "START ODOMETER ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->odo_data.op == GNSS_ODO_STOP_MSG)
  {
    if(pGNSSParser_Data->odo_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer Operation:\t[ %s ]\t",
                     "STOP ODOMETER OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Odometer Operation:\t[ %s ]\t",
                     "STOP ODOMETER ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else
  {
    /* nothing to do */
  }

  PRINT_INFO("\n\r>");

  return;
}

/* Puts to console the info about Datalog. */
void GNSS_DATA_GetDatalogInfo(const GNSSParser_Data_t *pGNSSParser_Data)
{ 
  if(pGNSSParser_Data->datalog_data.op == GNSS_FEATURE_EN_MSG)
  {
    if(pGNSSParser_Data->datalog_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog sub-system reply:\t[ %s ] (Saving params...)\t",
                     "DATALOG SUB-SYSTEM OK");
      GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog sub-system reply:\t[ %s ]\t",
                     "DATALOG SUB-SYSTEM ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->datalog_data.op == GNSS_DATALOG_CFG_MSG)
  {
    if(pGNSSParser_Data->datalog_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Configuring:\t\t[ %s ]\t",
                     "DATALOG CFG OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Configuring:\t\t[ %s ]\t",
                     "DATALOG CFG ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->datalog_data.op == GNSS_DATALOG_START_MSG)
  {
    if(pGNSSParser_Data->datalog_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Start:\t\t[ %s ]\t",
                     "DATALOG START OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Start:\t\t[ %s ]\t",
                     "DATALOG START ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->datalog_data.op == GNSS_DATALOG_STOP_MSG)
  {
    if(pGNSSParser_Data->datalog_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Stop:\t\t[ %s ]\t",
                     "DATALOG STOP OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Stop:\t\t[ %s ]\t",
                     "DATALOG STOP ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else if(pGNSSParser_Data->datalog_data.op == GNSS_DATALOG_ERASE_MSG)
  {
    if(pGNSSParser_Data->datalog_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Erase:\t\t[ %s ]\t",
                     "DATALOG ERASE OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ,  "Datalog Erase:\t\t[ %s ]\t",
                     "DATALOG ERASE ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else
  {
    /* nothing to do */
  }

  PRINT_INFO("\n\r>");
  
}

 /* Puts to console the confirmation of an updated message list. */
void GNSS_DATA_GetMsglistAck(const GNSSParser_Data_t *pGNSSParser_Data)
{ 
  if(pGNSSParser_Data->result == GNSS_OP_OK)
  {
    OS_Delay(500); /* Seems to mitigate error events in case of I2C */
    GNSS_DATA_SendCommand((uint8_t *)"$PSTMSAVEPAR");
    (void)snprintf((char *)msg, MSG_SZ,  "Saving NMEA msg configuration...\t");
    PRINT_INFO((char *)msg);
    PRINT_INFO("\n\r>");
  }
}

/* Puts to console the confirmation after saving params.
   NOTE: GNSS must be reset for the new saved params to have effect.
*/
void GNSS_DATA_GetGNSSAck(const GNSSParser_Data_t *pGNSSParser_Data)
{ 
  if(pGNSSParser_Data->result == GNSS_OP_OK)
  {
    (void)GNSS_Wrapper_Reset();
    (void)snprintf((char *)msg, MSG_SZ,  " Resetting...\t");
    PRINT_INFO((char *)msg);
    PRINT_INFO("\n\r>");    
  }
}

/* Configures the message list. */
void GNSS_DATA_CfgMessageList(int lowMask, int highMask)
{
  /* See CDB-ID 201 - This LOW_BITS Mask enables the following messages:
   * 0x1 $GPGNS Message
   * 0x2 $GPGGA Message
   * 0x4 $GPGSA Message
   * 0x8 $GPGST Message
   * 0x40 $GPRMC Message
   * 0x80000 $GPGSV Message
   * 0x100000 $GPGLL Message
   */
//  int lowMask = 0x18004F;

  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMCFGMSGL,%d,%d,%x,%x",
                 0, /*NMEA 0*/
                 1, /*Rate*/
                 lowMask,
                 highMask);

  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to enable/disable Geofence */
void GNSS_DATA_EnableGeofence(int toggle)
{
  //$PSTMCFGGEOFENCE,<en>,<tol>*<checksum><cr><lf>
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMCFGGEOFENCE,%d,%d",toggle,1);
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Configures Geofence */
void GNSS_DATA_ConfigGeofence(void* gnss_geofence)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMGEOFENCECFG,%d,%d,%d,%lf,%lf,%lf",
                 ((GNSSGeofence_t*)gnss_geofence)->id,
                 ((GNSSGeofence_t*)gnss_geofence)->enabled,
                 ((GNSSGeofence_t*)gnss_geofence)->tolerance,
                 ((GNSSGeofence_t*)gnss_geofence)->lat,
                 ((GNSSGeofence_t*)gnss_geofence)->lon,
                 ((GNSSGeofence_t*)gnss_geofence)->radius);
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to enable/disable odometer. */
void GNSS_DATA_EnableOdo(int toggle)
{
  //$PSTMCFGODO,<en>,<enmsg>,<alarm>*<checksum><cr><lf>
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMCFGODO,%d,1,1",toggle);
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to start odometer. */
void GNSS_DATA_StartOdo(unsigned alarmDistance)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMODOSTART,%08x", alarmDistance);
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to stop odometer. */
void GNSS_DATA_StopOdo(void)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMODOSTOP");
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to enable/disable Datalog. */
void GNSS_DATA_EnableDatalog(int toggle)
{
  //$PSTMCFGLOG,<en>,<circ>,<rectype>,<oneshot>,<rate>,<speed>,<dist>*<checksum><cr><lf>
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMCFGLOG,%d,%d,%d,%d,%u,%u,%u",
                 toggle, //Enable/Disable the log
                 1, //Enable/Disable circular mode
                 1, //Record type
                 0, //Enable/Disable one shot mode
                 5, //time interval in seconds between two consecutive logged records
                 0, //minimum speed threshold
                 0  //distance threshold
                   );
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to configure Datalog. */
void GNSS_DATA_ConfigDatalog(void *gnss_datalog)
{
  //$PSTMLOGCREATE,<cfg>,<min-rate>,<min-speed>,<min-position>,<logmask>*<checksum><cr><lf>
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMLOGCREATE,%03x,%u,%u,%u,%d",
                 ((((GNSSDatalog_t*)gnss_datalog)->enableBufferFullAlarm)<<1)|(((GNSSDatalog_t*)gnss_datalog)->enableCircularBuffer),
                 ((GNSSDatalog_t*)gnss_datalog)->minRate,
                 ((GNSSDatalog_t*)gnss_datalog)->minSpeed,
                 ((GNSSDatalog_t*)gnss_datalog)->minPosition,
                 ((GNSSDatalog_t*)gnss_datalog)->logMask
                   );
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to start Datalog. */
void GNSS_DATA_StartDatalog(void)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMLOGSTART");
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to stop Datalog. */
void GNSS_DATA_StopDatalog(void)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMLOGSTOP");
  
  GNSS_DATA_SendCommand(gnssCmd);
}

/* Sends a command to erase Datalog. */
void GNSS_DATA_EraseDatalog(void)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMLOGERASE");
  
  GNSS_DATA_SendCommand(gnssCmd);
}

#ifdef ASSISTED_GNSS

/* Requests the generation of a password to access the A-GNSS server */
void GNSS_DATA_PassGen(uint32_t gpsTime)
{
  (void)snprintf((char *)gnssCmd, CMD_SZ, "$PSTMSTAGPS8PASSGEN,%ld,%s,%s",gpsTime,vendorId,modelId);

  GNSS_DATA_SendCommand(gnssCmd);
}

/* Puts to console the info about Password generation for A-GNSS. */
void GNSS_DATA_GetPSTMPassInfo(GNSSParser_Data_t *pGNSSParser_Data)
{
  if(pGNSSParser_Data->pstmpass_data.result == GNSS_OP_OK)
  {
    (void)snprintf((char *)msg, MSG_SZ, "Password Generation:\t[ %s ]\t",
                   "PASS GEN OK");
    PRINT_INFO((char *)msg);

    /* DeviceId */
    PRINT_INFO("\r\n");
    (void)snprintf((char *)msg, MSG_SZ, "Device Id:\t[ %s ]\t",
             pGNSSParser_Data->pstmpass_data.deviceId);
    PRINT_INFO((char *)msg);

    /* Password */
    PRINT_INFO("\r\n");
    (void)snprintf((char *)msg, MSG_SZ, "Password:\t[ %s ]\t",
             pGNSSParser_Data->pstmpass_data.pwd);
    PRINT_INFO((char *)msg);
    
    PRINT_INFO("\n\n\r");
  }
  else
  {
    (void)snprintf((char *)msg, MSG_SZ, "Password Generation:\t[ %s ]\t",
                   "PASS GEN ERROR");
    PRINT_INFO((char *)msg);
  }
  
  PRINT_INFO("\n\r>");

  return;
}

/* Puts to console the STAGPS related info (A-GNSS). */
void GNSS_DATA_GetPSTMAGPSInfo(const GNSSParser_Data_t *pGNSSParser_Data)
{
  /* Status */
  if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_STATUS_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.status != 0)
    {
      (void)snprintf((char *)msg, MSG_SZ, "STAGPS processing is still ongoing");
      PRINT_INFO((char *)msg);
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "STAGPS processing is completed");
      PRINT_INFO((char *)msg);
    }
  }
  /* Begin */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_BEGIN_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED BEGIN OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED BEGIN ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  /* Block type */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_BLKTYPE_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "BLOCK TYPE OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "BLOCK TYPE ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  /* Slot freq */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_SLOTFRQ_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "SLOT FREQ OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "SLOT FREQ ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  /* Seed pkt */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_SEEDPKT_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED PKT OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED PKT ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  /* Prop */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_PROP_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED PROPAGATION OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "SEED PROPAGATION ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  /* Init time */
  else if(pGNSSParser_Data->pstmagps_data.op == GNSS_AGPS_INITTIME_MSG)
  {
    if(pGNSSParser_Data->pstmagps_data.result == GNSS_OP_OK)
    {
      (void)snprintf((char *)msg, MSG_SZ, "INIT TIME OK");
    }
    else
    {
      (void)snprintf((char *)msg, MSG_SZ, "INIT TIME ERROR");
    }
    PRINT_INFO((char *)msg);
  }
  else
  {
    /* do nothing */
  }
    
  PRINT_INFO("\n\r>");

  return;
}
#endif /* ASSISTED_GNSS */

__weak int GNSS_PRINT(char *pBuffer)
{
  /* Implement this function at application level */

  return 0;
}

