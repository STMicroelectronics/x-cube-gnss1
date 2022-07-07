/**
  ******************************************************************************
  * @file       a_gnss.c
  * @author     SRA
  * @brief      Implements high level API for Assisted GNSS
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
#include <stdlib.h>
#include "gnss_parser.h"
#include "gnss_if.h"
#include "gnss_data.h"
#include "assisted_gnss.h"
#include "gnss_ephemeris.h"
#include "gnss_almanac.h"
#include "httpclient.h"
#include "cJSON.h"
#include "mbedtls/base64.h"
#include "gnss1a1_conf.h"

/* Defines -------------------------------------------------------------------*/
//#define PRINT_JSON
#define PROPAGATE_PR_SEED
#define PROPAGATE_RT_SEED

/* Global variables ----------------------------------------------------------*/

extern RTC_HandleTypeDef hrtc;

/* Private variables ---------------------------------------------------------*/
static seed_info_t seed_info;

/* Private functions -----------------------------------------------------------*/

/* Checks the seed validity */
static int seed_valid(uint8_t *seed, int32_t len, seed_req_t seedReq)
{
  uint8_t pr_open[]  = "[{\"ee\":{\"body\"";
  uint8_t pr_close[] = "\"status\":200}}]";
  uint8_t rt_open[]  = "[{\"rtAssistance\":";
  uint8_t rt_close[] = "\"status\":200}}]";

  uint8_t *open;
  uint8_t *close;

  if(seedReq == PR_SEED)
  {
    open = pr_open;
    close = pr_close;
  }
  else if(seedReq == RT_SEED)
  {
    open = rt_open;
    close = rt_close;
  }
  else
  {
    return -1;
  }

  if(len <= 0)
  {
    return -1;
  }
  /* Examine the beginning of the buffer, it should be [{\"ee\":{\"body\" */
  if(memcmp((void*)seed, (void*)open, strlen((char*)open)) != 0)
  {
    return -2;
  }
  /* Examine the last 15 bytes of the seed, if they are \"status\":200}}] then the seed is complete */
  if(memcmp((void*)(seed + (len - 15)), (void*)close, strlen((char*)close)) != 0)
  {
    return -3;
  }

  return 0;
}

/* Prints seed in Json format */
#ifdef PRINT_JSON
static void printJson(uint8_t *jsonBuffer)
{
  cJSON *root = cJSON_Parse((char *)jsonBuffer);

  if(root != NULL)
  {
    char *str = cJSON_Print(root);
    // Free memory
    if(str != NULL)
    {
      (void)printf("root_str=%s\n", str);
      free(str);
    }
    cJSON_Delete(root);
  }
}
#endif /* PRINT_JSON */

/* Decodes Leap Sec part */
static int decodeJsonLeapsec(void)
{
  // First, parse the whole seed
  cJSON *root = cJSON_Parse((char *)seed_info.jsonBuffer);
  if(root == NULL)
  {
    return -1;
  }

  cJSON *child = root->child;
  while(child != NULL && strcmp(child->string, "body") != 0)
  {  
    child = child->child;
  }
  if(child != NULL)
  {
    int items_count = cJSON_GetArraySize(child);
    for (int i = 0; i < items_count; i++)
    {
      // Get the JSON element and then get the values
      cJSON *item = cJSON_GetArrayItem(child, i);
      if(strcmp(item->string, "leap") == 0)
      {
        seed_info.curr_secs = cJSON_GetObjectItem(item, "currSecs")->valueint;
        seed_info.next_gps_time = cJSON_GetObjectItem(item, "nextGpsTime")->valueint;
        seed_info.next_secs = cJSON_GetObjectItem(item, "nextSecs")->valueint;
        printf("curr_secs(%ld), next_gps_time(%ld) next_secs(%ld)\n", seed_info.curr_secs, seed_info.next_gps_time, seed_info.next_secs);
        break;
      }
    }
  }
  // Free memory
  cJSON_Delete(root);

  return 0;
}

