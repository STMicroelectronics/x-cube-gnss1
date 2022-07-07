/**
  ******************************************************************************
  * @file    httpclient.c
  * @author  MCD Application Team
  * @brief   HTTP Library client example
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
#include "main.h"
#include "http_lib.h"
#include "rootca.h"
#include "httpclient.h"

//#include "a_gnss_json.h"

/* Global variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define NET_READ_TIMEOUT  "2000"

#define BUF_SIZE (20480)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
http_handle_t Handle;
const char Httpbin_org_server_name[] = "httpbin.org";
const char * RootCA = ROOT_CERTIFICATE;

/* Private function prototypes -----------------------------------------------*/
int test_http_get_recv(http_handle_t http_handle);
int test_http_post_send(http_handle_t http_handle);
int rxn_http_post_send(http_handle_t http_handle,
                       char *rxn_server_name,
                       char *rxn_server_auth,
                       uint8_t *data_buffer,
                       uint32_t data_buffer_size,
                       char *JsonData);
int test_http_delete_send(http_handle_t http_handle);
int test_http_put_send(http_handle_t http_handle);
int test_http_get(http_handle_t http_handle);
int test_http_post(http_handle_t http_handle);
int test_http_put(http_handle_t http_handle);
int test_http_delete(http_handle_t http_handle);
/* Private functions ---------------------------------------------------------*/
extern int net_if_init(void * if_ctxt);
extern int net_if_deinit(void * if_ctxt);

/**
  * @brief  Test http get recv
  * @param  None
  * @retval None
  */
