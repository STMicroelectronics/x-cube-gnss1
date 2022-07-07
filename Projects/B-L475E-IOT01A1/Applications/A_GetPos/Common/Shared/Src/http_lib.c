/**
  ******************************************************************************
  * @file    http_lib.c
  * @author  MCD Application Team
  * @brief   HTTP Client library
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
#include <string.h>
#include "main.h"
#include "http_lib.h"
#include "msg.h"
#include "net.h"

/* Private defines -----------------------------------------------------------*/
#define HTTP_MAX_HOST_SIZE        80      /**< Max length of the http server hostname. */
#define HTTP_MAX_QUERY_SIZE       50      /**< Max length of the http query string. */
#define HTTP_BUFFER_SIZE          1050    /**< Size of the HTTP work buffer. */
#define HTTP_VERSION              "1.1"
#define HTTP_HEADER               "HTTP/"

#define MIN(a,b)        ( ((a)<(b)) ? (a) : (b) )

#define http_malloc malloc
#define http_free   free

/* Private typedef -----------------------------------------------------------*/
/** 
 * @brief HTTP internal session context. 
 */
typedef struct {
  net_sockhnd_t sock;                 /**< Network socket handle. */
  char hostname[HTTP_MAX_HOST_SIZE];  /**< HTTP server name. */
  int port;                           /**< HTTP port */
  http_proto_t protocol;
  char * certificate;
  bool tls_verify_server;
  char query[HTTP_MAX_QUERY_SIZE];    /**< HTTP query parsed from the URL. */
  bool connection_is_open;            /**< connection status. */
  uint8_t buffer[HTTP_BUFFER_SIZE];   /**< work buffer  */
} http_context_t;

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t * http_find_headers(uint8_t * http_message, uint32_t len);

int http_recv_payload(http_handle_t hnd,
                      uint8_t *buffer,
                      uint32_t buffer_length,
                      uint8_t *recv_buf,
                      uint32_t recv_buf_len,
                      int content_length,
                      int recv_len);

int http_recv_chunks(http_handle_t hnd,
                     uint8_t *buffer,
                     uint32_t buffer_length,
                     uint8_t *recv_buf,
                     uint32_t recv_buf_len,
                     int recv_len);

int http_append_chunk(http_handle_t hnd,
                      uint8_t *dst_buffer,
                      uint32_t *dst_offset,
                      uint32_t dst_length,
                      uint8_t *src_buffer,
                      uint32_t src_length);

/* Functions Definition ------------------------------------------------------*/

int http_create_session(http_handle_t * const pHnd, const char *host, int port, http_proto_t protocol)
{
  http_context_t * pCtx = 0;
  int ret = 0;

  pCtx = (http_context_t *) http_malloc(sizeof(http_context_t));
  if (!pCtx)
  {
    msg_error("error malloc pCtx");
    return HTTP_ERR;
  }

  memset(pCtx, 0, sizeof(http_context_t));
  strncpy(pCtx->hostname, host, sizeof(pCtx->hostname));
  pCtx->port = port;
  pCtx->protocol = protocol;
  pCtx->connection_is_open = false;

  bool tls = (pCtx->protocol==HTTP_PROTO_HTTPS);
  
  ret = net_sock_create(hnet, &pCtx->sock, (tls == true) ? NET_PROTO_TLS : NET_PROTO_TCP);
  if (NET_OK != ret)
  {
    msg_error("Could not create a socket.\n");
    return HTTP_ERR;
  }
  
  *pHnd = (http_handle_t) pCtx;
  return HTTP_OK;
}

int http_connect(http_handle_t Hnd)
{
  http_context_t * pCtx = (http_context_t *)Hnd;
  int ret = 0;

  if (!pCtx)
  {
    msg_error("error invalid handle");
    return HTTP_ERR;
  }

  ret = net_sock_open(pCtx->sock, (const char *) pCtx->hostname, pCtx->port, 0);
  if (NET_OK != ret)
  {
    msg_error("Could not open a socket.\n");
  }
  else
  {
    pCtx->connection_is_open = true;
    return HTTP_OK;
  }

  return HTTP_ERR;
}