/* Parses JSON containing Seed info (PR-GNSS) */
static size_t decodeJsonSeed(seed_type_t seed_type)
{
  char req[8] = {0};
  size_t len_block_type;
  size_t len_slot_freq;
  size_t len_time_model;
  size_t len_seed = 0;

  // First, parse the whole seed
  cJSON *root = cJSON_Parse((char *)seed_info.jsonBuffer);
  if(root == NULL)
  {
    AGNSS_PRINT_OUT("ERROR: Json root is NULL\n");
    return 0;
  }

  switch (seed_type)
  {
  case GPS_SEED:
    strcpy(req, "gps");
    break;
  case GLO_SEED:
    strcpy(req, "glonass");
    break;
  case GAL_SEED:
    strcpy(req, "galileo");
    break;
  case BEI_SEED:
    strcpy(req, "beidou");
    break;
  default:
    AGNSS_PRINT_OUT("ERROR: Unknown constellation\n");
  }

  cJSON *child = root->child;
  while(child != NULL && strcmp(child->string, "body") != 0)
  {
    child = child->child;
  }
  if(child != NULL)
  {
    int items_count = cJSON_GetArraySize(child);
    for (int i = 0; i < items_count; i++)
    {
      // Get the JSON element and then get the values
      cJSON *item = cJSON_GetArrayItem(child, i);
      if(strcmp(item->string, req) == 0)
      {
        // Get the JSON element and then get the values
        cJSON *bt_item = cJSON_GetObjectItem(item, "blockTypes");
        if(bt_item != NULL)
        {
          char *block_types_str = bt_item->valuestring;
          mbedtls_base64_decode(seed_info.block_types,
                                MAX_BLOCK_TYPE_SIZE,
                                &len_block_type,
                                (unsigned char *)block_types_str,
                                strlen(block_types_str));
//          printf( "Block Types (base64):\n" );
//          for(int k=0; k<len_block_type; k++)
//            printf( "0x%02X,", seed_info.block_types[k] );
//          printf( "\n" );
        }
        // Get the JSON element and then get the values
        cJSON *tm_item = cJSON_GetObjectItem(item, "timeModel");
        if(tm_item != NULL)
        {
          char *time_model_str = tm_item->valuestring;
          mbedtls_base64_decode(seed_info.time_model,
                                MAX_TIME_MODEL_SIZE,
                                &len_time_model,
                                (unsigned char *)time_model_str,
                                strlen(time_model_str));
//          printf( "Time Model (base64):\n" );
//          for(int k=0; k<len_time_model; k++)
//            printf( "0x%02X,", seed_info.time_model[k] );
//          printf( "\n" );
        }
        // Get the JSON element and then get the values
        cJSON *sf_item = cJSON_GetObjectItem(item, "slotFreq");
        if(sf_item != NULL)
        {
          char *slot_freq_str = sf_item->valuestring;
          mbedtls_base64_decode(seed_info.slot_freq,
                                MAX_SLOT_FREQ_SIZE,
                                &len_slot_freq,
                                (unsigned char *)slot_freq_str,
                                strlen(slot_freq_str));
//          printf( "Slot Freq (base64):\n" );
//          for(int k=0; k<len_slot_freq; k++)
//            printf( "0x%02X,", seed_info.slot_freq[k] );
//          printf( "\n" );
        }
        // Get the JSON element and then get the values
        cJSON *s_item = cJSON_GetObjectItem(item, "seed");
        if(s_item != NULL)
        {
          char *seed_str = s_item->valuestring;
          mbedtls_base64_decode(seed_info.seed,
                                seed_info.seed_size,
                                &len_seed,
                                (unsigned char *)seed_str,
                                strlen(seed_str));

          printf( "Seed (seed_size %d, base64 - len %d):\n", seed_info.seed_size, len_seed );
//          for(int k=0; k<len_seed; k++)
//            printf( "0x%02X,", seed_info.seed[k] );
//          printf( "\n" );
        }
      }
    }
  }
  // Free memory
  cJSON_Delete(root);

  return len_seed;
}

