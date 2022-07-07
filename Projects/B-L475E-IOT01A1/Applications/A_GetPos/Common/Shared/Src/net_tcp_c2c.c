/**
  ******************************************************************************
  * @file    net_tcp_c2c.c
  * @author  MCD Application Team
  * @brief   Network abstraction at transport layer level. TCP implementation
  *          on ST C2C connectivity API.
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
#include "net_internal.h"

#ifdef USE_C2C

/* Private defines -----------------------------------------------------------*/
/* The socket timeout of the non-blocking sockets is supposed to be 0.
 * But the underlying component does not necessarily support a non-blocking
 * socket interface.
 * The NOBLOCKING timeouts are intended to be set to the lowest possible value
 * supported by the underlying component. E.g. UART at 115200 needs at least
 * 105 ms to send a packet of 1500 bytes. */
#define NET_DEFAULT_NOBLOCKING_WRITE_TIMEOUT  500
#define NET_DEFAULT_NOBLOCKING_READ_TIMEOUT   500

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char OperatorsString[C2C_OPERATORS_LIST + 1];

/* Private function prototypes -----------------------------------------------*/
int net_sock_create_c2c(net_hnd_t nethnd, net_sockhnd_t * sockhnd, net_proto_t proto);
int net_sock_open_c2c(net_sockhnd_t sockhnd, const char * hostname, int remoteport, int localport);
int net_sock_recv_tcp_c2c(net_sockhnd_t sockhnd, uint8_t * buf, size_t len);
int net_sock_recvfrom_udp_c2c(net_sockhnd_t sockhnd, uint8_t * const buf, size_t len, net_ipaddr_t * remoteaddress, int * remoteport);
int net_sock_send_tcp_c2c(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len);
int net_sock_sendto_udp_c2c(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len,  net_ipaddr_t * remoteaddress, int remoteport);
int net_sock_close_tcp_c2c(net_sockhnd_t sockhnd);
int net_sock_destroy_tcp_c2c(net_sockhnd_t sockhnd);

/* Functions Definition ------------------------------------------------------*/

int net_sock_create_c2c(net_hnd_t nethnd, net_sockhnd_t * sockhnd, net_proto_t proto)
{
  int rc = NET_ERR;
  net_ctxt_t *ctxt = (net_ctxt_t *) nethnd;
  net_sock_ctxt_t *sock = NULL;

  sock = net_malloc(sizeof(net_sock_ctxt_t));
  if (sock == NULL)
  {
    msg_error("net_sock_create allocation failed.\n");
    rc = NET_ERR;
  }
  else
  {
    memset(sock, 0, sizeof(net_sock_ctxt_t));
    sock->net = ctxt;
    sock->next = ctxt->sock_list;
    sock->methods.open      = (net_sock_open_c2c);
    switch(proto)
    {
      case NET_PROTO_TCP:
        sock->methods.recv      = (net_sock_recv_tcp_c2c);
        sock->methods.send      = (net_sock_send_tcp_c2c);
        break;
      case NET_PROTO_UDP:
        // temporarily return, until the C2C UDP sockets are implemented.
        /* sock->methods.recvfrom  = (net_sock_recvfrom_udp_c2c); */
        /* sock->methods.sendto    = (net_sock_sendto_udp_c2c);   */
        /* break; */
        ;
      default:
        free(sock);
        return NET_PARAM;
    }
    sock->methods.close     = (net_sock_close_tcp_c2c);
    sock->methods.destroy   = (net_sock_destroy_tcp_c2c);
    sock->proto             = proto;
    sock->blocking          = NET_DEFAULT_BLOCKING;
    sock->read_timeout      = NET_DEFAULT_BLOCKING_READ_TIMEOUT;
    sock->write_timeout     = NET_DEFAULT_BLOCKING_WRITE_TIMEOUT;
    ctxt->sock_list         = sock; /* Insert at the head of the list */
    *sockhnd = (net_sockhnd_t) sock;

    rc = NET_OK;
  }

  return rc;
}


