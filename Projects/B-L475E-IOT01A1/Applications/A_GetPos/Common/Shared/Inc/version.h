/**
  ******************************************************************************
  * @file    version.h
  * @author  MCD Application Team
  * @brief   STM32 FW version definition
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
#ifndef __version_H
#define __version_H

#define FWVERSION_NAME_SIZE   20
#define FWVERSION_DATE_SIZE   16
#define FWVERSION_TIME_SIZE   9
   

/** Firmware version information.
 * @note  For user information.
 */
typedef struct {
  char name[FWVERSION_NAME_SIZE];
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
  char *packaged_date;
} firmware_version_t;


#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "28-March-2018 02:18:56 PM"

#ifdef AWS
#define FW_VERSION_NAME   "X-CUBE-AWS"
#define AWS_VERSION	TBD
#endif

#ifdef AZURE
#define FW_VERSION_NAME   "X-CUBE-AZURE"
#define AZURE_VERSION TBD

#endif

#ifdef BLUEMIX
#define FW_VERSION_NAME   "X-CUBE-WATSON-X"
#define BLUEMIX_VERSION TBD

#endif

#if defined(GENERICMQTT) || defined(EXOSITEHTTP) || defined(HTTPCLIENT) || defined(GROVESTREAMS) || defined(UBIDOTS)
#define FW_VERSION_NAME   "X-CUBE-CLD-GEN"
#define LITMUS_VERSION TBD
#endif


#endif /* __version_H */