/* Parses JSON containing Ephemeris info (RT-GNSS) */
static void decodeJsonEph(seed_type_t seed_type)
{
  char req[10] = {0};
  char req_nkc[10] = {0};
  size_t len_eph_seed = 0;
  size_t len_eph_nkc_seed = 0;

  // First, parse the whole seed
  cJSON *root = cJSON_Parse((char *)seed_info.jsonBuffer);
  if(root == NULL)
  {
    AGNSS_PRINT_OUT("ERROR: EPH Json root is NULL\n");
    return;
  }

  switch (seed_type)
  {
  case GPS_SEED:
    strcpy(req, "GPS:1NAC");
    break;
  case GLO_SEED:
    strcpy(req, "GLO:2NAC");
    strcpy(req_nkc, "GLO:2NKC");
    break;
  case GAL_SEED:
    strcpy(req, "GAL:2NAC");
    break;
  case BEI_SEED:
    strcpy(req, "BDS:2NAC");
    break;
  default:
    AGNSS_PRINT_OUT("ERROR: Unknown constellation\n");
  }

  cJSON *child = root->child;
  while(child != NULL && strcmp(child->string, "body") != 0)
  {
    child = child->child;
  }
  if(child != NULL)
  {
    int items_count = cJSON_GetArraySize(child);
    for (int i = 0; i < items_count; i++)
    {
      cJSON * subitem = cJSON_GetArrayItem(child, i);
      cJSON *eph = cJSON_GetObjectItem(subitem, req);
      if(eph != NULL)
      {
        char *seed_str = eph->valuestring;
        mbedtls_base64_decode(seed_info.eph_seed,
                              seed_info.eph_seed_size,
                              &len_eph_seed,
                              (unsigned char *)seed_str,
                              strlen(seed_str));

        printf( "Eph Seed (base64 - bytes written %d):\n", len_eph_seed );
//        for(int k=0; k<len_eph_seed; k++)
//          printf( "0x%02X,", seed_info.eph_seed[k] );
//        printf( "\n" );
      }
    }

    if(seed_type == GLO_SEED)
    {
      for (int i = 0; i < items_count; i++)
      {
        cJSON * subitem = cJSON_GetArrayItem(child, i);
        cJSON *eph = cJSON_GetObjectItem(subitem, req_nkc);
        if(eph != NULL)
        {
          char *seed_str = eph->valuestring;
          mbedtls_base64_decode(seed_info.eph_nkc_seed,
                                seed_info.eph_nkc_seed_size,
                                &len_eph_nkc_seed,
                                (unsigned char *)seed_str,
                                strlen(seed_str));

          printf( "Eph NKC Seed (base64 - bytes written %d):\n", len_eph_nkc_seed );
//          for(int k=0; k<len_eph_nkc_seed; k++)
//            printf( "0x%02X,", seed_info.eph_nkc_seed[k] );
//          printf( "\n" );
        }
      }
    }
  }
  // Free memory
  cJSON_Delete(root);
}

/* Parses JSON containing Alamnac info (RT-GNSS) */
static void decodeJsonAlm(seed_type_t seed_type)
{
  char req[10] = {0};
  size_t len_alm_seed = 0;

  // First, parse the whole seed
  cJSON *root = cJSON_Parse((char *)seed_info.jsonBuffer);
  if(root == NULL)
  {
    AGNSS_PRINT_OUT("ERROR: ALM Json root is NULL\n");
    return;
  }

  switch (seed_type)
  {
  case GPS_SEED:
    strcpy(req, "GPS:1ALM");
    break;
  case GLO_SEED:
    strcpy(req, "GLO:2ALM");
    break;
  case GAL_SEED:
    strcpy(req, "GAL:2ALM");
    break;
  case BEI_SEED:
    strcpy(req, "BDS:2ALM");
    break;
  default:
    AGNSS_PRINT_OUT("ERROR: Unknown constellation\n");
  }

  cJSON *child = root->child;
  while(child != NULL && strcmp(child->string, "body") != 0)
  {
    child = child->child;
  }
  if(child != NULL)
  {
    int items_count = cJSON_GetArraySize(child);
    for (int i = 0; i < items_count; i++)
    {
      cJSON * subitem = cJSON_GetArrayItem(child, i);
      cJSON *alm = cJSON_GetObjectItem(subitem, req);
      if(alm != NULL)
      {
        char *seed_str = alm->valuestring;
        mbedtls_base64_decode(seed_info.alm_seed,
                              seed_info.alm_seed_size,
                              &len_alm_seed,
                              (unsigned char *)seed_str,
                              strlen(seed_str));

        printf( "Alm Seed (base64 - bytes written %d):\n", len_alm_seed );
//        for(int k=0; k<len_alm_seed; k++)
//          printf( "0x%02X,", seed_info.alm_seed[k] );
//        printf( "\n" );
      }
    }
  }
  // Free memory
  cJSON_Delete(root);
}

