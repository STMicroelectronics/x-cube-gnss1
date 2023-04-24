/**
  ******************************************************************************
  * @file    http_lib.h
  * @author  MCD Application Team
  * @brief   Helper functions for sending HTTP requests
  *          Header for http_lib.c
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
#ifndef __http_lib_H
#define __http_lib_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* to be aligned with net.h return codes */
#define  HTTP_OK          0   /**< Success */
#define  HTTP_ERR        -1   /**< Internal error */
#define  HTTP_PARAM      -2   /**< Wrong parameters, or wrong state. */
#define  HTTP_TIMEOUT    -3   /**< Timeout rearched during a blocking operation. */
#define  HTTP_EOF        -4   /**< Connection dropped during the operation. */
#define  HTTP_NOT_FOUND  -5   /**< The remote host could not be reached. */
#define  HTTP_AUTH       -6   /**< The remote host could not be authentified. */

#define MIN(a,b)        ( ((a)<(b)) ? (a) : (b) )
   
typedef void * http_handle_t;
typedef void * http_sock_handle_t;

typedef enum {
  HTTP_PROTO_NONE = 0,
  HTTP_PROTO_HTTP,
  HTTP_PROTO_HTTPS
} http_proto_t;

typedef enum {
  HTTP_REQUEST_NONE = 0,
  HTTP_REQUEST_GET,
  HTTP_REQUEST_POST,
  HTTP_REQUEST_PUT,
  HTTP_REQUEST_DELETE
} http_request_t;

/**
 * @brief   Create an HTTP session.
 * @note    The internal session context is allocated by the callee.
 * @param   In: pHnd      Pointer on the session handle.
 * @param   In: host      target host. Ex: www.host.com
 * @param   In: port      target port. Ex: 80
 * @param   In: protocol  plain or secure connection (TLS)
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_create_session(http_handle_t * const pHnd, const char *host, int port, http_proto_t protocol);
/**
 * @brief   Open an HTTP session.
 * @note    The internal session context is allocated by the callee.
 * @param   In: Hnd       session handle.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_connect(http_handle_t Hnd);
/**
 * @brief   Close an HTTP session.
 * @note    The internal session context is freed by the callee.
 * @param   In: hnd   Session handle.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_close(const http_handle_t hnd);

/**
 * @brief   Write beginning of a request in the provided buffer.
 *          It will be sent with http_send().
 *          It can be followed to calls to http_add_header() and http_add_body()
 * @param   In:       type        request type (GET, POST ...)
 * @param   In:       host        target host name (it will be used as HTTP header Host: hostname.com)
 * @param   In:       resource    resource (path with leading slash. Ex: "/path/to/resource.html")
 * @param   In:       buffer      buffer provided by user.
 * @param   In:       length      buffer size.
 * @retval  >0 Header size
 *          <0 Error
 */
int http_create_request(http_request_t request_type, char * host, char * resource, uint8_t * buffer, uint32_t length);

/**
 * @brief   Add an HTTP header to a request created in the provided buffer.
 * @note    Must be called before http_add_body()
 * @note    Can be called several times
 * @param   In:       buffer      HTTP buffer provided by user. Must contain header and end with double CR-LF.
 * @param   In:       length      buffer size.
 * @param   In:       header name
 * @param   In:       header value
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_add_header(uint8_t * buffer, uint32_t buffer_length, char * header, char * value);

/**
 * @brief   Add body part to a request created in the provided buffer.
 * @note    All the headers must have been added before calling this function.
 * @note    Can be called zero or one time.
 * @param   In:       buffer             full buffer provided by user (with header).
 * @param   In:       buffer length      buffer size (large enough for headers and body).
 * @param   In:       body               pointer to body part to be copied in buffer.
 * @param   In:       body length        body size.
 * @retval  returns total length (header+body) or HTTP_ERR (<0) in case of error
 */
int http_add_body(uint8_t * buffer, uint32_t buffer_length, uint8_t *body, uint32_t body_length);

/**
 * @brief   Send a request.
 * @param   In:       Handle      handle to session created and connected.
 * @param   In:       buffer      full buffer provided by user (prepared with header and body).
 * @param   In:       buffer      buffer size.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_send(http_handle_t hnd, uint8_t * buffer, uint32_t buffer_length);

/**
 * @brief   Receive data from server.
 * @param   In:       Handle      Session handle
 * @param   In/Out:   buffer      buffer to receive data from server.
 * @param   In:       length      buffer size.
 * @retval  size of received data
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_recv(http_handle_t hnd, uint8_t * buffer, uint32_t length);

/**
 * @brief   Receive full response to a request.
 *          This loops until it has received the amount of data indicated
 *          in Content-Length: header
 *          or the supplied response buffer is full.
 * @param   In:       Handle      
 * @param   Out:      buffer      response buffer (empty).
 * @param   In:       buffer_length      buffer size.
 * @retval  size of received data
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_recv_response(http_handle_t hnd, uint8_t * buffer, uint32_t buffer_length);
int http_recv_chunked(http_handle_t hnd, uint8_t * buffer, uint32_t length, int *http_status);

/**
 * @brief   Set network socket option for an HTTP session.
 *          Used mainly for TLS options ( "tls_ca_certs" ).
 *          Must be done after create_session() and before connect().
 * @param   In: hnd   Session handle.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_sock_setopt(http_handle_t Hnd, const char * optname, const uint8_t * optbuf, size_t optlen);
/**
 * @brief   Send an HTTP GET request.
 * @param   In:   hnd             Handle created by http_create_session().
 * @param   In:       query           HTTP query string ( "/path/to/request.html" ).
 *                                    Must begin with '/'.
 * @param   In:   additional_headers  HTTP headers (each line ending with CRLF)
 * @param   In:   readbuffer      pointer to read buffer filled with server response to GET request (with header and body).
 * @param   In:   size            size of read buffer.
 * @retval  Error code
 *            >=0              Success, length of the received data.
 *            HTTP_ERR (<0)   Failure
 */
