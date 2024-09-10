/**
  ******************************************************************************
  * @file    app_gnss.h
  * @author  SRA Application Team
  * @brief   Header file for app_gnss.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_GNSS_H
#define APP_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/
void MX_GNSS_Init(void);
void MX_GNSS_PreOSInit(void);
void MX_GNSS_PostOSInit(void);

#ifdef __cplusplus
}
#endif
#endif /* APP_GNSS_H */