/* Public functions -----------------------------------------------------------*/

/* Parses option provided by the user according to the requested constellation */
int AppAGNSS_ParseSeedOp(char *cmd)
{
  int seedMask = 0;
  const char s[2] = ",";
  char *constellation_option;

  /* get the first token */
  constellation_option = strtok(cmd, s);
  constellation_option = strtok(NULL, s);
  if(constellation_option == NULL)
  {
    AGNSS_PRINT_OUT("Please, specify constellation flag\r\n> ");
    return seedMask;
  }

  if( (strchr(constellation_option,'G')!= NULL) || (strchr(constellation_option,'g')!= NULL))
  {
    seedMask |= GPS_CONSTELLATION;
    AGNSS_PRINT_OUT("GPS_CONSTELLATION\r\n");
  }
  if( (strchr(constellation_option,'R')!= NULL) || (strchr(constellation_option,'r')!= NULL))
  {
    seedMask |= GLO_CONSTELLATION;
    AGNSS_PRINT_OUT("GLO_CONSTELLATION\r\n");
  }
  if( (strchr(constellation_option,'E')!= NULL) || (strchr(constellation_option,'e')!= NULL))
  {
    seedMask |= GAL_CONSTELLATION;
    AGNSS_PRINT_OUT("GAL_CONSTELLATION\r\n");
  }
  if( (strchr(constellation_option,'C')!= NULL) || (strchr(constellation_option,'c')!= NULL))
  {
    seedMask |= BEI_CONSTELLATION;
    AGNSS_PRINT_OUT("BEI_CONSTELLATION\r\n");
  }
  /* Lightweight cJSON parser could not handle huge buffers, so exclude multiple constellations case*/
//  if ( (strchr(constellation_option,'A')!= NULL) || (strchr(constellation_option,'a')!= NULL))
//  {
//    seedMask = DEFAULT_CONSTELLATION;
//    AGNSS_PRINT_OUT("DEFAULT_CONSTELLATION\r\n");
//  }

  if(seedMask == 0)
  {
    AGNSS_PRINT_OUT("Please, specify constellation flag\r\n> ");
  }

  return seedMask;
}

/* Invokes the API to request pass generation */
void AppAGNSS_PassGen(void)
{
  uint32_t gps_time;
  current_timedate_t utc_time;

  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  // Init the Network to set time
  if(httpclient_init() != 0)
  {
    return;
  }

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  // Deinit the Network
  httpclient_deinit();

  utc_time.Year = sDate.Year+2000;
  utc_time.Month = sDate.Month;
  utc_time.Day = sDate.Date;

  utc_time.Hours = sTime.Hours;
  utc_time.Minutes = sTime.Minutes;
  utc_time.Seconds = sTime.Seconds;

  printf("Today's time : %02d:%02d:%02d\n", utc_time.Hours, utc_time.Minutes, utc_time.Seconds);

  gps_time = UTC_ToGPS(&utc_time);
  printf("GPS time [sec] : %ld\n", gps_time);
  GNSS_DATA_PassGen(gps_time);
}