int http_close(const http_handle_t hnd)
{
  int rc = HTTP_ERR;
  
  http_context_t * pCtx = (http_context_t *) hnd;
  if (pCtx != NULL)
  {
    int ret = 0;
    ret = net_sock_close(pCtx->sock);
    pCtx->connection_is_open = false;
    ret |= net_sock_destroy(pCtx->sock);
    if (ret == NET_OK)
    {
      rc = HTTP_OK;
      http_free(pCtx);
    }
    else
    {
      msg_error("Could not close and destroy a socket.\n");
    }
  }
  
  return rc;
}


int http_sock_setopt(http_handle_t hnd, const char * optname, const uint8_t * optbuf, size_t optlen)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  
  return net_sock_setopt(pCtx->sock, optname, optbuf, optlen);
}


int http_create_request(http_request_t request_type, char * host, char * resource, uint8_t * buffer, uint32_t length)
{
  int remaining_size = length;
  int printed_size = 0;
  uint8_t * p = buffer;
  
  switch(request_type)
  {
  case HTTP_REQUEST_GET:
    {
      printed_size = snprintf((char *)p, remaining_size, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
      p += printed_size;
      remaining_size -= printed_size;
    }
    break;
  case HTTP_REQUEST_POST:
    {
    printed_size = snprintf((char *)p, remaining_size, "POST %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    }
    break;
  case HTTP_REQUEST_PUT:
    printed_size = snprintf((char *)p, remaining_size, "PUT %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    break;
  case HTTP_REQUEST_DELETE:
    printed_size = snprintf((char *)p, remaining_size, "DELETE %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    break;
  default:
    break;
  }
  return printed_size;
}

int http_add_header(uint8_t *buffer, uint32_t buffer_length, char * header, char * value)
{
  int ret = 0;
  uint32_t len = buffer_length;
  uint8_t *body_start = NULL;
  uint8_t *header_start = NULL;

  /* We insert the header just before the body. */
  /* There must be a "\r\n\r\n" in the buffer. */
  body_start = http_find_body(buffer, &len);
  if (body_start != NULL)
  {
    if (body_start != buffer)
    {
      header_start = body_start;
      /* we go back two characters to insert the header after the first "\r\n" of "\r\n\r\n" */
      header_start--;
      header_start--;
      ret = sprintf((char*)header_start, "%s: %s\r\n\r\n", header, value); /* @TODO : need to check length */
      ret = ret + (header_start - buffer);
    }
    else {
      ret = HTTP_ERR;
    }
  } else {
    ret = HTTP_ERR;
  }
  return ret;
}

int http_add_body(uint8_t * buffer, uint32_t buffer_length, uint8_t *body, uint32_t body_length)
{
  int ret = 0;
  uint8_t *body_start = NULL;
  uint32_t len = buffer_length;

  body_start = http_find_body(buffer, &len);
  if (body_start != NULL)
  {
    memcpy(body_start, body, body_length); /* @TODO : check max length */
    ret = (body_start - buffer) + body_length; /* return total length (header + body */
  } else {
    ret = HTTP_ERR;
  }
  
  return ret;
}

int http_send(http_handle_t hnd, uint8_t * buffer, uint32_t length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int rc = HTTP_OK;

  /* send request */
  msg_debug("http_send (%lu):\n%.*s\n", length, (int)length, buffer);

  rc = net_sock_send(pCtx->sock, buffer, length);
  msg_debug("net_sock_send() rc = %d\n", rc);

  return rc;
}

int http_recv(http_handle_t hnd, uint8_t * buffer, uint32_t length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int rc = HTTP_OK;

  rc = net_sock_recv(pCtx->sock, buffer, length);
  msg_debug("net_sock_recv() rc = %d\n", rc);
  return rc;
}

int http_recv_response(http_handle_t hnd, uint8_t * buffer, uint32_t buffer_length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int received = 0;
  uint8_t *pBody = NULL;
  uint32_t read_offset = 0;
  uint32_t body_length = 0;
  uint32_t content_length = 0;
  uint32_t response_length = 0;
  
  response_length = buffer_length;

  do
  {
    received = net_sock_recv(pCtx->sock, buffer + read_offset, buffer_length - read_offset);
    msg_debug("net_sock_recv() received = %d\n", received);
    if (received >= 0)
    {
      read_offset += received;
      if (pBody == NULL)
      {
        body_length = read_offset;
        pBody = http_find_body(buffer, &body_length);
        if (pBody != NULL)
        {
          content_length = http_content_length(buffer, read_offset);
          response_length = pBody - buffer + content_length;
          msg_debug("buffer=%p pBody=%p content-length=%lu response_length=%lu\n",
                    buffer, pBody, content_length, response_length);
          if (response_length > buffer_length)
          {
            response_length = buffer_length;
          }
        }
      }
    } else {
      break;
    }
  } while ((received > 0) && (read_offset < response_length));
  if (received < 0)
  {
    return received;
  }
  else
  {
    return read_offset;
  }
}

int http_append_chunk(http_handle_t hnd,
                      uint8_t *dst_buffer,
                      uint32_t *dst_offset,
                      uint32_t dst_length,
                      uint8_t *src_buffer,
                      uint32_t src_length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int received = 0;
  int recv_size = 0;
  int chunk_length = 0;

  uint8_t *chunk = NULL;
  uint32_t offset = *dst_offset;

  uint8_t trailer_buf[2];

  //find the start of the chunk within src_buffer
  chunk = http_find_chunk(src_buffer, &src_length, &chunk_length);

  if(chunk != NULL && chunk_length > 0 &&
       (chunk_length <= dst_length-offset))
  {
    //append the chunk to dst_buffer
    memcpy(dst_buffer+offset, chunk, src_length);

    //loop to complete a single chunk
    do
    {
      recv_size = chunk_length-src_length;
      //msg_debug("recv_size=%d\n", recv_size);
      if(recv_size == 0)
      {
        break;
      }
      received = net_sock_recv(pCtx->sock, dst_buffer+offset+src_length, recv_size);
      if(received > 0)
      {
        src_length += received;
        //msg_debug("received=%d src_length=%d\n", received, src_length);
      }
    } while(received > 0);

    *dst_offset += src_length;
    //msg_debug("dst_offset=%d\n", *dst_offset);

    //each chunk is closed by a "\r\n" sequence, hence move to the beginning of the next chunk
    net_sock_recv(pCtx->sock, trailer_buf, 2);

    return HTTP_OK;
  }
  else
  {
    return HTTP_ERR;
  }
}

int http_recv_chunked(http_handle_t hnd, uint8_t *buffer, uint32_t buffer_length, int *http_status)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  uint8_t recv_buf[1024];

  uint32_t body_length = 0;

  int received = 0;

  memset(recv_buf, 0, sizeof(recv_buf));
  received = net_sock_recv(pCtx->sock, recv_buf, sizeof(recv_buf));
  //msg_debug("http_recv received=%d recv_buf=\n%.*s\n", received, received, recv_buf);
  if(received > 0)
  {
    int content_length = 0;
    //check the HTTP status of the response
    *http_status = http_response_status(recv_buf, received);
    printf("status=%d\n", *http_status);
    //check the HTTP content length of the response
    content_length = http_content_length(recv_buf, received);
    msg_debug("content_length=%d\n", content_length);
    //check if the 'Transfer-Encoding: chunked' is included in the HTTP headers
    if(http_transfer_is_chunked(recv_buf, received))
    {
      msg_debug("response is chunked\n");
      body_length = http_recv_chunks(hnd,
                                     buffer,
                                     buffer_length,
                                     recv_buf,
                                     sizeof(recv_buf),
                                     received);
    }
    else
    {
      msg_debug("response is not chunked\n");
      body_length = http_recv_payload(hnd,
                                      buffer,
                                      buffer_length,
                                      recv_buf,
                                      sizeof(recv_buf),
                                      content_length,
                                      received);
    }
  }
  return body_length;

}

int http_recv_chunks(http_handle_t hnd,
                     uint8_t *buffer,
                     uint32_t buffer_length,
                     uint8_t *recv_buf,
                     uint32_t recv_buf_len,
                     int recv_len)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  uint8_t* pBody = NULL;
  uint32_t body_offset = 0;
  uint32_t len = 0;

  int received = recv_len;

  while(received > 0)
  {
    len = recv_len;
    if(pBody == NULL)
    {
      pBody = http_find_body(recv_buf, &len);
      if(pBody == NULL)
      {
        break;
      }
      if(http_append_chunk(hnd,
                           buffer,
                           &body_offset,
                           buffer_length,
                           pBody,
                           len) != HTTP_OK)
      {
        break;
      }
    }
    else
    {
      if(http_append_chunk(hnd,
                           buffer,
                           &body_offset,
                           buffer_length,
                           recv_buf,
                           len) != HTTP_OK)
      {
        break;
      }
    }

    memset(recv_buf, 0, recv_buf_len);
    received = net_sock_recv(pCtx->sock, recv_buf, recv_buf_len);
    msg_debug("net_sock_recv received=%d recv_buf=\n%.*s\n", received, received, recv_buf);
  }

  return body_offset;
}

int http_recv_payload(http_handle_t hnd,
                      uint8_t *buffer,
                      uint32_t buffer_length,
                      uint8_t *recv_buf,
                      uint32_t recv_buf_len,
                      int content_length,
                      int recv_len)
{
  http_context_t * pCtx = (http_context_t *) hnd;

  uint8_t* pBody = NULL;
  uint32_t len = 0;

  int recv_size = 0;
  int received = 0;

  len = recv_len;
  pBody = http_find_body(recv_buf, &len);
  if(pBody != NULL && content_length > 0 && (content_length <= buffer_length))
  {
    //append the piece of body already received to destination buffer
    memcpy(buffer, pBody, len);

    //loop to recv all remaining data bytes
    do
    {
      recv_size = content_length - len;
      //msg_debug("recv_size=%d\n", recv_size);
      if(recv_size == 0)
      {
        break;
      }
      received = net_sock_recv(pCtx->sock, buffer+len, recv_size);
      if(received > 0)
      {
        len += received;
        //msg_debug("received=%d len=%d\n", received, len);
      }
    } while(received > 0);
  }

  return len;
}

int http_get(const http_handle_t hnd,
             const char * query,
             const char * additional_headers,
             uint8_t * readbuffer,
             uint32_t readbuffer_size)
{
  int rc = HTTP_OK;
  int header_size = 0;
  int response_size = 0;
  uint32_t status = 0;
  int additional_headers_length = 0;
  int need_final_crlf = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }
  
  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }
  
  additional_headers_length = strlen(additional_headers);
  if ( additional_headers[additional_headers_length-1] != '\n'
        || additional_headers[additional_headers_length-2] != '\r' )
  {
    need_final_crlf = 1;
  }
  
  /* format request */
  header_size = snprintf((char *)pCtx->buffer, sizeof(pCtx->buffer),
             "GET %s HTTP/" HTTP_VERSION "\r\n"
             "Host: %s\r\n"
             "%s"
             "%s"
             "\r\n",
             query,
             pCtx->hostname,
             additional_headers,
             need_final_crlf?"\r\n":""
             );
  /* send request */
  msg_debug("sending request (%d):\n%.*s\n", header_size, header_size, pCtx->buffer);

  rc = net_sock_send(pCtx->sock, (uint8_t *) pCtx->buffer, header_size);
  msg_debug("net_sock_send() rc = %d\n", rc);
  if (rc <= 0)
  {
    return rc;
  }
  /* receive response */
  rc = http_recv_response(hnd, readbuffer, readbuffer_size);
  msg_debug("http_recv_response() rc = %d\n", rc);
  if (rc < 0)
  {
    return rc;
  }

  response_size = rc;
  msg_debug("response (%d)\n%.*s\n", response_size, response_size, readbuffer);

  if (http_response_validate(readbuffer, response_size) != HTTP_OK)
  {
    msg_debug("incorrect header\n");
    return HTTP_ERR;
  }
  status = http_response_status(readbuffer, response_size);
  if (status < 200 || (status>=300))
  {
    msg_debug("incorrect status code %lu\n", status);
    return HTTP_ERR;
  }
  if (status == 204 /* No Content */)
  {
    msg_debug("status code 204 - No Content\n");
    return 0;
  }
  
  return response_size;
}


int http_post(const http_handle_t hnd,
              const char * query,
              const char * additional_headers,
              const uint8_t * postbuffer,
              uint32_t postbuffer_size,
              uint8_t *responsebuffer, 
              uint32_t responsebuffer_size
              )
{
  int header_size = 0;
  int len = 0;
  int additional_headers_length = 0;
  int need_final_crlf = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }
  
  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }
  additional_headers_length = strlen(additional_headers);
  if ( additional_headers[additional_headers_length-1] != '\n'
        || additional_headers[additional_headers_length-2] != '\r' )
  {
    need_final_crlf = 1;
  }
  
  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
                 "POST %s HTTP/" HTTP_VERSION "\r\n"
                 "Host: %s\r\n"
                 "Content-Length: %lu\r\n"
                 "%s"
                 "%s"
                 "\r\n",
                 query,
                 pCtx->hostname,
                 postbuffer_size,
                 additional_headers,
                 need_final_crlf?"\r\n":""
               );
  /* append POST body */
  if ((header_size + postbuffer_size) > sizeof(pCtx->buffer))
  {
    msg_debug("postbuffer_size too big (%lu) - truncated to %d\n", postbuffer_size, sizeof(pCtx->buffer) - header_size);
    postbuffer_size = sizeof(pCtx->buffer) - header_size;
  }
  memcpy(pCtx->buffer + header_size, postbuffer, postbuffer_size);
  msg_debug("post buffer (%lu):\n%.*s\n", header_size + postbuffer_size,
            (int)(header_size + postbuffer_size), pCtx->buffer);
  /* send request */
  len = net_sock_send(pCtx->sock, pCtx->buffer, header_size + postbuffer_size);
  msg_debug("post net_sock_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }

  /* receive response */
  len = http_recv_response(hnd, responsebuffer, responsebuffer_size);
  msg_debug("http_recv_response() len = %d\n", len);

  return len;
}


