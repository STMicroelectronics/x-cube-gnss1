/**
  ******************************************************************************
  * @file    app_gnss.c
  * @author  SRA Application Team
  * @brief   GNSS initialization and applicative code
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_gnss.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"
#include "gnss1a1_gnss.h"
#include "gnss_data.h"
#include "stm32l4xx_nucleo.h"
#include "gnss1a1_conf.h"
#include "teseo_liv3f_conf.h"
#include "gnss_feature_cfg_data.h"
#include "gnss_utils.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define CONSUMER_STACK_SIZE     1024
#define CONSOLE_STACK_SIZE      1024
#if (USE_I2C == 1)
  #define BACKGROUND_STACK_SIZE 1024
#else
  #define BACKGROUND_STACK_SIZE    0   /* in case of USART bus, the background task is not used */
#endif /* USE_I2C */

/* Global variables ----------------------------------------------------------*/
/* Mutex for GNSS data access */
#if (osCMSIS < 0x20000U)
  osMutexId gnssDataMutexHandle;
#else
  osMutexId_t gnssDataMutexHandle;
  static osMutexAttr_t mutex_attributes = {
    .name = "dataMutex"
  };
#endif /* osCMSIS */

/* Tasks handle */
#if (osCMSIS < 0x20000U)
  osThreadId teseoConsumerTaskHandle;
  osThreadId consoleParseTaskHandle;
#else
  osThreadId_t teseoConsumerTaskHandle;
  osThreadId_t consoleParseTaskHandle;
#endif /* osCMSIS */

#if (USE_I2C == 1)
#if (osCMSIS < 0x20000U)
  osThreadId backgroundTaskHandle;
#else
  osThreadId_t backgroundTaskHandle;
#endif /* osCMSIS */
#endif /* USE_I2C */

/* Private variables ---------------------------------------------------------*/
static GNSSParser_Data_t GNSSParser_Data;
static int gnss_feature = 0x0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void GNSSData_Mutex_Init(void);
static void Console_Parse_Task_Init(void);
static void ConsoleRead(uint8_t *string);
static void Teseo_Consumer_Task_Init(void);
#if (USE_I2C == 1)
static void Background_Task_Init(void);
#endif /* USE_I2C */

#if (osCMSIS < 0x20000U)
  static void TeseoConsumerTask(void const *argument);
  static void ConsoleParseTask(void const *argument);
#else
  static void TeseoConsumerTask(void *argument);
  static void ConsoleParseTask(void *argument);
#endif /* osCMSIS */
#if (USE_I2C == 1)
#if (osCMSIS < 0x20000U)
  static void BackgroundTask(void const *argument);
#else
  static void BackgroundTask(void *argument);
#endif /* osCMSIS */
#endif /* USE_I2C */

#if (osCMSIS >= 0x20000U)
  static osThreadAttr_t task_attributes = {
    .priority = (osPriority_t) osPriorityNormal
  };
#endif /* osCMSIS */

static int ConsoleReadable(void);

static void AppCmdProcess(char *com);
static void AppCfgMsgList(int lowMask, int highMask);

#if (configUSE_FEATURE == 1)
static void AppEnFeature(char *command);
#endif /* configUSE_FEATURE */

#if (configUSE_GEOFENCE == 1)
static void AppGeofenceCfg(char *command);
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
static void AppOdometerOp(char *command);
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
static void AppDatalogOp(char *command);
#endif /* configUSE_DATALOG */