/* Requests the seed (PR o RT) to the remote server. */
int AppAGNSS_DownloadSeed(GNSSParser_Data_t *pGNSSParser_Data, int seedMask, seed_req_t seedReq)
{
  int16_t seedSize;
  int8_t reqLen;
  int ret = 0;

  if(pGNSSParser_Data->pstmpass_data.result == GNSS_OP_OK)
  {
    char rxn_server_res[] = "/rxn-api/locationApi";
    uint8_t auth_string[150/*128*/] = {0};
    uint8_t jsonData[128] = {0};

    /* Start building json payload request */
    (void)A_GNSS_BuildJsonReq(jsonData, (int8_t)seedMask, &seedSize, &reqLen, seedReq);

    /* Add the HTTP authorization header */
    (void)A_GNSS_BuildAuthHeader(pGNSSParser_Data, auth_string);

    /* Start sending request */
    seed_info.jsonBuffer = (uint8_t*)malloc(sizeof(uint8_t)*seedSize);
    memset(seed_info.jsonBuffer, 0, sizeof(uint8_t)*seedSize);
    if(seed_info.jsonBuffer != NULL)
    {
      // Init the Network
      httpclient_init();

      /* Query the server */
      ret = httpclient_rxn(rxn_server_res,
                           (char *)auth_string,
                           seed_info.jsonBuffer,
                           sizeof(uint8_t)*seedSize,
                           (char *)jsonData);

      // Deinit the Network
      httpclient_deinit();

      if(seed_valid(seed_info.jsonBuffer, ret, seedReq) != 0)
      {
        AGNSS_PRINT_OUT("ERROR: Failed to download a valid seed\n\n>");
        /* Free resource */
        free(seed_info.jsonBuffer);
        seed_info.jsonBuffer = NULL;
        return -1;
      }
      else
      {
#ifdef PRINT_JSON
        printJson(seed_info.jsonBuffer);
#endif /* PRINT_JSON */
      }
    }
    else
    {
      AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for seed\n\n>");
      return -1;
    }
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Unable to download a valid seed (Invalid parameters)\n\n>");
    return -1;
  }

  return 0;
}

