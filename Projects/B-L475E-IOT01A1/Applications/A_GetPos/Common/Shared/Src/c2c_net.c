/**
  ******************************************************************************
  * @file    c2c_net.c
  * @author  MCD Application Team
  * @brief   C2C-specific NET initialization.
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
#ifdef USE_C2C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "c2c.h"
#include "iot_flash_config.h"

/* Private defines -----------------------------------------------------------*/
#define  C2C_CONNECT_MAX_ATTEMPT_COUNT  3
#define MAX(a,b) ((a) < (b) ? (b) : (a))

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static char OperatorsString[C2C_OPERATORS_LIST + 1];

/* Private function prototypes -----------------------------------------------*/
int net_if_init(void * if_ctxt);
int net_if_deinit(void * if_ctxt);
int net_if_reinit(void * if_ctxt);

/* Functions Definition ------------------------------------------------------*/
int net_if_init(void * if_ctxt)
{
  C2C_Ret_t ret = C2C_RET_ERROR;
  C2C_RegiStatus_t reg_status = C2C_REGISTATUS_UNKNOWN;
  int32_t quality_level = 0;
  int8_t quality_level_db = 0;
  uint8_t c2cConnectCounter = 0;
  bool use_internal_sim = false;
  const char *oper_ap_code;
  const char *username;
  const char *password;
  bool skip_reconf = false;

  char moduleinfo[MAX(MAX(MAX(MAX(C2C_FW_REV_SIZE, C2C_MFC_SIZE), C2C_PROD_ID_SIZE), C2C_IMEI_SIZE), C2C_ICCID_SIZE) + 1];

  printf("\n*** C2C connection ***\n\n");


  skip_reconf = (checkC2cCredentials(&oper_ap_code, &username, &password, &use_internal_sim) == HAL_OK);

  if (skip_reconf == true)
  {
    printf("Push the User button (Blue) within the next 5 seconds if you want to update"
           " the C2C configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }

  if (skip_reconf == false)
  {
    printf("Your C2C parameters need to be entered to proceed.\n");
    do
    {
      updateC2cCredentials();
    } while (checkC2cCredentials(&oper_ap_code, &username, &password, &use_internal_sim) != HAL_OK);
  }
  
  printf("Initializing the C2C module\n");

  /* let's try all the SIMs, in the order they are defined in ENUM*/
  while (ret != C2C_RET_OK) {
    
   if (use_internal_sim == true) 
   {
       printf("Trying to connect with the embedded SIM\n");

   }
   else
   {
       printf("Trying to connect with the external SIM\n");
   }
     
   C2C_SimSelect((use_internal_sim == true) ? SIM_EMBEDDED: SIM_EXT_SLOT);
   HAL_Delay(50);

   /*  C2C Module power up and initialization */
   /* printf("Power Up the C2C module\n"); */
   C2C_HwResetAndPowerUp();
   /* wait additional 2 sec: even the modem is ON, registering can fail if request is done too early */
   HAL_Delay(2000);


   reg_status =  C2C_Init(300);

   if ( (reg_status != C2C_REGISTATUS_ERROR) && (reg_status != C2C_REGISTATUS_SIM_NOT_INSERTED))
   {
    /* Retrieve the C2C module info  */
    memset(moduleinfo, 0, sizeof(moduleinfo));
    C2C_GetModuleName(moduleinfo);
    printf("Module initialized successfully: %s\n", moduleinfo);
    memset(moduleinfo, 0, sizeof(moduleinfo));

    C2C_GetModuleID(moduleinfo);
    printf("ProductID: %s\n", moduleinfo);

    C2C_GetModuleFwRevision(moduleinfo);
    printf("FW version: %s\n", moduleinfo);

    C2C_GetSimId(moduleinfo);
    printf("SIM Id (IccID): %s\n",moduleinfo);

    /* Retrieve the quality level of the connectio  */
    C2C_GetSignalQualityStatus(&quality_level);

    if (quality_level != 99)
    {
      quality_level_db = (int8_t) (-113 + 2*quality_level);
      printf("Signal Quality Level: %d dBm (Quectel format: %ld) \n", quality_level_db, quality_level);
    }
    else
    {
      printf("Signal Quality Level not detectable \n");
    }
   }

   switch (reg_status) {
    case C2C_REGISTATUS_HOME_NETWORK:
    case C2C_REGISTATUS_ROAMING:
#ifdef USE_BG96
    case C2C_REGISTATUS_UNKNOWN:
#endif
      BSP_LED_On(LED_GREEN);
      ret = C2C_RET_OK;
      printf("C2C module registered\n");
      break;
    case C2C_REGISTATUS_TRYING:
      printf("C2C registration trying\n");
      break;
    case C2C_REGISTATUS_REG_DENIED:
      printf("C2C registration denied\n");
      break;
    case C2C_REGISTATUS_NOT_REGISTERED:
      printf("C2C registration failed\n");
      break;
    case C2C_REGISTATUS_ERROR:
      printf("C2C AT communication error with the C2C device\n");
      printf("C2C device might be disconnected or wrongly connected\n");
      break;
    case C2C_REGISTATUS_SIM_NOT_INSERTED:
      printf("SIM is not inserted\n");
      break;
    default:
      printf("C2C SIM error: %d \n", reg_status);
      printf("Please check if SIM is inserted & valid, if credentials are ok, etc.\n");
      break;
   }

   if((ret != C2C_RET_OK) && (use_internal_sim == false)) {
    /* first time we've tried external SIM, now try embedded one */
    use_internal_sim = true;
   }
   else
   {
    /*  we've already tried both or ret is OK, so let's quit */
    break;
   }
  }


  if (ret == C2C_RET_OK)
  {
    /* Retrieve the C2C current cellularoperator to confirm that it is detected and communicating. */
    printf("Retrieving the cellular operator: ");
    ret = C2C_GetCurrentOperator(OperatorsString, sizeof(OperatorsString));
    if (ret == C2C_RET_OK)
    {
      printf("%s\n", OperatorsString);
    }
    else
    {
      printf("Failed to get current cellular operator name\n");
    }
    /* Connect to the specified APN. */

    ret = C2C_ConfigureAP(1, oper_ap_code, username, password, 0);           /* Emnify SIM */
    //ret = C2C_ConfigureAP(1,"ESEYE1","","",0);     /* Eseye SIM */
    /* printf("\rConnecting to AP: be patient ...\n"); */
    HAL_Delay(1000);
    do
    {
      ret = C2C_Connect();
      c2cConnectCounter++;
      if (ret == C2C_RET_OK)
      {
        c2cConnectCounter = C2C_CONNECT_MAX_ATTEMPT_COUNT;
      }
      else
      {
        printf(" Connection try nr %d of %d failed", c2cConnectCounter, C2C_CONNECT_MAX_ATTEMPT_COUNT);
      }
    }
    while (c2cConnectCounter < C2C_CONNECT_MAX_ATTEMPT_COUNT);

    BSP_LED_Off(LED_GREEN);
    if (ret == C2C_RET_OK)
    {
      printf("Connected to AP\n");
    }
    else
    {
      printf("Failed to connect to AP\n");
    }
  }
  return (ret == C2C_RET_OK) ? 0 : -1;
}


int net_if_deinit(void * if_ctxt)
{
  // TODO: Anything to do here?
  return 0;
}


int net_if_reinit(void * if_ctxt)
{
  C2C_Ret_t ret = C2C_RET_ERROR;
  C2C_RegiStatus_t reg_status = C2C_REGISTATUS_UNKNOWN;
  int32_t quality_level = 0;
  int8_t quality_level_db = 0;
  uint8_t c2cConnectCounter = 0;
  bool use_internal_sim = false;
  const char *oper_ap_code = NULL;
  const char *username = NULL;
  const char *password = NULL;


  if (checkC2cCredentials(&oper_ap_code, &username, &password, &use_internal_sim) != HAL_OK)
  {
    msg_error("Invalid C2C configuration in Flash.\n");
    return -1;
  }

  C2C_HwResetAndPowerUp();
  /* wait additional 2 sec: even the modem is ON, registering can fail if request is done too early */
  HAL_Delay(2000);

  printf("Re-initializing the C2C module\n");
  reg_status =  C2C_Init(30);

  if (reg_status != C2C_REGISTATUS_ERROR)
  {
    /* Retrieve the quality level of the connectio  */
    C2C_GetSignalQualityStatus(&quality_level);

    if (quality_level != 99)
    {
      quality_level_db = (int8_t) (-113 + 2*quality_level);
      printf("Signal Quality Level: %d dBm (Quectel format: %ld) \n", quality_level_db, quality_level);
    }
    else
    {
      printf("Signal Quality Level not detectable \n");
    }
  }

  switch (reg_status) {
    case C2C_REGISTATUS_HOME_NETWORK:
    case C2C_REGISTATUS_ROAMING:
      BSP_LED_On(LED_GREEN);
      ret = C2C_RET_OK;
      printf("C2C module registered\n");
      break;
    case C2C_REGISTATUS_TRYING:
      printf("C2C registration trying\n");
      break;
    case C2C_REGISTATUS_REG_DENIED:
      printf("C2C registration denied\n");
      break;
    case C2C_REGISTATUS_NOT_REGISTERED:
      printf("C2C registration failed\n");
      break;
    case C2C_REGISTATUS_ERROR:
      printf("C2C AT communication error with the C2C device\n");
      printf("C2C device might be disconnected or wrongly connected\n");
      break;
    default:
      printf("C2C SIM error: %d \n", reg_status);
      printf("Please check if SIM is inserted & valid, if credentials are ok, etc.\n");
      break;
  }

  if (ret == C2C_RET_OK)
  {
    /* Retrieve the C2C current cellularoperator to confirm that it is detected and communicating. */
    printf("Retrieving the cellular operator: ");
    ret = C2C_GetCurrentOperator(OperatorsString, sizeof(OperatorsString));
    if (ret == C2C_RET_OK)
    {
      printf("%s\n", OperatorsString);
    }
    else
    {
      printf("Failed to get current cellular operator name\n");
    }
    /* Connect to the specified APN. */

    printf("\n");
    ret = C2C_ConfigureAP(1, oper_ap_code, username, password, 0);

    /* printf("\rConnecting to AP: be patient ...\n"); */
    
    HAL_Delay(1000);
    do
    {
      ret = C2C_Connect();
      c2cConnectCounter++;
      if (ret == C2C_RET_OK)
      {
        c2cConnectCounter = C2C_CONNECT_MAX_ATTEMPT_COUNT;
        printf("Connected to AP\n");
      }
      else
      {
        printf(" Connection try nr %d of %d failed", c2cConnectCounter, C2C_CONNECT_MAX_ATTEMPT_COUNT);
      }
    }
    while (c2cConnectCounter < C2C_CONNECT_MAX_ATTEMPT_COUNT);

    BSP_LED_Off(LED_GREEN);
    if (ret == C2C_RET_OK)
    {
      printf("\nConnected to AP \n");
    }
    else
    {
      printf("\nFailed to connect to AP \n");
    }
  }

  return (ret == C2C_RET_OK) ? 0 : -1;
}

#endif /* USE_C2C */

