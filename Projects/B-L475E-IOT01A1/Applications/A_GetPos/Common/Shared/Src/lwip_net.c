/**
  ******************************************************************************
  * @file    lwip_net.c
  * @author  MCD Application Team
  * @brief   LwIP ethernet specific network initialization.
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
#ifdef USE_LWIP

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iot_flash_config.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"
#include "ethernetif.h"

/* Private defines -----------------------------------------------------------*/
#define USE_DHCP
#ifdef USE_DHCP

#define IP_ADDR0  0
#define IP_ADDR1  0
#define IP_ADDR2  0
#define IP_ADDR3  0

#define GW_ADDR0  0
#define GW_ADDR1  0
#define GW_ADDR2  0
#define GW_ADDR3  0

#define MASK_ADDR0  0
#define MASK_ADDR1  0
#define MASK_ADDR2  0
#define MASK_ADDR3  0

#else

#define IP_ADDR0  192
#define IP_ADDR1  168
#define IP_ADDR2  1
#define IP_ADDR3  1

#define GW_ADDR0  192
#define GW_ADDR1  168
#define GW_ADDR2  1
#define GW_ADDR3  1

#define MASK_ADDR0  255
#define MASK_ADDR1  255
#define MASK_ADDR2  254
#define MASK_ADDR3  0
#endif /* USE_DHCP */


/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);
int net_if_reinit(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/

int net_if_init(void * if_ctxt)
{
  struct netif * Netif = (struct netif *) if_ctxt;
  ip4_addr_t addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  uint32_t start;

  msg_info("\n*** Ethernet connection ***\n");
  
  msg_info("Initializing LwIP on Ethernet interface\n\n");

  tcpip_init(NULL, NULL);

  /* IP default settings, to be overridden by DHCP */  

  IP4_ADDR(&addr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  IP4_ADDR(&netmask, MASK_ADDR0, MASK_ADDR1, MASK_ADDR2, MASK_ADDR3);
  
  /* add the network interface */    
  netif_add(Netif, &addr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /* register the default network interface */
  netif_set_default(Netif);
  
  netif_set_up(Netif);

#ifdef USE_DHCP
  msg_info("Starting DHCP client to get IP address...\n");

  dhcp_start(Netif);

  start = HAL_GetTick();
  
  while((Netif->ip_addr.addr == 0) && (HAL_GetTick() - start < 10000))
  {
  }
#endif

  if (Netif->ip_addr.addr == 0)
  {
    msg_info(" Failed to get IP address! Please check cable and/or network configuration.\n");
    Error_Handler();
  }
  else
  {
    msg_debug("\nIpAddress = %lu.%lu.%lu.%lu\n", (Netif->ip_addr.addr & 0xff), ((Netif->ip_addr.addr >> 8) & 0xff)
                                        , ((Netif->ip_addr.addr >> 16) & 0xff), ((Netif->ip_addr.addr >> 24)& 0xff));
#ifdef USE_DHCP
    dhcp_stop(Netif);
#endif
  }

  return 0;
}


int net_if_deinit(void * if_ctxt)
{
  (void) if_ctxt;
  return 0;
}

int net_if_reinit(void * if_ctxt)
{
  msg_error("net_if_reinit() not implemented on LwIP.\n");
  return -1;
}

#endif /* USE_LWIP */