/* Starts conversion of the seed (Predictive). */
void AppAGNSS_ConvertPRSeed(int seedMask)
{
  size_t num_sats_gps = 0;
  size_t num_sats_glo = 0;
  size_t num_sats_gal = 0;
  size_t num_sats_bei = 0;
  size_t len_seed;

  if(seed_info.jsonBuffer != NULL)
  {
    // get some preliminary info
    decodeJsonLeapsec();

    // GPS Constellation
    if(seedMask & GPS_CONSTELLATION)
    {
      memset(seed_info.block_types, 0, sizeof(seed_info.block_types));
      memset(seed_info.slot_freq, 0, sizeof(seed_info.slot_freq));
      memset(seed_info.time_model, 0, sizeof(seed_info.time_model));

      seed_info.seed = (uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE_5K);
      memset(seed_info.seed, 0, sizeof(uint8_t)*BUF_SIZE_5K);
      if(seed_info.seed != NULL)
      {
        // start decoding step - GPS seed
        seed_info.seed_size = BUF_SIZE_5K;
        len_seed = decodeJsonSeed(GPS_SEED);

        // before ending the decode step, update some values
        num_sats_gps = len_seed / (SINGLE_SEED_BIT_LEN / 8);
        seed_info.nsat = 0;
        seed_info.max_satid = num_sats_gps;
        printf("\n\n*********************num_sats_gps = %d\n\n", num_sats_gps);
        printf("\n\n*********************seed_info.nsat = %d\n\n", seed_info.nsat);
        printf("\n\n*********************seed_info.max_satid = %d\n\n", seed_info.max_satid);

        // end decoding step and propagation
#ifdef PROPAGATE_PR_SEED
        A_GNSS_SendSeed(GPS_SEED, &seed_info);
#endif /*PROPAGATE_PR_SEED*/

        /* Free resource */
        free(seed_info.seed);
        seed_info.seed = NULL;
      }
      else
      {
        AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for GPS seed\n");
      }
    }

    // Glonass constellation
    if(seedMask & GLO_CONSTELLATION)
    {
      memset(seed_info.block_types, 0, sizeof(seed_info.block_types));
      memset(seed_info.slot_freq, 0, sizeof(seed_info.slot_freq));
      memset(seed_info.time_model, 0, sizeof(seed_info.time_model));

      seed_info.seed = (uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE_4K);
      memset(seed_info.seed, 0, sizeof(uint8_t)*BUF_SIZE_4K);
      if(seed_info.seed != NULL)
      {
        // start decoding step - GLO seed
        seed_info.seed_size = BUF_SIZE_4K;
        len_seed = decodeJsonSeed(GLO_SEED);

        // before ending the decode step, update some values
        num_sats_glo = len_seed / (SINGLE_SEED_BIT_LEN / 8);
        seed_info.nsat = num_sats_gps;
        seed_info.max_satid = num_sats_gps + num_sats_glo;
        printf("\n\n*********************num_sats_glo = %d\n\n", num_sats_glo);
        printf("\n\n*********************seed_info.nsat = %d\n\n", seed_info.nsat);
        printf("\n\n*********************seed_info.max_satid = %d\n\n", seed_info.max_satid);

        // end decoding step and propagation
#ifdef PROPAGATE_PR_SEED
        A_GNSS_SendSeed(GLO_SEED, &seed_info);
#endif /*PROPAGATE_PR_SEED*/

        /* Free resource */
        free(seed_info.seed);
        seed_info.seed = NULL;
      }
      else
      {
        AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for GLO seed\n");
      }
    }

    // Galileo constellation
    if(seedMask & GAL_CONSTELLATION)
    {
      memset(seed_info.block_types, 0, sizeof(seed_info.block_types));
      memset(seed_info.slot_freq, 0, sizeof(seed_info.slot_freq));
      memset(seed_info.time_model, 0, sizeof(seed_info.time_model));

      seed_info.seed = (uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE_4K);
      memset(seed_info.seed, 0, sizeof(uint8_t)*BUF_SIZE_4K);
      if(seed_info.seed != NULL)
      {
        // start decoding step - GAL seed
        seed_info.seed_size = BUF_SIZE_4K;
        len_seed = decodeJsonSeed(GAL_SEED);

        // before ending the decode step, update some values
        num_sats_gal = len_seed / (SINGLE_SEED_BIT_LEN / 8);
        seed_info.nsat = num_sats_gps + num_sats_glo;
        seed_info.max_satid = num_sats_gps + num_sats_glo + num_sats_gal;
        printf("\n\n*********************num_sats_gal = %d\n\n", num_sats_gal);
        printf("\n\n*********************seed_info.nsat = %d\n\n", seed_info.nsat);
        printf("\n\n*********************seed_info.max_satid = %d\n\n", seed_info.max_satid);

        // end decoding step and propagation
#ifdef PROPAGATE_PR_SEED
        A_GNSS_SendSeed(GAL_SEED, &seed_info);
#endif /*PROPAGATE_PR_SEED*/

        /* Free resource */
        free(seed_info.seed);
        seed_info.seed = NULL;
      }
      else
      {
        AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for GAL seed\n");
      }
    }

    // Beidou constellation
    if(seedMask & BEI_CONSTELLATION)
    {
      memset(seed_info.block_types, 0, sizeof(seed_info.block_types));
      memset(seed_info.slot_freq, 0, sizeof(seed_info.slot_freq));
      memset(seed_info.time_model, 0, sizeof(seed_info.time_model));

      seed_info.seed = (uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE_4K);
      memset(seed_info.seed, 0, sizeof(uint8_t)*BUF_SIZE_4K);
      if(seed_info.seed != NULL)
      {
        // start decoding step - BEI seed
        seed_info.seed_size = BUF_SIZE_4K;
        len_seed = decodeJsonSeed(BEI_SEED);

        // before ending the decode step, update some values
        num_sats_bei = len_seed / (SINGLE_SEED_BIT_LEN / 8);
        seed_info.nsat = num_sats_gps + num_sats_glo + num_sats_gal;
        seed_info.max_satid = num_sats_gps + num_sats_glo + num_sats_gal + num_sats_bei;
        printf("\n\n*********************num_sats_bei = %d\n\n", num_sats_bei);
        printf("\n\n*********************seed_info.nsat = %d\n\n", seed_info.nsat);
        printf("\n\n*********************seed_info.max_satid = %d\n\n", seed_info.max_satid);

        // end decoding step and propagation
#ifdef PROPAGATE_PR_SEED
        A_GNSS_SendSeed(BEI_SEED, &seed_info);
#endif /*PROPAGATE_PR_SEED*/

        /* Free resource */
        free(seed_info.seed);
        seed_info.seed = NULL;
      }
      else
      {
        AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for BEI seed\n");
      }
    }

    // Free memory
    free(seed_info.jsonBuffer);
    seed_info.jsonBuffer = NULL;
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Unable to convert a NULL seed\n");
  }

  AGNSS_PRINT_OUT("\n\r>");

  return;
}