int net_sock_open_c2c(net_sockhnd_t sockhnd, const char * hostname, int remoteport, int localport)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint8_t ip_addr[4] = { 0, 0, 0, 0 };
  C2C_Protocol_t proto;

  sock->underlying_sock_ctxt = (net_sockhnd_t) -1; /* Initialize to a non-null value which may not be confused with a valid port number. */

  /* Find a free underlying socket on the network interface. */
  bool underlying_socket_busy[C2C_MAX_CONNECTIONS];
  memset(underlying_socket_busy, 0, sizeof(underlying_socket_busy));

  net_sock_ctxt_t * cur = sock->net->sock_list;
  do
  {
    if ( ((cur->proto == NET_PROTO_TCP) || (cur->proto == NET_PROTO_UDP) )&& ((int) cur->underlying_sock_ctxt >= 0) )
    {
      underlying_socket_busy[(int) cur->underlying_sock_ctxt] = true;
    }
    cur = cur->next;
  } while (cur != NULL);

  for (int i = 0; i < C2C_MAX_CONNECTIONS; i++)
  {
    if (underlying_socket_busy[i] == false)
    {
      sock->underlying_sock_ctxt = (net_sockhnd_t) i;
      break;
    }
  }

  /* A free socket has been found */
  if (sock->underlying_sock_ctxt >= 0)
  {
    switch(sock->proto)
    {
      case NET_PROTO_TCP:
        if (localport != 0)
        { /* TCP local port setting is not implemented */
          rc = NET_PARAM;
        }
        else
        {
          if (C2C_GetHostAddress((char *)hostname, ip_addr) != C2C_RET_OK)
          {
            /* NB: This blocking call may take several seconds before returning.
             *     An asynchronous interface should be added. */
            msg_info("The address of %s could not be resolved.\n", hostname);
            rc = NET_ERR;
          }
          else
          {
            proto = C2C_TCP_PROTOCOL;
            rc = NET_OK;
          }
        }
        break;
      case NET_PROTO_UDP:
        /* Record the local port binding. */
        sock->localport = localport;
        proto = C2C_UDP_PROTOCOL;
        rc = NET_OK;
        break;
      default:
        ;
    }


    if (rc == NET_OK)
    {
      /* open client connection */
      if( C2C_RET_OK != C2C_StartClientConnection(
            (uint32_t) sock->underlying_sock_ctxt, proto, NULL, ip_addr, remoteport, localport) )
      {
        msg_error("Failed opening the underlying C2C socket %d.\n", (int) sock->underlying_sock_ctxt);
        sock->underlying_sock_ctxt = (net_sockhnd_t) -1;
        rc = NET_ERR;
      }
    }
  }
  else
  {
    msg_error("Could not find a free socket on the specified network interface.\n");
    rc = NET_PARAM;
  }

  return rc;
}


int net_sock_recv_tcp_c2c(net_sockhnd_t sockhnd, uint8_t * buf, size_t len)
{
  int rc = 0;
  C2C_Ret_t status = C2C_RET_OK;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint16_t read = 0;
  uint16_t tmp_len = MIN(len, C2C_PAYLOAD_SIZE);
  uint8_t * tmp_buf = buf;
  uint32_t start_time = HAL_GetTick();

  /* Read the received payload by chunks of C2C_PAYLOAD_SIZE bytes because of
   * a constraint of C2C_ReceiveData(). */
  do
  {
    if ( (sock->blocking == true) && (net_timeout_left_ms(start_time, HAL_GetTick(), sock->read_timeout) <= 0) )
    {
      rc = NET_TIMEOUT;
      break;
    }

    status = C2C_ReceiveData((uint8_t) ((uint32_t)sock->underlying_sock_ctxt & 0xFF), tmp_buf, tmp_len, &read,
                             (sock->blocking == true) ? sock->read_timeout : NET_DEFAULT_NOBLOCKING_READ_TIMEOUT);

    msg_debug("Read %d/%d.\n", read, tmp_len);
    if (status != C2C_RET_OK)
    {
      msg_error("net_sock_recv(): error %d in C2C_ReceiveData() - socket=%d requestedLen=%d received=%d\n",
             status, (int) sock->underlying_sock_ctxt, tmp_len, read);
      msg_error("The port is likely to have been closed by the server.\n")
      rc = NET_EOF;
      break;
    }
    else
    {
      if (read > tmp_len)
      {
        msg_error("C2C_ReceiveData() returned a longer payload than requested (%d/%d).\n", read, tmp_len);
        rc = NET_ERR;
        break;
      }
      tmp_buf += read;
      tmp_len = MAX(0, MIN(len - (tmp_buf - buf), C2C_PAYLOAD_SIZE));
    }

  } while ( (read == 0) && (sock->blocking == true) && (rc == 0) );

  return (rc < 0) ? rc : tmp_buf - buf;
}


