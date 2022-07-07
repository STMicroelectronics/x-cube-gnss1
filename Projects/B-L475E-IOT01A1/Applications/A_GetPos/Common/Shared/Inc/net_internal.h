/**
  ******************************************************************************
  * @file    net_internal.h
  * @author  MCD Application Team
  * @brief   Network abstraction at transport layer level. Internal definitions.
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
#ifndef __NET_INTERNAL_H__
#define __NET_INTERNAL_H__

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>   /* atoi() */
#include <stdbool.h>
#include "main.h"
#include "msg.h"
#include "net.h"

#ifdef USE_MBED_TLS

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/timing.h"
#include "mbedtls_net.h"  /* mbedTLS data callbacks, implemented on WiFi_LL */
#include "heap.h"         /* memory allocator overloading */

#endif /* USE_MBED_TLS */


#ifdef USE_WIFI
#include "wifi.h"
#endif

#ifdef USE_C2C
#include "c2c.h"
#endif

#ifdef USE_LWIP
#include "lwip/netif.h"
#endif /* USE_LWIP_ETH */

/* Private defines -----------------------------------------------------------*/
#define NET_DEFAULT_BLOCKING_WRITE_TIMEOUT  2000
#define NET_DEFAULT_BLOCKING_READ_TIMEOUT   2000
#define NET_DEFAULT_BLOCKING                true


/* Private typedef -----------------------------------------------------------*/
typedef struct net_ctxt_s net_ctxt_t;
typedef struct net_sock_ctxt_s net_sock_ctxt_t;

typedef int net_sock_create_t(net_hnd_t nethnd, net_sockhnd_t * sockhnd, net_proto_t proto);
typedef int net_sock_open_t(net_sockhnd_t sockhnd, const char * hostname, int remoteport, int localport);
typedef int net_sock_recv_t(net_sockhnd_t sockhnd, uint8_t * const buf, size_t len);
typedef int net_sock_recvfrom_t(net_sockhnd_t sockhnd, uint8_t * const buf, size_t len, net_ipaddr_t * remoteaddress, int * remoteport);
typedef int net_sock_send_t(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len);
typedef int net_sock_sendto_t(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len,  net_ipaddr_t * remoteaddress, int remoteport);
typedef int net_sock_close_t(net_sockhnd_t sockhnd);
typedef int net_sock_destroy_t(net_sockhnd_t sockhnd);

typedef struct {
  net_sock_open_t     * open;
  net_sock_recv_t     * recv;
  net_sock_recvfrom_t * recvfrom;
  net_sock_send_t     * send;
  net_sock_sendto_t   * sendto;
  net_sock_close_t    * close;
  net_sock_destroy_t  * destroy;
} net_sock_methods_t;

#ifdef USE_MBED_TLS
typedef struct {
  unsigned char * tls_ca_certs; /**< Socket option. */
  unsigned char * tls_ca_crl;   /**< Socket option. */
  unsigned char * tls_dev_cert; /**< Socket option. */
  unsigned char * tls_dev_key;  /**< Socket option. */
  uint8_t * tls_dev_pwd;        /**< Socket option. */
  size_t tls_dev_pwd_len;       /**< Socket option / meta. */
  bool tls_srv_verification;    /**< Socket option. */
  char * tls_srv_name;          /**< Socket option. */
  /* mbedTLS objects */
  mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	uint32_t flags;
	mbedtls_x509_crt cacert;
	mbedtls_x509_crl cacrl;               /** Optional certificate revocation list */
	mbedtls_x509_crt clicert;
	mbedtls_pk_context pkey;
} net_tls_data_t;
#endif /* USE_MBED_TLS */

/** Network socket context. */
struct net_sock_ctxt_s {
  net_ctxt_t * net;                     /**< Network interface to which the socket is attached. */
  net_sock_ctxt_t * next;               /**< Linear linked list (not circular) of the sockets opened on the same network interface. */
  net_sock_methods_t methods;           /**< Proto-specific function pointers. */
  net_proto_t proto;                    /**< Socket type. */
  bool blocking;                        /**< Socket option. */
  uint16_t read_timeout;                /**< Socket option. */
  uint16_t write_timeout;               /**< Socket option. */
#ifdef USE_MBED_TLS
  net_tls_data_t * tlsData;             /**< TLS specific context. */
#endif  /* USE_MBED_TLS */
  net_sockhnd_t underlying_sock_ctxt;   /**< Socket context of the underlying software layer. */
  int localport;                        /**< Local port number binding. Used by UDP sockets. */
};

/** Network interface context. */
struct net_ctxt_s {
  net_if_t itf;
  net_sock_ctxt_t * sock_list;  /**< Linked list of the sockets opened on the network interface. */
#ifdef USE_LWIP
  struct netif lwip_netif;       /**< LwIP interface context. */
#endif /* USE_LWIP */
};

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) < (b) ? (b) : (a))

#define net_malloc(a) malloc((a))
#define net_free(a)   free((a))

int32_t net_timeout_left_ms(uint32_t init, uint32_t now, uint32_t timeout);
#ifdef USE_MBED_TLS
extern int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
#endif /* USE_MBED_TLS */

#endif /* __NET_INTERNAL_H__ */