/* Invokes Almanac decoding API (RT) */
static void decodeAlmanac(seed_type_t seed_type, uint16_t week, current_timedate_t *timedate)
{
  seed_info.stm_alm = (almanac_raw_t *)malloc(sizeof(almanac_raw_t)*64);
  memset(seed_info.stm_alm, 0, sizeof(almanac_raw_t)*64);
  if(seed_info.stm_alm != NULL)
  {
    // Decode Almanac
    A_GNSS_DecodeAlmRT(seed_type, &seed_info, week, timedate);
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for decoded ALM\n");
  }
}

/* Starts Almanac decoding (RT) */
static void handleAlmConversion(seed_type_t seed_type, uint16_t seed_size, uint16_t week, current_timedate_t *timedate)
{
  seed_info.alm_seed = (uint8_t*)malloc(sizeof(uint8_t)*seed_size);
  (void)memset(seed_info.alm_seed, 0, sizeof(uint8_t)*seed_size);
  if(seed_info.alm_seed != NULL)
  {
    seed_info.alm_seed_size = seed_size;

    // Initial json parsing for ALM
    decodeJsonAlm(seed_type);

    // Prepare to decode Almanac
    decodeAlmanac(seed_type, week, timedate);

    // Free alm seed resource
    free(seed_info.alm_seed);
    seed_info.alm_seed = NULL;
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for RT ALM seed\n");
  }

}

/* Invokes Ephemeris decoding API (RT) */
static uint16_t decodeEphemeris(seed_type_t seed_type, current_timedate_t *timedate)
{
  uint16_t week = 0;

  seed_info.stm_ephem = (ephemeris_raw_t *)malloc(sizeof(ephemeris_raw_t)*64);
  memset(seed_info.stm_ephem, 0, sizeof(ephemeris_raw_t)*64);
  if(seed_info.stm_ephem != NULL)
  {    
    // Decode Ephemeris
    week = A_GNSS_DecodeEphemRT(seed_type, &seed_info, timedate);
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for decoded EPH\n");
  }

  return week;
}

/* Starts Ephemeris decoding (RT) */
static uint16_t handleEphConversion(seed_type_t seed_type, uint16_t seed_size, current_timedate_t *timedate)
{
  uint16_t week = 0;

  seed_info.eph_seed = (uint8_t*)malloc(sizeof(uint8_t)*seed_size);
  memset(seed_info.eph_seed, 0, sizeof(uint8_t)*seed_size);
  if(seed_info.eph_seed != NULL)
  {
    seed_info.eph_seed_size = seed_size;

    if(seed_type == GLO_SEED)
    {
      seed_info.eph_nkc_seed = (uint8_t*)malloc(sizeof(uint8_t)*seed_size);
      (void)memset(seed_info.eph_nkc_seed, 0, sizeof(uint8_t)*seed_size);
      if(seed_info.eph_nkc_seed != NULL)
      {
        seed_info.eph_nkc_seed_size = seed_size;

        // Initial json parsing for EPH
        decodeJsonEph(seed_type);

        // Prepare to decode Ephemeris
        week = decodeEphemeris(seed_type, timedate);

        // Free eph nkc seed resource
        free(seed_info.eph_nkc_seed);
        seed_info.eph_nkc_seed = NULL;
      }
      else
      {
        AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for RT NKC seed\n");
      }
    }
    else
    {
      // Initial json parsing for EPH
      decodeJsonEph(seed_type);

      // Prepare to decode Ephemeris
      week = decodeEphemeris(seed_type, timedate);
    }

    // Free eph seed resource
    free(seed_info.eph_seed);
    seed_info.eph_seed = NULL;
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Failed to alloc memory for RT EPH seed\n");
  }

  return week;
}

