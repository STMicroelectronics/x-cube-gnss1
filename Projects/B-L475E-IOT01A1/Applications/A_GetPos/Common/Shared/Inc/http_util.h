/**
  ******************************************************************************
  * @file    http_util.h
  * @author  MCD Application Team
  * @brief   Helper functions for building HTTP GET and POST requests, and 
  *          stream reading.
  *          Header for http_util.c
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
#ifndef __http_util_H
#define __http_util_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define HTTP_OK                0
#define HTTP_ERR               -1
#define HTTP_ERR_HTTP          -2  /**< HTTP error */
#define HTTP_ERR_CLOSED        -3  /**< The HTTP connection was closed by the server. */

#define FWVERSION_NAME_SIZE   20
#define FWVERSION_DATE_SIZE   16
#define FWVERSION_TIME_SIZE   9
   
typedef void * http_handle_t;
typedef void * http_sock_handle_t;

/** HTTP progressive download progress status */
typedef struct {
  int first_byte;           /**< File position of the first byte of the last downloaded chunk. Starting at 0. */
  int last_byte;            /**< File position of the last byte of the last downloaded chunk. Starting at 0. */
  int resource_size;        /**< File/resource length (in bytes). */
  bool connection_is_open;  /**< HTTP keep-alive connection status. If false, the http_handle must be closed and re-opened. */
} http_range_status_t;

typedef enum {
  HTTP_PROTO_NONE = 0,
  HTTP_PROTO_HTTP,
  HTTP_PROTO_HTTPS
} http_proto_t;

int http_open(http_handle_t * const pHnd, const char *url);
int http_close(const http_handle_t hnd);
bool http_is_open(const http_handle_t hnd);
int http_read(uint8_t * const readbuffer, http_range_status_t * const status, const size_t offset, const size_t size,
              const char * const extra_headers, const uint8_t * const post_buf, const size_t post_buf_size,
              const http_handle_t hnd);

int http_url_parse(char * const host, const int host_max_len, int * const port,  bool * tls, char * const query, const int query_max_len, const char * url);
int http_req_create(char ** const req_buf , const char * const query, const char * const hostname, const size_t offset, const size_t size, const char * const extra_headers, const size_t post_buf_size);
void http_req_destroy(const char *req_buf);

int http_sock_open(http_sock_handle_t * const http_sock_hnd, const char *url);
int http_sock_close(const http_sock_handle_t http_sock_hnd);
int http_sock_rcv(uint8_t *const readbuffer, const int size, const http_sock_handle_t http_sock_hnd);
int http_sock_get_resource_size(const http_sock_handle_t http_sock_hnd);
#ifdef __cplusplus
}
#endif

#endif /* __http_util_H */

/**
  * @}
  */