int http_get(const http_handle_t hnd, const char * query, const char * additional_headers, uint8_t *readbuffer, uint32_t readbuffer_size);
/**
 * @brief   Send an HTTP POST request.
 * @param   In:       hnd             HTTP Handle created by http_open().
 * @param   In:       query           HTTP query string ( "/path/to/request.html" ).
 *                                    Must begin with '/'.
 * @param   In:       additional_headers  HTTP headers ("name: value", each line ending with \r\n)
 * @param   In:       postbuffer          pointer to data to be sent with POST request
 * @param   In:       postbuffer_size     length of data of POST request.
 * @param   In:       responsebuffer        pointer to buffer for POST response
 * @param   In:       responsebuffer_size   max size of data of POST response.
 * @retval  Error code
 *            >0              Success, length of the received data.
 *            HTTP_ERR (<0)   Failure
 */
int http_post(const http_handle_t hnd, const char * query, const char * additional_headers, const uint8_t *postbuffer, uint32_t postbuffer_size,
              uint8_t *responsebuffer, uint32_t responsebuffer_size);
/**
 * @brief   Send an HTTP PUT request.
 * @param   In:       hnd             HTTP Handle created by http_open().
 * @param   In:       resource        HTTP resource string ( "/path/to/resource.html" ).
 *                                    Must begin with '/'.
 * @param   In:       additional_headers  HTTP headers (each line ending with \r\n)
 * @param   In:       putbuffer          pointer to data to be sent with PUT request
 * @param   In:       putbuffer_size     length of data of PUT request.
 * @param   In:       responsebuffer        pointer to buffer for response
 * @param   In:       responsebuffer_size   max size of response.
 * @retval  Error code
 *            >0              Success, length of the received data.
 *            HTTP_ERR (<0)   Failure
 */
int http_put(const http_handle_t hnd, const char * query, const char * additional_headers, const uint8_t *putbuffer, uint32_t putbuffer_size,
              uint8_t *responsebuffer, uint32_t responsebuffer_size);
/**
 * @brief   Send an HTTP DELETE request.
 * @param   In:       hnd             HTTP Handle created by http_open().
 * @param   In:       query           HTTP resource path ( "/path/to/resource.html" ).
 *                                    Must begin with '/'.
 * @param   In:       additional_headers  HTTP headers (each line ending with \r\n)
 * @param   In:       responsebuffer        pointer to buffer for POST response
 * @param   In:       responsebuffer_size   max size of data of POST response.
 * @retval  Error code
 *            >0              Success, length of the received data.
 *            HTTP_ERR (<0)   Failure
 */
int http_delete(const http_handle_t hnd, const char * resource, const char * additional_headers,
              uint8_t *responsebuffer, uint32_t responsebuffer_size);

/**
 * @brief   check if a connection is open.
 * @param   In:       hnd             HTTP Handle created by http_open().
 * @retval  bool
 *            >0              true, connection is open.
 *            0               False, connection is not open
 */
bool http_is_open(const http_handle_t hnd);
/**
 * @brief   Find the start of body data in HTTP buffer.
 * @param   In:       buffer      HTTP buffer.
 * @param   In/Out:   length      In: buffer size. Out: body size.
 * @retval  !=NULL    pointer to body part of HTTP buffer.
 *            NULL    Body part was not found.
 */
uint8_t * http_find_body(uint8_t * http_message, uint32_t *length);
uint8_t * http_find_chunk(uint8_t * http_message, uint32_t *length, int *chunk_len);

/**
 * @brief   Get the HTTP response status in headers.
 * @param   In:       buffer      buffer containing headers.
 * @param   In:       length      buffer size.
 * @retval  Error code
 *            >0              HTTP status ( 200 == OK, 404 == not found, ... ).
 *            HTTP_ERR (<0)   Failure
 */
uint32_t http_response_status(uint8_t * http_response, uint32_t length);
/**
 * @brief   Validate the HTTP response.
 * @param   In:       buffer      buffer containing headers.
 * @param   In:       length      buffer size.
 * @retval  Error code
 *            >0              ok.
 *            HTTP_ERR (<0)   Failure
 */
int http_response_validate(uint8_t * http_response, uint32_t length);
/**
 * @brief   Get the value of Content-Length: in headers.
 * @param   In:       buffer      buffer containing headers.
 * @param   In:       length      buffer size.
 * @retval  Error code
 *            >0              content length.
 *            HTTP_ERR (<0)   Failure
 */
uint32_t http_content_length(uint8_t * buffer, uint32_t length);
int http_transfer_is_chunked(uint8_t * buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __http_lib_H */

/**
  * @}
  */

