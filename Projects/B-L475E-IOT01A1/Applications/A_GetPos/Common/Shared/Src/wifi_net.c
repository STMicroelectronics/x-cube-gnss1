/**
  ******************************************************************************
  * @file    wifi_net.c
  * @author  MCD Application Team
  * @brief   Wifi-specific NET initialization.
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
#ifdef USE_WIFI

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "wifi.h"
#include "iot_flash_config.h"

/* Private defines -----------------------------------------------------------*/
#define  WIFI_CONNECT_MAX_ATTEMPT_COUNT  3

#ifdef ES_WIFI_PRODUCT_NAME_SIZE
#define WIFI_PRODUCT_INFO_SIZE                      ES_WIFI_PRODUCT_NAME_SIZE
#define WIFI_PAYLOAD_SIZE                           ES_WIFI_PAYLOAD_SIZE
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);
int net_if_reinit(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  char moduleinfo[WIFI_PRODUCT_INFO_SIZE];
  WIFI_Status_t wifiRes;
  uint8_t macAddress[6];
  int wifiConnectCounter = 0;
  bool skip_reconf = false;

  printf("\n*** WIFI connection ***\n\n");

  skip_reconf = (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User button (Blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  
  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials();
    } while (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK);
  }
  
  /*  Wifi Module initialization */
  printf("Initializing the WiFi module\n");
  
  wifiRes = WIFI_Init();
  if ( WIFI_STATUS_OK != wifiRes )
  {
    printf("Failed to initialize WIFI module\n");
    return -1;
  }
    
  /* Retrieve the WiFi module mac address to confirm that it is detected and communicating. */
  WIFI_GetModuleName(moduleinfo);
  printf("Module initialized successfully: %s",moduleinfo);
  
  WIFI_GetModuleID(moduleinfo);
  printf(" %s",moduleinfo);
  
  WIFI_GetModuleFwRevision(moduleinfo);
  printf(" %s\n",moduleinfo);
  
  printf("Retrieving the WiFi module MAC address:");
  wifiRes = WIFI_GetMAC_Address( (uint8_t*)macAddress);
  if ( WIFI_STATUS_OK == wifiRes)
  {
    printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
         macAddress[0], macAddress[1], macAddress[2],
         macAddress[3], macAddress[4], macAddress[5]);
  }
  else
  {
       printf("Failed to get MAC address\n");
  }
  /* Connect to the specified SSID. */

  printf("\n");
  do 
  {
    printf("\rConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    wifiRes = WIFI_Connect(ssid, psk, security_mode);
    if (wifiRes == WIFI_STATUS_OK) break;
  } 
  while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
  
  if (wifiRes == WIFI_STATUS_OK)
  {
    printf("\nConnected to AP %s\n",ssid);
  }
  else
  {
    printf("\nFailed to connect to AP %s\n",ssid);
  }
  
  return (wifiRes == WIFI_STATUS_OK)?0:-1;
}


int net_if_deinit(void * if_ctxt)
{
  return 0;
}


int net_if_reinit(void * if_ctxt)
{
  int ret = 0;
  const char *ssid;
  const char  *psk;
  WIFI_Ecn_t security_mode;
  WIFI_Status_t wifiRes;
  int wifiConnectCounter = 0;
  
  // wifiRes = WIFI_Disconnect(); // Disconnect() is not enough for the Inventek module which does not clean the previous state properly.
  wifiRes = WIFI_Init();
  if (wifiRes != WIFI_STATUS_OK)
  {
    msg_error("WIFI_Init() failed.\n");
  }
  
  if (checkWiFiCredentials(&ssid, &psk, (uint8_t *) &security_mode) != HAL_OK)
  {
    ret = -1;
  }
  else
  {
    do 
    {
      printf("\rConnecting to AP: %s  Attempt %d/%d ...",ssid, ++wifiConnectCounter,WIFI_CONNECT_MAX_ATTEMPT_COUNT);
      wifiRes = WIFI_Connect(ssid, psk, security_mode);
      if (wifiRes == WIFI_STATUS_OK) break;
    } 
    while (wifiConnectCounter < WIFI_CONNECT_MAX_ATTEMPT_COUNT);
    
    if (wifiRes == WIFI_STATUS_OK)
    {
      printf("\nRe-connected to AP %s\n",ssid);
    }
    else
    {
      printf("\nFailed to re-connect to AP %s\n",ssid);
      ret = -1;
    }
  }
    
  return ret;
}

#endif /* USE_WIFI */