int http_put(const http_handle_t hnd,
              const char * resource,
              const char * additional_headers,
              const uint8_t * putbuffer,
              uint32_t putbuffer_size,
              uint8_t *responsebuffer, 
              uint32_t responsebuffer_size
              )
{
  int header_size = 0;
  int len = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }
  
  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }
  
  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
               "PUT %s HTTP/" HTTP_VERSION "\r\n"
               "Host: %s\r\n"
               "Content-Length: %lu\r\n"
               "%s"
               "\r\n",
               resource,
               pCtx->hostname,
               putbuffer_size,
               additional_headers
             );
  /* append PUT body */
  if ((header_size + putbuffer_size) > sizeof(pCtx->buffer))
  {
    msg_debug("putbuffer_size too big (%lu) - truncated to %d\n", putbuffer_size, sizeof(pCtx->buffer) - header_size);
    putbuffer_size = sizeof(pCtx->buffer) - header_size;
  }
  memcpy(pCtx->buffer + header_size, putbuffer, putbuffer_size);
  /* send request */
  msg_debug("sending request (%lu):\n%.*s\n", header_size + putbuffer_size, (int)(header_size + putbuffer_size), pCtx->buffer);
  len = net_sock_send(pCtx->sock, pCtx->buffer, header_size + putbuffer_size);
  msg_debug("put net_sock_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }
  /* receive response */
  len = http_recv_response(hnd, responsebuffer, responsebuffer_size);
  msg_debug("http_recv_response() len = %d\n", len);

  return len;
}