static int GetSWVerCmdIsAllowed(char* com);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void MX_GNSS_Init(void)
{
  /* USER CODE BEGIN GNSS_Init_PreTreatment */

  /* USER CODE END GNSS_Init_PreTreatment */

  /* Initialize the BSP common utilities*/
  if(BSP_COM_Init(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN GNSS_Init_PostTreatment */

  /* USER CODE END GNSS_Init_PostTreatment */
}

void MX_GNSS_PreOSInit(void)
{
  /* USER CODE BEGIN GNSS_PreOSInit */

  /* USER CODE END GNSS_PreOSInit */
}

void MX_GNSS_PostOSInit(void)
{
  /* USER CODE BEGIN GNSS_PostOSInit_PreTreatment */

  /* USER CODE END GNSS_PostOSInit_PreTreatment */

  /* Initialize the tasks */
  Console_Parse_Task_Init();
#if (USE_I2C == 1)
  Background_Task_Init();
#endif /* USE_I2C */
  Teseo_Consumer_Task_Init();

  PRINT_DBG("Booting...\r\n");

  /* USER CODE BEGIN GNSS_PostOSInit_PostTreatment */

  /* USER CODE END GNSS_PostOSInit_PostTreatment */
}

/*
 * This function creates the Mutex for GNSS Data access
 */
static void GNSSData_Mutex_Init(void)
{
#if (osCMSIS < 0x20000U)
  osMutexDef(data_mutex);
  gnssDataMutexHandle = osMutexCreate(osMutex(data_mutex));
#else
  gnssDataMutexHandle = osMutexNew((osMutexAttr_t *)&mutex_attributes);
#endif /* osCMSIS */
}

/*
 * This function creates the task reading the messages coming from Teseo
 */
static void Teseo_Consumer_Task_Init(void)
{
#if (osCMSIS < 0x20000U)
  osThreadDef(teseoConsumerTask, TeseoConsumerTask, osPriorityNormal, 0, CONSUMER_STACK_SIZE);
  teseoConsumerTaskHandle = osThreadCreate(osThread(teseoConsumerTask), NULL);
#else
  task_attributes.name = "teseoConsumerTask";
  task_attributes.stack_size = CONSUMER_STACK_SIZE;
  teseoConsumerTaskHandle = osThreadNew(TeseoConsumerTask, NULL, (const osThreadAttr_t *)&task_attributes);
#endif /* osCMSIS */
}

#if (USE_I2C == 1)
/* This function creates a background task for I2C FSM */
static void Background_Task_Init(void)
{
#if (osCMSIS < 0x20000U)
  osThreadDef(backgroundTask, BackgroundTask, osPriorityNormal, 0, BACKGROUND_STACK_SIZE);
  backgroundTaskHandle = osThreadCreate(osThread(backgroundTask), NULL);
#else
  task_attributes.name = "backgroundTask";
  task_attributes.stack_size = BACKGROUND_STACK_SIZE;
  backgroundTaskHandle = osThreadNew(BackgroundTask, NULL, (const osThreadAttr_t *)&task_attributes);
#endif /* osCMSIS */
}
#endif /* USE_I2C */

static void Console_Parse_Task_Init(void)
{
#if (osCMSIS < 0x20000U)
  osThreadDef(consoleParseTask, ConsoleParseTask, osPriorityNormal, 0, CONSOLE_STACK_SIZE);
  consoleParseTaskHandle = osThreadCreate(osThread(consoleParseTask), NULL);
#else
  task_attributes.name = "consoleParseTask";
  task_attributes.stack_size = CONSOLE_STACK_SIZE;
  consoleParseTaskHandle = osThreadNew(ConsoleParseTask, NULL, (const osThreadAttr_t *)&task_attributes);
#endif /* osCMSIS */
}

#if (USE_I2C == 1)
/* BackgroundTask function */
#if (osCMSIS < 0x20000U)
static void BackgroundTask(void const *argument)
#else
static void BackgroundTask(void *argument)
#endif /* osCMSIS */
{
  for(;;)
  {
    GNSS1A1_GNSS_BackgroundProcess(GNSS1A1_TESEO_LIV3F);
  }
}
#endif /* USE_I2C */

/* TeseoConsumerTask function */
#if (osCMSIS < 0x20000U)
static void TeseoConsumerTask(void const *argument)
#else
static void TeseoConsumerTask(void *argument)
#endif /* osCMSIS */
{
  GNSSParser_Status_t status, check;
  const GNSS1A1_GNSS_Msg_t *gnssMsg;

  GNSS1A1_GNSS_Init(GNSS1A1_TESEO_LIV3F);

#if (configUSE_ODOMETER == 1)
  gnss_feature |= ODOMETER;
#endif /* configUSE_ODOMETER */

#if (configUSE_GEOFENCE == 1)
  gnss_feature |= GEOFENCE;
#endif /* configUSE_GEOFENCE */

#if (configUSE_DATALOG == 1)
  gnss_feature |= DATALOG;
#endif /* configUSE_DATALOG */

  GNSSData_Mutex_Init();
  GNSS_PARSER_Init(&GNSSParser_Data);

  for(;;)
  {
    gnssMsg = GNSS1A1_GNSS_GetMessage(GNSS1A1_TESEO_LIV3F);

    if(gnssMsg == NULL)
    {
      continue;
    }

    check = GNSS_PARSER_CheckSanity((uint8_t *)gnssMsg->buf, gnssMsg->len);

    if(check != GNSS_PARSER_ERROR)
    {
      for(int m = 0; m < NMEA_MSGS_NUM; m++)
      {
        osMutexWait(gnssDataMutexHandle, osWaitForever);
        status = GNSS_PARSER_ParseMsg(&GNSSParser_Data, (eNMEAMsg)m, (uint8_t *)gnssMsg->buf);
        osMutexRelease(gnssDataMutexHandle);

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMVER))
        {
          GNSS_DATA_GetPSTMVerInfo(&GNSSParser_Data);
        }
#if (configUSE_GEOFENCE == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMGEOFENCE))
        {
          GNSS_DATA_GetGeofenceInfo(&GNSSParser_Data);
        }
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMODO))
        {
          GNSS_DATA_GetOdometerInfo(&GNSSParser_Data);
        }
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMDATALOG))
        {
          GNSS_DATA_GetDatalogInfo(&GNSSParser_Data);
        }
