/**
  ******************************************************************************
  * @file    a_gnss.h
  * @author  SRA
  * @brief   Provides high level API for Assisted GNSS
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
#ifndef A_GNSS_H
#define A_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup A_GetPos
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "gnss_parser.h"
#include "assisted_gnss.h"

/* Defines -------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup A_GetPos_PUBLIC_FUNCTIONS PUBLIC FUNCTIONS
 * @{
 */

/**
 * @brief  Parse command option for constellationspecificied by the user;
 * @param  cmd The command to be parsed
 * @retval Constellation mask
 */
int AppAGNSS_ParseSeedOp(char *cmd);

/**
 * @brief  Prepare the Password generation process
 * @retval None
 */
void AppAGNSS_PassGen(void);

/**
 * @brief  Request the seed to the remote provider
 * @param  pGNSSParser_Data Handler of the GNSS data
 * @param  seedMask Constellation mask
 * @param  seedReq Type of Assisted GNSS (PR/RT)
 * @retval 0 on success -1 otherwise
 */
int AppAGNSS_DownloadSeed(GNSSParser_Data_t *pGNSSParser_Data, int seedMask, seed_req_t seedReq);

/**
 * @brief  Start process for seed conversion (PR case)
 * @param  seedMask Constellation mask
 * @retval None
 */
void AppAGNSS_ConvertPRSeed(int seedMask);

/**
 * @brief  Start process for seed conversion (RT case)
 * @param  seedMask Constellation mask
 * @retval None
 */
void AppAGNSS_ConvertRTSeed(int seedMask);

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

#endif /* A_GNSS_H */