int http_delete(const http_handle_t hnd, const char * resource, const char * additional_headers,
              uint8_t *responsebuffer, uint32_t responsebuffer_size)
{
  int header_size = 0;
  int len = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }
  
  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }
  
  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
               "DELETE %s HTTP/" HTTP_VERSION "\r\n"
               "Host: %s\r\n"
               "%s"
               "\r\n",
               resource,
               pCtx->hostname,
               additional_headers
             );
  /* send request */
  len = net_sock_send(pCtx->sock, pCtx->buffer, header_size);
  msg_debug("delete net_sock_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }
  /* receive response */
  len = net_sock_recv(pCtx->sock, responsebuffer, responsebuffer_size);
  msg_debug("delete net_sock_recv: len=%d\n", len);
  
  return len; /* Return the response length, or a <0 error code. */
}

/**
 * @brief   Tells whether an HTTP session is still open, or has been closed by the server.
 * @param   In: hnd   Session handle.
 * @retval  true:   The session is open.
 *          false:  The session is closed: the handle should be freed by calling http_close().
 */
bool http_is_open(const http_handle_t hnd)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  return pCtx->connection_is_open;
}

/**
  * @brief  find the body part in an HTTP message
  * @arg    http_message: pointer to full HTTP message
  * @arg    len: pointer to HTTP message length. It will contain body length in return.
  * @retval pointer to body in HTTP message
  */