#endif /* configUSE_DATALOG */
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMSGL))
        {
          GNSS_DATA_GetMsglistAck(&GNSSParser_Data);
        }

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMSAVEPAR))
        {
          GNSS_DATA_GetGNSSAck(&GNSSParser_Data);
        }
      }
    }
  GNSS1A1_GNSS_ReleaseMessage(GNSS1A1_TESEO_LIV3F, gnssMsg);

  }
}

#if (osCMSIS < 0x20000U)
static void ConsoleParseTask(void const *argument)
#else
static void ConsoleParseTask(void *argument)
#endif /* osCMSIS */
{
  char cmd[32] = {0};
  uint8_t ch;

  showCmds();
  for(;;)
  {
    ch = '\0';

    while(!ConsoleReadable())
    {
      osThreadYield();
    }
    HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, 100);

    if((ch > 31 && ch < 126))
    {
      PUTC_OUT((char)ch);
    }
    if(ch == '\r')
    {
      PRINT_OUT("\n\r");

      if (strlen(cmd) > 0)
      {
        cmd[strlen(cmd)] = '\0';
        AppCmdProcess(cmd);
        memset(cmd, 0, sizeof(cmd));
      }
      else
      {
        showPrompt();
      }
    }
    else if((ch > 31 && ch < 126))
    {
      cmd[strlen(cmd)] = ch;
    }
  }
}

static void ConsoleRead(uint8_t *string)
{
  uint8_t ch;

  while(1)
  {
    ch = '\0';

    HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, 1000);

    if((ch > 31 && ch < 126))
    {
      PUTC_OUT((char)ch);
    }
    if(ch == '\r')
    {
      PRINT_OUT("\n\r");
      string[strlen((char *)string)] = '\0';
      break;
    }
    else
    {
      if((ch > 31 && ch < 126)) {
        string[strlen((char *)string)] = ch;
      }
    }
  }
}