int test_http_get_recv(http_handle_t http_handle)
{
  uint8_t buffer[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  int content_length = 0;

  printf("GET RECV test.\n");
  memset(buffer, 0, sizeof(buffer));
  
  ret = http_create_request(HTTP_REQUEST_GET, "httpbin.org", "/ip", buffer, sizeof(buffer));
  msg_debug("http_create_request ret=%d\n", ret);

  /* no header */
  ret = http_add_header(buffer, sizeof(buffer), "Connection", "keep-alive");
  msg_debug("http_add_header ret=%d\n", ret);
  /* no body */
  
  ret = http_send(http_handle, buffer, ret); /* ret contains request + header + CRLF length */
  msg_debug("http_send ret=%d\n", ret);
  
  ret = http_recv(http_handle, buffer, sizeof(buffer));
  msg_debug("http_recv ret=%d\n", ret);
  status = http_response_status(buffer, ret);
  printf("status=%d\n", status);
  content_length = http_content_length(buffer, ret);
  msg_debug("content_length=%d\n", content_length);

  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(buffer, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http post send
  * @param  None
  * @retval None
  */
int test_http_post_send(http_handle_t http_handle)
{
  uint8_t buffer[1024];
  char content_length_value[10];
  const char post_body[] = "post buffer";
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  int content_length = 0;

  printf("POST SEND test.\n");
  memset(buffer, 0, sizeof(buffer));
  
  ret = http_create_request(HTTP_REQUEST_POST, "httpbin.org", "/post", buffer, sizeof(buffer));
  msg_debug("http_create_request ret=%d\n", ret);

  /* headers */
  ret = http_add_header(buffer, sizeof(buffer), "Accept", "text/html");
  msg_debug("http_add_header ret=%d\n", ret);

  sprintf(content_length_value, "%d", sizeof(post_body)-1);
  ret = http_add_header(buffer, sizeof(buffer), "Content-Length", content_length_value);
  msg_debug("http_add_header ret=%d\n", ret);

  ret = http_add_body(buffer, sizeof(buffer), (uint8_t*)post_body, sizeof(post_body)-1);
  msg_debug("http_add_body ret=%d\n", ret);

  ret = http_send(http_handle, buffer, ret);
  msg_debug("http_send ret=%d\n", ret);
  
  ret = http_recv(http_handle, buffer, sizeof(buffer));
  msg_debug("http_recv ret=%d\n", ret);
  status = http_response_status(buffer, ret);
  printf("status=%d\n", status);
  content_length = http_content_length(buffer, ret);
  msg_debug("content_length=%d\n", content_length);

  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(buffer, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  rxn http post send
  * @param  None
  * @retval None
  */
int rxn_http_post_send(http_handle_t http_handle,
                       char *rxn_server_name,
                       char *rxn_server_auth,
                       uint8_t *data_buffer,
                       uint32_t data_buffer_size,
                       char *JsonData)
{
  uint8_t buffer[1024];
  char content_length_value[16];

  char rxn_server_res[] = "/rxn-api/locationApi";

  int ret = 0;
  int status = 0;

  printf("POST SEND rxn(size of buffer=%d).\n", sizeof(buffer));
  memset(buffer, 0, sizeof(buffer));
  //memset(seed, 0, sizeof(uint8_t)*BUF_SIZE);

  ret = http_create_request(HTTP_REQUEST_POST, rxn_server_name, rxn_server_res, buffer, sizeof(buffer));
  msg_debug("http*_create_request() ret=%d\n", ret);

  /* headers */
  ret = http_add_header(buffer, sizeof(buffer), "Accept", "application/json");
  msg_debug("http_add_header() ret=%d\n", ret);

  ret = http_add_header(buffer, sizeof(buffer), "Content-Type", "application/json");
  msg_debug("http_add_header() ret=%d\n", ret);

  (void)snprintf(content_length_value, sizeof(content_length_value), "%d", strlen(JsonData));
  ret = http_add_header(buffer, sizeof(buffer), "Content-Length", content_length_value);
  msg_debug("http_add_header() ret=%d\n", ret);

  //RXN-SP cId=<CUSTOMER>,mId=<MODEL>,pw=<BASE64-ENCODED-PASSWORD>=
  ret = http_add_header(buffer, sizeof(buffer), "Authorization", rxn_server_auth);
  msg_debug("http_add_header() ret=%d\n", ret);

  /* body */
  ret = http_add_body(buffer, sizeof(buffer), (uint8_t*)JsonData, strlen(JsonData));
  msg_debug("http_add_body() ret=%d\n", ret);

  ret = http_send(http_handle, buffer, ret);
  msg_debug("http_send() ret=%d\n", ret);

  /* receive response */  
  ret = http_recv_chunked(http_handle, data_buffer, data_buffer_size, &status);
  //printf("response body size=%lu:\n%.*s\n", ret, ret, data_buffer);

  return ret;
}

/**
  * @brief  Test http put send
  * @param  None
  * @retval None
  */
int test_http_put_send(http_handle_t http_handle)
{
  uint8_t buffer[1024];
  char content_length_value[10];
  const char put_body[] = "put buffer";
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  int content_length = 0;

  printf("PUT SEND test.\n");
  memset(buffer, 0, sizeof(buffer));
  
  ret = http_create_request(HTTP_REQUEST_PUT, "httpbin.org", "/put", buffer, sizeof(buffer));
  msg_debug("http_create_request ret=%d\n", ret);

  sprintf(content_length_value, "%d", sizeof(put_body)-1);
  ret = http_add_header(buffer, sizeof(buffer), "Content-Length", content_length_value);
  msg_debug("http_add_header ret=%d\n", ret);

  ret = http_add_body(buffer, sizeof(buffer), (uint8_t*)put_body, sizeof(put_body)-1);
  msg_debug("http_add_body ret=%d\n", ret);

  ret = http_send(http_handle, buffer, ret);
  msg_debug("http_send ret=%d\n", ret);
  
  ret = http_recv(http_handle, buffer, sizeof(buffer));
  msg_debug("http_recv ret=%d\n", ret);
  status = http_response_status(buffer, ret);
  printf("status=%d\n", status);
  content_length = http_content_length(buffer, ret);
  msg_debug("content_length=%d\n", content_length);

  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(buffer, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http put using http_send
  * @param  None
  * @retval None
  */
int test_http_delete_send(http_handle_t http_handle)
{
  uint8_t buffer[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  int content_length = 0;

  printf("DELETE SEND test.\n");
  memset(buffer, 0, sizeof(buffer));
  
  ret = http_create_request(HTTP_REQUEST_DELETE, "httpbin.org", "/delete", buffer, sizeof(buffer));
  msg_debug("http_create_request ret=%d\n", ret);
  
  ret = http_send(http_handle, buffer, ret);
  msg_debug("http_send ret=%d\n", ret);
  
  ret = http_recv(http_handle, buffer, sizeof(buffer));
  msg_debug("http_recv ret=%d\n", ret);
  status = http_response_status(buffer, ret);
  printf("status=%d\n", status);
  content_length = http_content_length(buffer, ret);
  msg_debug("content_length=%d\n", content_length);

  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(buffer, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http get
  * @param  None
  * @retval None
  */
int test_http_get(http_handle_t http_handle)
{  
  uint8_t response[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  int content_length = 0;

  printf("GET test.\n");
  memset(response, 0, sizeof(response));
  
  ret = http_get(http_handle, "/ip" /* "/encoding/utf8"*/,
                  "", 
                  (uint8_t*)response,
                  sizeof(response));
  
  msg_debug("ret=%d\n", ret);
  status = http_response_status(response, ret);
  printf("status=%d\n", status);
  content_length = http_content_length(response, ret);
  msg_debug("content_length=%d\n", content_length);

  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(response, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http post
  * @param  None
  * @retval None
  */
int test_http_post(http_handle_t http_handle)
{  
  uint8_t response[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  const uint8_t postbuffer[] = "post buffer";
  int status = 0;

  memset(response, 0, sizeof(response));
  printf("POST test.\n");
  ret = http_post(http_handle, "/post", 
                  "Accept: application/json\r\nContent-Type: application/json\r\n",
                  postbuffer,
                  sizeof(postbuffer)-1,
                  (uint8_t*)response,
                  sizeof(response));
  
  msg_debug("ret=%d\n", ret);
  status = http_response_status(response, ret);
  printf("status=%d\n", status);
  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(response, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http put
  * @param  None
  * @retval None
  */
int test_http_put(http_handle_t http_handle)
{  
  uint8_t response[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  const uint8_t putbuffer[] = "put buffer";
  int status = 0;
  
  printf("PUT test.\n");
  memset(response, 0, sizeof(response));
  ret = http_put(http_handle, "/put", 
                  "Accept: application/json\r\nContent-Type: application/json\r\n",
                  putbuffer,
                  sizeof(putbuffer)-1,
                  (uint8_t*)response,
                  sizeof(response));
  
  msg_debug("ret=%d\n", ret);
  status = http_response_status(response, ret);
  printf("status=%d\n", status);
  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(response, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http delete
  * @param  None
  * @retval None
  */
int test_http_delete(http_handle_t http_handle)
{
  uint8_t response[1024];
  int ret = 0;
  uint32_t body_size = 0;
  uint8_t* pBody = NULL;
  int status = 0;
  
  printf("DELETE test.\n");
  memset(response, 0, sizeof(response));
  ret = http_delete(http_handle, "/delete", 
                  "Accept: application/json\r\nContent-Type: application/json\r\n",
                  (uint8_t*)response,
                  sizeof(response));
  
  msg_debug("ret=%d\n", ret);
  status = http_response_status(response, ret);
  printf("status=%d\n", status);
  if (ret > 0)
  {
    body_size = ret;
    pBody = http_find_body(response, &body_size);
    printf("response body size=%lu:\n%.*s\n", body_size, (int)body_size, pBody );
  }
  return ret;
}

/**
  * @brief  Test http client
  * @param  None
  * @retval None
  */
void httpclient_test(void const *arg)
{
  int ret = 0;

  printf("\n**** HTTP client test ****\n\n");
  
  printf("Opening connection to server %s.\n", Httpbin_org_server_name);
  
  ret = http_create_session(&Handle, Httpbin_org_server_name, 443, HTTP_PROTO_HTTPS ); /* see http://httpbin.org */
  msg_debug("http_create_session() ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error opening connection (%d)\n", ret);
    while(1);
  }

  ret = http_sock_setopt(Handle, "tls_ca_certs", (void *)RootCA, strlen(RootCA));
  msg_debug("http_sock_setopt(\"tls_ca_certs\") ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error setting connection option (%d)\n", ret);
    while(1);
  }
  ret = http_sock_setopt(Handle, "tls_server_name", (const uint8_t*)Httpbin_org_server_name, sizeof(Httpbin_org_server_name));
  msg_debug("http_sock_setopt(\"tls_server_name\") ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error setting connection option tls_server_name (%d)\n", ret);
    while(1);
  }
  ret = http_sock_setopt(Handle, "sock_read_timeout", (const uint8_t*)NET_READ_TIMEOUT, strlen(NET_READ_TIMEOUT));
  if (ret != NET_OK)
  {
    msg_error("error setting socket option read timeout (%d)\n", ret);
  }
  ret = http_connect(Handle);
  msg_debug("http_connect() ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error opening connection (%d)\n", ret);
    while(1);
  }
  
  ret = test_http_get_recv(Handle);
  msg_debug("test_http_get_recv() ret=%d\n", ret);

  ret = test_http_post_send(Handle);
  msg_debug("test_http_post_send() ret=%d\n", ret);
  
  ret = test_http_put_send(Handle);
  msg_debug("test_http_put_send() ret=%d\n", ret);
 
  ret = test_http_delete_send(Handle);
  msg_debug("test_http_delete_send() ret=%d\n", ret);


  ret = test_http_get(Handle);
  msg_debug("test_http_get() ret=%d\n", ret);

  ret = test_http_post(Handle);
  msg_debug("test_http_post() ret=%d\n", ret);
  
  ret = test_http_put(Handle);
  msg_debug("test_http_put() ret=%d\n", ret);

  ret = test_http_delete(Handle);
  msg_debug("test_http_delete() ret=%d\n", ret);

  http_close(Handle);
  
  printf("\n**** End ****\n");
}

/**
  * @brief  rxn http client
  * @param  None
  * @retval None
  */
int httpclient_rxn(char *rxn_server_res,
                   char *rxn_server_auth,
                   uint8_t *buffer,
                   uint32_t buffer_size,
                   char *JsonData)
{
  int ret = 0;
  http_proto_t rxn_http_proto = HTTP_PROTO_HTTP;
  char rxn_server_name[] = "api.location.io";
  int rxn_server_port = 80;

  printf("\n**** HTTP client rxn ****\n\n");
  
  printf("Opening connection to server %s.\n", rxn_server_name);

  ret = http_create_session(&Handle, (char*)rxn_server_name, rxn_server_port, rxn_http_proto);
  msg_debug("http_create_session() ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error opening connection (%d)\n", ret);
    return ret;
  }

  ret = http_sock_setopt(Handle, "sock_read_timeout", (const uint8_t*)NET_READ_TIMEOUT, strlen(NET_READ_TIMEOUT));
  if (ret != NET_OK)
  {
    msg_error("error setting socket option read timeout (%d)\n", ret);
  }
  ret = http_connect(Handle);
  msg_debug("http_connect() ret=%d\n", ret);
  if (ret < 0)
  {
    printf("error opening connection (%d)\n", ret);
    return ret;
  }
  
  ret = rxn_http_post_send(Handle,
                           rxn_server_name,
                           rxn_server_auth,
                           buffer,
                           buffer_size,
                           JsonData);
  msg_debug("rxn_http_post_send() ret=%d\n", ret);

  http_close(Handle);
  
  return ret;
}

/**
  * @brief  Initialize application
  * @param  None
  * @retval None
  */
int httpclient_init(void)
{
  net_ipaddr_t ipAddress;
  net_macaddr_t macAddr;
  
  printf("\n**** HTTP client init ****\n");

  if (net_init(&hnet, NET_IF, (net_if_init)) != NET_OK)
  {
    msg_error("error in net_init()\n");
    return -1;
  }

  if (net_get_mac_address(hnet, &macAddr) == NET_OK)
  {
    msg_info("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
             macAddr.mac[0], macAddr.mac[1], macAddr.mac[2], macAddr.mac[3], macAddr.mac[4], macAddr.mac[5]);
  }
  else
  {
    printf("error getting mac address\n");

    return -1;
  }
  
  /* Slight delay since the module seems to take some time prior to being able
   to retrieve its IP address after a connection. */
  HAL_Delay(500);

  printf("Retrieving the IP address.\n");

  if (net_get_ip_address(hnet, &ipAddress) != NET_OK)
  {
    msg_error("error retrieving IP address");
    return -1;
  }
  else
  {
    /* net_ipaddr_t : IPv4 mapped in IPv6 in network order format */
    printf("IP address: %d.%d.%d.%d\n",
         ipAddress.ip[12], ipAddress.ip[13], ipAddress.ip[14], ipAddress.ip[15]);
  }
  /* End of network initialisation */

  printf("Setting the RTC from the network time.\n");
  if (0 != setRTCTimeDateFromNetwork(true))
  {
    msg_error("Could not set the RTC from the network time.\n");
  }

  /* end of configuration verification */
  
  return 0;
}

/**
  * @brief  deinit application
  * @param  None
  * @retval None
  */
void httpclient_deinit(void)
{
  net_deinit(hnet, (net_if_deinit));
}

/* needed by cloud.c */
bool app_needs_device_keypair(void)
{
  return false;
}

int cloud_device_enter_credentials(void)
{
  int ret = 0;
  iot_config_t iot_config;

  memset(&iot_config, 0, sizeof(iot_config_t));

  if(setIoTDeviceConfig(&iot_config) != 0)
  {
    ret = -1;
    msg_error("Failed programming the IoT device configuration to Flash.\n");
  }
  
  return ret;
}