uint8_t * http_find_body(uint8_t * http_message, uint32_t *len)
{
  uint8_t *p = NULL;
  
  p = http_message;
  while ((*len) >= 4)
  {
    /* the HTTP message body is after an empty line (CR-LF-CR-LF) */
    if (p[0] == '\r'
        && p[1] == '\n'
        && p[2] == '\r'
        && p[3] == '\n')
    {
      *len = (*len) - 4;
      return p+4;
    }
    p++;
    (*len)--;
  }
  *len = 0;
  /* if not found, return NULL */
  return NULL;
}

uint8_t * http_find_chunk(uint8_t * http_message, uint32_t *len, int *chunk_len)
{
  uint8_t *p = NULL;
  char len_str[8] = {0};
  int i = 0;

  if((*len) < 2)
  {
    *len = 0;
    return NULL;
  }

  p = http_message;
  while ((*len) >= 2)
  {
    /* the HTTP message chunk is after an empty line (CR-LF) */
    if (p[0] == '\r'
        && p[1] == '\n')
    {
      *len = (*len) - 2;
      *chunk_len = (int)strtol(len_str, NULL, 16); // HexToInt
      return p+2;
    }
    else
    {
      len_str[i] = *(p);
    }
    p++;
    i++;
    (*len)--;
  }
  *len = 0;
  /* if not found, return NULL */
  return NULL;
}

