/**
  ******************************************************************************
  * @file    mbedtls_net.c
  * @author  MCD Application Team
  * @brief   Mbedtls network porting layer. Callbacks implementation.
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
#include "mbedtls_net.h"
#include "mbedtls/ssl.h"
#include "net.h"
#include "msg.h"
#include <string.h>

/* Non-blocking interface implementation. Timeout is not applicable. */
int mbedtls_net_recv(void *ctx, unsigned char *buf, size_t len)
{
  int ret = net_sock_recv((net_sockhnd_t) ctx, buf, len);
  
  if (ret > 0)
  {
    return ret;
  }
  
  if(ret == 0)
  {
    return  MBEDTLS_ERR_SSL_WANT_READ; 
  }
  
  msg_error("mbedtls_net_recv(): error %d in net_sock_recv() - requestedLen=%d\n", ret, len);
 
  return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}


/* Blocking interface implementation.*/
int mbedtls_net_recv_blocking(void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
  int ret = 0;
  char stimeout[8];
  
  if ((sizeof(stimeout) - 1) <= snprintf(stimeout, sizeof(stimeout), "%lu",timeout))
  {
    msg_error("mbedtls_net_recv_blocking(): out of range timeout %lu\n", timeout);
    return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
  }
  
  if (net_sock_setopt((net_sockhnd_t) ctx, "sock_read_timeout", (uint8_t *)stimeout, strlen(stimeout) + 1) == NET_OK)
  {
    ret = net_sock_recv((net_sockhnd_t) ctx, buf, len);
  
    if (ret > 0)
    {
      return ret;
    }
    else
    {
      switch(ret)
      {
        case 0:
          return MBEDTLS_ERR_SSL_WANT_READ; 
        case NET_TIMEOUT:
          /* According to mbedtls headers, MBEDTLS_ERR_SSL_TIMEOUT should be returned. */
          /* But it saturates the error log with false errors. By contrast, MBEDTLS_ERR_SSL_WANT_READ does not raise any error. */
          return MBEDTLS_ERR_SSL_WANT_READ;
        default:
          ;
      }
    }
  }
  
  msg_error("mbedtls_net_recv_blocking(): error %d in net_sock_recv() - requestedLen=%d\n", ret, len);
  return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}


int mbedtls_net_send(void *ctx, const unsigned char *buf, size_t len)
{
  int ret = net_sock_send((net_sockhnd_t) ctx, buf, len);
  
  if (ret > 0)
  {
    return ret;
  }
  
  if(ret == 0)
  {
    return  MBEDTLS_ERR_SSL_WANT_WRITE; 
  }
  msg_error("mbedtls_net_send(): error %d in net_sock_send() - requestedLen=%d\n", ret, len);
  
  /* TODO: The underlying layers do not allow to distinguish between
   *          MBEDTLS_ERR_SSL_INTERNAL_ERROR,
   *          MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY,
   *          MBEDTLS_ERR_SSL_CONN_EOF.
   *  Most often, the error is due to the closure of the connection by the remote host. */ 
  
  return MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY;
  
}