static void AppCmdProcess(char *com)
{
  uint8_t tracks[MAX_STR_LEN];
  uint8_t secs[MAX_STR_LEN];
  char msg[128];
  uint8_t status;
  static int32_t tracked = 0;

  if(strcmp((char *)com, "y") == 0)
  {
   /* See CDB-ID 201 - This LOW_BITS Mask enables the following messages:
    * 0x1 $GPGNS Message
    * 0x2 $GPGGA Message
    * 0x4 $GPGSA Message
    * 0x8 $GPGST Message
    * 0x40 $GPRMC Message
    * 0x80000 $GPGSV Message
    * 0x100000 $GPGLL Message
    */
    int lowMask = 0x18004F;
    int highMask = gnss_feature;
    //PRINT_DBG("Saving Configuration...");
    AppCfgMsgList(lowMask, highMask);

    PRINT_OUT("\r\n>");
  }
  // 1 - GETPOS / 2 - LASTPOS
  else if((strcmp((char *)com, "1") == 0 || strcmp((char *)com, "getpos") == 0) ||
     (strcmp((char *)com, "2") == 0 || strcmp((char *)com, "lastpos") == 0))
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetValidInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 3 - WAKEUPSTATUS
  else if(strcmp((char *)com, "3") == 0 || strcmp((char *)com, "wakestatus") == 0)
  {
    GNSS1A1_GNSS_Wakeup_Status(GNSS1A1_TESEO_LIV3F, &status);

    PRINT_OUT("WakeUp Status: ");
    status == 0 ? PRINT_OUT("0") : PRINT_OUT("1");
    PRINT_OUT("\r\n>");
  }

  // 4 - HELP
  else if(strcmp((char *)com, "4") == 0 || strcmp((char *)com, "help") == 0)
  {
    showCmds();
  }

  // 5 - DEBUG
  else if(strcmp((char *)com, "5") == 0 || strcmp((char *)com, "debug") == 0)
  {
    GNSSParser_Data.debug = (GNSSParser_Data.debug == DEBUG_ON ? DEBUG_OFF : DEBUG_ON);
    if(GNSSParser_Data.debug == DEBUG_OFF)
      PRINT_OUT("Debug: OFF\r\n>");
    else
      PRINT_OUT("Debug: ON\r\n>");
  }

  // 6 - TRACKPOS
  else if(strcmp((char *)com, "6") == 0 || strcmp((char *)com, "track") == 0)
  {
    uint32_t t, s;
    do
    {
      memset(tracks, 0, 16);
      sprintf(msg, "How many positions do you want to track? (max allowed %d)\r\n>", (int)MAX_STOR_POS);
      PRINT_OUT(msg);
      ConsoleRead((uint8_t *)tracks);
    } while(atoi((char *)tracks) < 0 || atoi((char *)tracks) > MAX_STOR_POS);
    do
    {
      memset(secs, 0, 16);
      PRINT_OUT("How many seconds do you want to delay while tracking? (>= 0)\r\n> ");
      ConsoleRead((uint8_t *)secs);
    } while(atoi((char *)secs) < 0);
    t = strtoul((char *)tracks, NULL, 10);
    s = strtoul((char *)secs, NULL, 10);

    tracked = GNSS_DATA_TrackGotPos(&GNSSParser_Data, t, s);
    if(tracked > 0)
    {
      PRINT_OUT("Last tracking process went good.\r\n\n>");
    }
    else
    {
      PRINT_OUT("Last tracking process went bad.\r\n\n>");
    }
  }

  // 7 - LASTTRACK
  else if(strcmp((char *)com, "7") == 0 || strcmp((char *)com, "lasttrack") == 0)
  {
    if(tracked > 0)
    {
      PRINT_OUT("Acquired positions:\r\n");
      GNSS_DATA_PrintTrackedPositions(tracked);
    }
    else
    {
      PRINT_OUT("Last tracking process went bad.\r\n\n>");
    }
  }

  // 8 - GETFWVER
  else if(strcmp((char *)com, "8") == 0 || strcmp((char *)com, "getfwver") == 0)
  {
    memset(com, 0, MAX_STR_LEN);

#ifdef TESEO_LIV3F_DEVICE
    PRINT_OUT("Type \"$PSTMGETSWVER\"     to get the GNSSLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,1\"   to get the OS20LIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,2\"   to get the GPSAPP version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,4\"   to get the WAASLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,6\"   to get the BINIMG version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,7\"   to get the board version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,8\"   to get the STAGPSLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,255\" to get all versions \r\n");
#endif
#ifdef TESEO_VIC3DA_DEVICE
    PRINT_OUT("Type \"$PSTMGETSWVER\"     to get the GNSSLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,1\"   to get the OS20LIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,2\"   to get the GPSAPP version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,6\"   to get the BINIMG version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,7\"   to get the board version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,8\"   to get the STAGPSLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,10\"  to get the DRLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,11\"  to get the SWCFG version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,12\"  to get the PID version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,255\" to get all versions \r\n");
#endif
    PRINT_OUT("\nType the command now:\r\n> ");
  }

  // GETFWVER,x
  else if(strncmp((char *)com, "$PSTMGETSWVER", strlen("$PSTMGETSWVER")) == 0)
  {
    if (GetSWVerCmdIsAllowed(com)) {
      GNSS_DATA_SendCommand((uint8_t *)com);
    }
    else {
      PRINT_OUT("\nCommand not valid.\r\n> ");
    }
  }

  // 9 - GET Fix data for single or combined Satellite navigation system
  else if(strcmp((char *)com, "9") == 0 || strcmp((char *)com, "getgnsmsg") == 0)
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGNSInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 10 - GET GPS Pseudorange Noise Statistics
  else if(strcmp((char *)com, "10") == 0 || strcmp((char *)com, "getgpgst") == 0)
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGPGSTInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 11 - GET Recommended Minimum Specific GPS/Transit data
  else if(strcmp((char *)com, "11") == 0 || strcmp((char *)com, "getgprmc") == 0)
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGPRMCInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 12 - GET GPS DOP and Active Satellites
  else if(strcmp((char *)com, "12") == 0 || strcmp((char *)com, "getgsamsg") == 0)
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGSAInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 13 - GET GPS Satellites in View
  else if(strcmp((char *)com, "13") == 0 || strcmp((char *)com, "getgsvmsg") == 0)
  {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGSVInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

#if (configUSE_FEATURE == 1)
  // 14 - EN-FEATURE
  else if(strcmp((char *)com, "14") == 0 || strcmp((char *)com, "en-feature") == 0)
  {
#if (configUSE_GEOFENCE == 1)
    PRINT_OUT("Type \"GEOFENCE,1\" to enable geofence\r\n");
    PRINT_OUT("Type \"GEOFENCE,0\" to disable geofence\r\n");
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
    PRINT_OUT("Type \"ODO,1\" to enable odometer\r\n");
    PRINT_OUT("Type \"ODO,0\" to disable odometer\r\n");
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
    PRINT_OUT("Type \"DATALOG,1\" to enable datalog\r\n");
    PRINT_OUT("Type \"DATALOG,0\" to disable datalog\r\n");
#endif /* configUSE_DATALOG */

    PRINT_OUT("\nType the command now:\r\n> ");
  }
#endif /* configUSE_FEATURE */

#if (configUSE_FEATURE == 1)
  else if(strncmp((char *)com, "GEOFENCE,1", strlen("GEOFENCE,1")) == 0 ||
          strncmp((char *)com, "GEOFENCE,0", strlen("GEOFENCE,0")) == 0 ||
          strncmp((char *)com, "ODO,1", strlen("ODO,1")) == 0 ||
          strncmp((char *)com, "ODO,0", strlen("ODO,0")) == 0 ||
          strncmp((char *)com, "DATALOG,1", strlen("DATALOG,1")) == 0 ||
          strncmp((char *)com, "DATALOG,0", strlen("DATALOG,0")) == 0)
  {
    AppEnFeature(com);
  }
#endif /* configUSE_FEATURE */

#if (configUSE_GEOFENCE == 1)
  // 15 - CONF-GEOFENCE
  else if(strcmp((char *)com, "15") == 0 || strcmp((char *)com, "conf-geofence") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"Geofence-Lecce\" to config circle in Lecce \r\n");
    PRINT_OUT("Type \"Geofence-Catania\" to config circle in Catania \r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }
  // GEOFENCE-CIRCLE
  else if(strncmp((char *)com, "Geofence-Lecce", strlen("Geofence-Lecce")) == 0 ||
          strncmp((char *)com, "Geofence-Catania", strlen("Geofence-Catania")) == 0)
  {
    AppGeofenceCfg(com);
  }

  // 16 - REQ-GEOFENCE
  else if(strcmp((char *)com, "16") == 0 || strcmp((char *)com, "req-geofence") == 0)
  {
    GNSS_DATA_SendCommand("$PSTMGEOFENCEREQ");
  }
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
  // 17 - ODOMETER (START/STOP)
  else if(strcmp((char *)com, "17") == 0 || strcmp((char *)com, "odometer-op") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"START-ODO\" to start odometer\r\n");
    PRINT_OUT("Type \"STOP-ODO\"  to stop odometer\r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }

  // Odometer op
  else if(strncmp((char *)com, "START-ODO", strlen("START-ODO")) == 0 ||
          strncmp((char *)com, "STOP-ODO", strlen("STOP-ODO")) == 0)
  {
    AppOdometerOp(com);
  }
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
  // 18 - DATALOG (START/STOP/ERASE)
  else if(strcmp((char *)com, "18") == 0 || strcmp((char *)com, "datalog-op") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"CONFIG-DATALOG\" to config datalog\r\n");
    PRINT_OUT("Type \"START-DATALOG\" to start datalog\r\n");
    PRINT_OUT("Type \"STOP-DATALOG\"  to stop datalog\r\n");
    PRINT_OUT("Type \"ERASE-DATALOG\"  to erase datalog\r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }
  // Datalog op
  else if(strncmp((char *)com, "CONFIG-DATALOG", strlen("CONFIG-DATALOG")) == 0 ||
          strncmp((char *)com, "START-DATALOG", strlen("START-DATALOG")) == 0 ||
          strncmp((char *)com, "STOP-DATALOG", strlen("STOP-DATALOG")) == 0 ||
          strncmp((char *)com, "ERASE-DATALOG", strlen("ERASE-DATALOG")) == 0)
  {
    AppDatalogOp(com);
  }
#endif /* configUSE_DATALOG */

  // 19 - EXT-HELP
  else if(strcmp((char *)com, "19") == 0 || strcmp((char *)com, "ext-help") == 0)
  {
    printHelp();
  }

  else
  {
    PRINT_OUT("Command not valid.\r\n\n>");
  }
}

/* CfgMessageList */
static void AppCfgMsgList(int lowMask, int highMask)
{
  GNSS_DATA_CfgMessageList(lowMask, highMask);
}

/* Enable feature */
#if (configUSE_FEATURE == 1)
static void AppEnFeature(char *command)
{
  if(strcmp(command, "GEOFENCE,1") == 0)
  {
    GNSS_DATA_EnableGeofence(1);
  }
  if(strcmp(command, "GEOFENCE,0") == 0)
  {
    GNSS_DATA_EnableGeofence(0);
  }
  if(strcmp(command, "ODO,1") == 0)
  {
    GNSS_DATA_EnableOdo(1);
  }
  if(strcmp(command, "ODO,0") == 0)
  {
    GNSS_DATA_EnableOdo(0);
  }
  if(strcmp(command, "DATALOG,1") == 0)
  {
    GNSS_DATA_EnableDatalog(1);
  }
  if(strcmp(command, "DATALOG,0") == 0)
  {
    GNSS_DATA_EnableDatalog(0);
  }
}
#endif /* configUSE_FEATURE */

/* Geofence configure */
#if (configUSE_GEOFENCE == 1)
static void AppGeofenceCfg(char *command)
{
  if(strcmp(command, "Geofence-Lecce") == 0)
  {
    GNSS_DATA_ConfigGeofence(&Geofence_STLecce);
  }
  if(strcmp(command, "Geofence-Catania") == 0)
  {
    GNSS_DATA_ConfigGeofence(&Geofence_Catania);
  }
}
#endif /* configUSE_GEOFENCE */

/* Odometer configure */
#if (configUSE_ODOMETER == 1)
static void AppOdometerOp(char *command)
{
  if(strcmp(command, "START-ODO") == 0)
  {
    GNSS_DATA_StartOdo(1);
  }
  if(strcmp((char *)command, "STOP-ODO") == 0)
  {
    GNSS_DATA_StopOdo();
  }
}
#endif /* configUSE_ODOMETER */

/* Datalog configure */
#if (configUSE_DATALOG == 1)
static void AppDatalogOp(char *command)
{
  if(strcmp(command, "CONFIG-DATALOG") == 0)
  {
    GNSS_DATA_ConfigDatalog(&SampleDatalog);
  }
  if(strcmp(command, "START-DATALOG") == 0)
  {
    GNSS_DATA_StartDatalog();
  }
  if(strcmp(command, "STOP-DATALOG") == 0)
  {
    GNSS_DATA_StopDatalog();
  }
  if(strcmp(command, "ERASE-DATALOG") == 0)
  {
    GNSS_DATA_EraseDatalog();
  }
}
#endif /* configUSE_DATALOG */

static int ConsoleReadable(void)
{
  /*  To avoid a target blocking case, let's check for
  *  possible OVERRUN error and discard it
  */
  if(__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_ORE)) {
    __HAL_UART_CLEAR_OREFLAG(&hcom_uart[COM1]);
  }
  // Check if data is received
  return (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_RXNE) != RESET) ? 1 : 0;
}

static int GetSWVerCmdIsAllowed(char* com)
{
  int ret = 0;
#ifdef TESEO_LIV3F_DEVICE
  if ((com[13] == '\0')
      || (((com[14] == '1') || (com[14] == '2') || (com[14] == '4') || (com[14] == '6') || (com[14] == '7') || (com[14] == '8')) && (com[15] == '\0'))
        || ((com[14] == '2') && (com[15] == '5') && (com[16] == '5') && (com[17] == '\0'))
     )
  {
    ret = 1;
  }
#endif
#ifdef TESEO_VIC3DA_DEVICE
  if ((com[13] == '\0')
      || (((com[14] == '1') || (com[14] == '2') || (com[14] == '6') || (com[14] == '7') || (com[14] == '8')) && (com[15] == '\0'))
        || ((com[14] == '1') && ((com[15] == '0') || (com[15] == '1') || (com[15] == '2')) && (com[16] == '\0'))
          || ((com[14] == '2') && (com[15] == '5') && (com[16] == '5') && (com[17] == '\0'))
     )
  {
    ret = 1;
  }
#endif

  return ret;
}

int GNSS_PRINT(char *pBuffer)
{
  if (HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t*)pBuffer, (uint16_t)strlen((char *)pBuffer), 1000) != HAL_OK)
  {
    return 1;
  }
  fflush(stdout);

  return 0;
}

int GNSS_PUTC(char pChar)
{
  if (HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t*)&pChar, 1, 1000) != HAL_OK)
  {
    return 1;
  }
  fflush(stdout);

  return 0;
}