/**
  * @brief  return the status code from an HTTP response
  * @arg    http_response: pointer to full HTTP response
  * @arg    len: HTTP response length.
  * @retval HTTP status code ( 200 = success, 404 = not found ... )
  */
uint32_t http_response_status(uint8_t * http_response, uint32_t length)
{
  uint32_t status = 0;
  char string[4];
  
  if (memcmp(http_response, HTTP_HEADER, sizeof(HTTP_HEADER)-1) != 0)
  {
    return 0; /* incorrect buffer */
  }
  
  string[0] = http_response[9]; /* after HTTP/1.1<space> */
  string[1] = http_response[10];
  string[2] = http_response[11];
  string[3] = 0;
  
  status = atoi(string);
  
  return status;
}

/**
  * @brief  check response format
  * @arg    http_response: pointer to full HTTP response
  * @arg    len: HTTP response length.
  * @retval 
  */
int http_response_validate(uint8_t * http_response, uint32_t length)
{
  uint32_t status = HTTP_OK;
  
  if (memcmp(http_response, HTTP_HEADER, sizeof(HTTP_HEADER)-1) != 0)
  {
    return HTTP_ERR; /* incorrect header */
  }
  
  return status;
}

uint8_t * http_find_headers(uint8_t * http_message, uint32_t len)
{
  uint8_t *p = NULL;
  unsigned int length = 0; 
  
  for( p = http_message, length = len; 
       (length > 0) && (*p != '\r') && (*p != '\n'); 
       p++, length-- )
  {
  }
  if (*p == '\r')
  {
    p++;
    if (*p == '\n')
    {
      p++;
    }
    return p;
  } else if (*p == '\n')
  {
    return p;
  } else {
    return NULL;
  }
}

uint32_t http_content_length(uint8_t * buffer, uint32_t len)
{
  uint8_t *p = NULL;
  unsigned int length = 0; 
  
#define CONTENT_LENGTH "Content-Length: "
  for( p = buffer, length = len; 
       (length > 0) ; 
       p++, length-- )
  {
    if (strncasecmp((char *)p, CONTENT_LENGTH, sizeof(CONTENT_LENGTH)-1)==0)
    {
      return atoi((char*)p+(sizeof(CONTENT_LENGTH)-1));
    }
  }
  return 0;
}

int http_transfer_is_chunked(uint8_t * buffer, uint32_t len)
{
  uint8_t *p = NULL;
  char *encoding = NULL;
  unsigned int length = 0;

#define TRANSFER_ENCODING "Transfer-Encoding: "
#define TRANSFER_ENCODING_CHUNKED "chunked"
  for( p = buffer, length = len;
       (length > 0) ;
       p++, length-- )
  {
    if (strncasecmp((char *)p, TRANSFER_ENCODING, sizeof(TRANSFER_ENCODING)-1)==0)
    {
      encoding = ((char*)p+(sizeof(TRANSFER_ENCODING)-1));
      if(strncasecmp(encoding, TRANSFER_ENCODING_CHUNKED, (sizeof(TRANSFER_ENCODING_CHUNKED)-1))==0)
      {
        return 1;
      }
    }
  }
  return 0;
}