int net_sock_recvfrom_udp_c2c(net_sockhnd_t sockhnd, uint8_t * const buf, size_t len, net_ipaddr_t * remoteaddress, int * remoteport)
{
  /* Not implemented */
  return NET_PARAM;
}


int net_sock_send_tcp_c2c( net_sockhnd_t sockhnd, const uint8_t * buf, size_t len)
{
  int rc = 0;
  C2C_SendStatus_t status = C2C_SEND_OK;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  uint16_t sent = 0;
  uint32_t start_time = HAL_GetTick();
  char ErrorString[C2C_ERROR_STRING];

  memset(ErrorString, 0, C2C_ERROR_STRING);

  do
  {
    if ( (sock->blocking == true) && (net_timeout_left_ms(start_time, HAL_GetTick(), sock->write_timeout) <= 0) )
    {
      rc = NET_TIMEOUT;
      break;
    }

    status = C2C_SendData((uint8_t) ((uint32_t)sock->underlying_sock_ctxt & 0xFF), (uint8_t *)buf, len, &sent,
                          (sock->blocking == true) ? sock->write_timeout : NET_DEFAULT_NOBLOCKING_WRITE_TIMEOUT );

    msg_debug("Sent %d/%d.\n", sent, len);
    if (status !=  C2C_SEND_OK)
    {
      if (status == C2C_SEND_BUF_FULL)
      {
        msg_debug("C2C_SendData(): warning: modem returned buffer full; partial send %d/%d ", sent, len);
      }
      else
      {
        C2C_RetrieveLastErrorDetails(ErrorString);
        msg_error("C2C_SendData(): send ERROR: %s\n", ErrorString);
        rc = NET_ERR;
        msg_error("Send failed.\n");
        break;
      }
    }

  } while ( (sent == 0) && (sock->blocking == true) && (rc == 0) );

  return (rc < 0) ? rc : sent;
}


int net_sock_sendto_udp_c2c(net_sockhnd_t sockhnd, const uint8_t * buf, size_t len, net_ipaddr_t * remoteaddress, int remoteport)
{
  /* Not implemented */
  return NET_PARAM;
}


int net_sock_close_tcp_c2c(net_sockhnd_t sockhnd)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  C2C_Ret_t status = C2C_StopClientConnection((uint8_t) ((uint32_t)sock->underlying_sock_ctxt && 0xFF));
  if (status == C2C_RET_OK)
  {
    sock->underlying_sock_ctxt = (net_sockhnd_t) -1;
    rc = NET_OK;
  }
  return rc;
}


int net_sock_destroy_tcp_c2c(net_sockhnd_t sockhnd)
{
  int rc = NET_ERR;
  net_sock_ctxt_t *sock = (net_sock_ctxt_t * ) sockhnd;
  net_ctxt_t *ctxt = sock->net;

  /* Find the parent in the linked list.
   * Unlink and free.
   */
  if (sock == ctxt->sock_list)
  {
    ctxt->sock_list = sock->next;
    rc = NET_OK;
  }
  else
  {
    net_sock_ctxt_t *cur = ctxt->sock_list;
    do
    {
      if (cur->next == sock)
      {
        cur->next = sock->next;
        rc = NET_OK;
        break;
      }
      cur = cur->next;
    } while(cur->next != NULL);
  }
  if (rc == NET_OK)
  {
    net_free(sock);
  }

  return rc;
}

#endif /* USE_C2C */

