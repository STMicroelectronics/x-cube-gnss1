/**
  ******************************************************************************
  * @file    mbedtls_net.h
  * @author  MCD Application Team
  * @brief   MbedTLS network callbacks declaration.
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
#ifndef __MBEDTLS_NET_H__
#define __MBEDTLS_NET_H__

#include "mbedtls/ssl.h"  /* Some return codes are defined in ssl.h, so that the functions below may be used as mbedtls callbacks. */

int  mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len);
int  mbedtls_net_recv_blocking(void *ctx, unsigned char *buf, size_t len, uint32_t timeout);
int  mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len);

#endif /* __MBEDTLS_NET_H__ */

