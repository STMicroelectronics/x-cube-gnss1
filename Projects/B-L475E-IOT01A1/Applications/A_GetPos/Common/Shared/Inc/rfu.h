/**
  ******************************************************************************
  * @file    rfu.h
  * @author  MCD Application Team
  * @brief   Remote firmware update over TCP/IP.
  *          Header for rfu.c file
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
#ifndef __rfu_H
#define __rfu_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define RFU_OK                0
#define RFU_ERR               -1
#define RFU_ERR_HTTP          -2  /**< HTTP error */
#define RFU_ERR_FF            -3  /**< IAR simple file format error */
#define RFU_ERR_FLASH         -4  /**< FLASH erase or programming error */
#define RFU_ERR_OB            -5  /**< Option bytes programming error */
#define RFU_ERR_HTTP_CLOSED   -6  /**< The HTTP connection was closed by the server. */

int rfu_update(const char * const url);

#ifdef __cplusplus
}
#endif

#endif /* __rfu_H */

/**
  * @}
  */