/* Invokes API to send decoded ephemeris and almanac to the Teseo device (RT) */
static void sendRTSeed(seed_type_t seed_type)
{
  // Send $PSTMEPHEM command
  AGNSS_PRINT_OUT("Send $PSTMEPHEM command...\n");
  A_GNSS_SendEphemRT(seed_type, &seed_info);
  // Send $PSTMALMANAC command
  AGNSS_PRINT_OUT("Send $PSTMALMANAC command...\n");
  A_GNSS_SendAlmRT(seed_type, &seed_info);
}

/* Clean utility */
static void clearDecodingBuffer()
{
  if(seed_info.stm_ephem != NULL)
  {
    free(seed_info.stm_ephem);
    seed_info.stm_ephem = NULL;
  }
  if(seed_info.stm_alm != NULL)
  {
    free(seed_info.stm_alm);
    seed_info.stm_alm = NULL;
  }
}

/* Starts seed conversion (Real-time). */
void AppAGNSS_ConvertRTSeed(int seedMask)
{
  current_timedate_t timedate;
  uint16_t week;

  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  if(seed_info.jsonBuffer != NULL)
  {

    // Get time and send $PSTMINITTIME command
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    timedate.Year = sDate.Year+2000;
    timedate.Month = sDate.Month;
    timedate.Day = sDate.Date;

    timedate.Hours = sTime.Hours;
    timedate.Minutes = sTime.Minutes;
    timedate.Seconds = sTime.Seconds;

//    printf("Today's time : %02d %02d %04d %02d:%02d:%02d\n",
//           timedate.Day,
//           timedate.Month,
//           timedate.Year+2000,
//           timedate.Hours, timedate.Minutes, timedate.Seconds);
    A_GNSS_SendCurrentTime(&timedate);

    // GPS Constellation
    if(seedMask & GPS_CONSTELLATION)
    {
      week = handleEphConversion(GPS_SEED, BUF_SIZE_3K, &timedate);

      handleAlmConversion(GPS_SEED, BUF_SIZE_2K, week, &timedate);
#ifdef PROPAGATE_RT_SEED
      sendRTSeed(GPS_SEED);
#endif /*PROPAGATE_RT_SEED*/

      clearDecodingBuffer();
    }
    // GLO Constellation
    else if(seedMask & GLO_CONSTELLATION)
    {
      week = handleEphConversion(GLO_SEED, BUF_SIZE_2K, &timedate);

      handleAlmConversion(GLO_SEED, BUF_SIZE_2K, week, &timedate);
#ifdef PROPAGATE_RT_SEED
      sendRTSeed(GLO_SEED);
#endif /*PROPAGATE_RT_SEED*/

      clearDecodingBuffer();
    }
    // GAL Constellation
    else if(seedMask & GAL_CONSTELLATION)
    {
      week = handleEphConversion(GAL_SEED, BUF_SIZE_2K, &timedate);

      handleAlmConversion(GAL_SEED, BUF_SIZE_1K, week, &timedate);
#ifdef PROPAGATE_RT_SEED
      sendRTSeed(GAL_SEED);
#endif /*PROPAGATE_RT_SEED*/

      clearDecodingBuffer();
    }
    // BEI Constellation
    else if(seedMask & BEI_CONSTELLATION)
    {
      week = handleEphConversion(BEI_SEED, BUF_SIZE_2K, &timedate);

      handleAlmConversion(BEI_SEED, BUF_SIZE_1K, week, &timedate);
#ifdef PROPAGATE_RT_SEED
      sendRTSeed(BEI_SEED);
#endif /*PROPAGATE_RT_SEED*/

      clearDecodingBuffer();
    }

    // Free memory
    free(seed_info.jsonBuffer);
    seed_info.jsonBuffer = NULL;
  }
  else
  {
    AGNSS_PRINT_OUT("ERROR: Unable to convert a NULL seed\n");
  }

  AGNSS_PRINT_OUT("\n\r>");

  return;
}

