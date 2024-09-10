/**
  ******************************************************************************
  * @file    app_gnss.c
  * @author  SRA Application Team
  * @brief   GNSS initialization and applicative code
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "gnss1a1_gnss.h"
#include "gnss_data.h"
#include "stm32u5xx_nucleo.h"
#include "gnss1a1_conf.h"
#include "teseo_liv3f_conf.h"
#include "gnss_feature_cfg_data.h"
#include "gnss_utils.h"
#include "app_azure_rtos_config.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define CONSOLE_STACK_SIZE      (GNSS_THREAD_MINIMUM_STACK)
#define CONSUMER_STACK_SIZE     (GNSS_THREAD_MINIMUM_STACK)
#if (USE_I2C == 1)
  #define BACKGROUND_STACK_SIZE (GNSS_THREAD_MINIMUM_STACK)
#else
  #define BACKGROUND_STACK_SIZE 0 /* in case of USART bus, the background task is not used */
#endif /* USE_I2C */
#define SAFETY_STACK_SIZE       (GNSS_THREAD_MINIMUM_STACK)

#if (GNSS_APP_MEM_POOL_SIZE < (BACKGROUND_STACK_SIZE + CONSOLE_STACK_SIZE + CONSUMER_STACK_SIZE + SAFETY_STACK_SIZE))
  #warning "The application requires a greater GNSS_APP_MEM_POOL_SIZE!"
#endif

#define TESEO_CONSUMER_TASK_PRIORITY      1
#define TESEO_CONSOLE_TASK_PRIORITY       1
#if (USE_I2C == 1)
  #define TESEO_BACKGROUND_TASK_PRIORITY  1
#endif /* USE_I2C */

/* Global variables ----------------------------------------------------------*/
/* Mutex for GNSS data access */
static TX_MUTEX gnssDataMutexHandle;

/* Tasks handle */
TX_THREAD teseoConsumerTaskHandle;
TX_THREAD consoleParseTaskHandle;

#if (USE_I2C == 1)
  TX_THREAD backgroundTaskHandle;
#endif /* USE_I2C */

/* Private variables ---------------------------------------------------------*/
static GNSSParser_Data_t GNSSParser_Data;
static int gnss_feature = 0x0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void GNSSData_Mutex_Init(void);
static UINT Console_Parse_Task_Init(VOID *memory_ptr);
static void ConsoleRead(uint8_t *string);
static UINT Teseo_Consumer_Task_Init(VOID *memory_ptr);
#if (USE_I2C == 1)
  static UINT Background_Task_Init(VOID *memory_ptr);
#endif /* USE_I2C */

static void TeseoConsumerTask(ULONG argument);
static void ConsoleParseTask(ULONG argument);
#if (USE_I2C == 1)
  static void BackgroundTask(ULONG argument);
#endif /* USE_I2C */

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

void MX_GNSS_PreOSInit(void)
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

UINT MX_GNSS_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;

  /* USER CODE BEGIN MX_GNSS_Init */

  /* USER CODE END MX_GNSS_Init */

  /* Initialize the tasks */

  ret = Console_Parse_Task_Init(memory_ptr);
  if (ret != TX_SUCCESS) {
    return ret;
  }
#if (USE_I2C == 1)
  ret = Background_Task_Init(memory_ptr);
  if (ret != TX_SUCCESS) {
    return ret;
  }
#endif /* USE_I2C */
  ret = Teseo_Consumer_Task_Init(memory_ptr);
  if (ret != TX_SUCCESS) {
    return ret;
  }

  PRINT_DBG("Booting...\r\n");
  return ret;
}

/*
 * This function creates the Mutex for GNSS Data access
 */
static void GNSSData_Mutex_Init(void)
{
  tx_mutex_create(&gnssDataMutexHandle, "GnssDataSemaphore", TX_NO_INHERIT);
}

/*
 * This function creates the task reading the messages coming from Teseo
 */
static UINT Teseo_Consumer_Task_Init(VOID *memory_ptr)
{
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  UINT ret = TX_SUCCESS;
  CHAR *pointer;

  UINT teseo_consumer_task_preemption_th = TESEO_CONSUMER_TASK_PRIORITY;

  ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, CONSUMER_STACK_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS) {
    return ret;
  }

  ret = tx_thread_create(&teseoConsumerTaskHandle, "TeseoConsumerTask", TeseoConsumerTask, 0,
                         pointer, CONSUMER_STACK_SIZE, TESEO_CONSUMER_TASK_PRIORITY,
                         teseo_consumer_task_preemption_th,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  return ret;
}

#if (USE_I2C == 1)
/* This function creates a background task for I2C FSM */
static UINT Background_Task_Init(VOID *memory_ptr)
{
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  UINT ret = TX_SUCCESS;
  CHAR *pointer;
  UINT teseo_background_task_preemption_th = TESEO_BACKGROUND_TASK_PRIORITY;

  ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, BACKGROUND_STACK_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS) {
    return ret;
  }

  ret = tx_thread_create(&backgroundTaskHandle, "BackgroundTask", BackgroundTask, 0,
                         pointer, BACKGROUND_STACK_SIZE, TESEO_BACKGROUND_TASK_PRIORITY,
                         teseo_background_task_preemption_th,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  return ret;
}
#endif /* USE_I2C */

static UINT Console_Parse_Task_Init(VOID *memory_ptr)
{
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  UINT ret = TX_SUCCESS;
  CHAR *pointer;
  UINT teseo_console_task_preemption_th = TESEO_CONSOLE_TASK_PRIORITY;

  ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, CONSOLE_STACK_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS) {
    return ret;
  }

  ret = tx_thread_create(&consoleParseTaskHandle, "ConsoleParseTask", ConsoleParseTask, 0,
                         pointer, CONSOLE_STACK_SIZE, TESEO_CONSOLE_TASK_PRIORITY,
                         teseo_console_task_preemption_th,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  return ret;
}

#if (USE_I2C == 1)
/* BackgroundTask function */
static void BackgroundTask(ULONG argument)
{
  for(;;)
  {
    GNSS1A1_GNSS_BackgroundProcess(GNSS1A1_TESEO_LIV3F);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND/100);
  }
}
#endif /* USE_I2C */

/* TeseoConsumerTask function */
static void TeseoConsumerTask(ULONG argument)
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
        tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
        status = GNSS_PARSER_ParseMsg(&GNSSParser_Data, (eNMEAMsg)m, (uint8_t *)gnssMsg->buf);
        tx_mutex_put(&gnssDataMutexHandle);

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

static void ConsoleParseTask(ULONG argument)
{
  char cmd[32] = {0};
  uint8_t ch;

  showCmds();
  for(;;)
  {
    ch = '\0';

    while(!ConsoleReadable())
    {
      tx_thread_relinquish();
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
    /* PRINT_DBG("Saving Configuration..."); */
    AppCfgMsgList(lowMask, highMask);

    PRINT_OUT("\r\n>");
  }
  /*  1 - GETPOS / 2 - LASTPOS */
  else if((strcmp((char *)com, "1") == 0 || strcmp((char *)com, "getpos") == 0) ||
     (strcmp((char *)com, "2") == 0 || strcmp((char *)com, "lastpos") == 0))
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetValidInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

  /* 3 - WAKEUPSTATUS */
  else if(strcmp((char *)com, "3") == 0 || strcmp((char *)com, "wakestatus") == 0)
  {
    GNSS1A1_GNSS_Wakeup_Status(GNSS1A1_TESEO_LIV3F, &status);

    PRINT_OUT("WakeUp Status: ");
    status == 0 ? PRINT_OUT("0") : PRINT_OUT("1");
    PRINT_OUT("\r\n>");
  }

  /* 4 - HELP */
  else if(strcmp((char *)com, "4") == 0 || strcmp((char *)com, "help") == 0)
  {
    showCmds();
  }

  /* 5 - DEBUG */
  else if(strcmp((char *)com, "5") == 0 || strcmp((char *)com, "debug") == 0)
  {
    GNSSParser_Data.debug = (GNSSParser_Data.debug == DEBUG_ON ? DEBUG_OFF : DEBUG_ON);
    if(GNSSParser_Data.debug == DEBUG_OFF)
      PRINT_OUT("Debug: OFF\r\n>");
    else
      PRINT_OUT("Debug: ON\r\n>");
  }

  /* 6 - TRACKPOS */
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

  /* 7 - LASTTRACK */
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

  /* 8 - GETFWVER */
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

  /* GETFWVER,x  */
  else if(strncmp((char *)com, "$PSTMGETSWVER", strlen("$PSTMGETSWVER")) == 0)
  {
    if (GetSWVerCmdIsAllowed(com)) {
      GNSS_DATA_SendCommand((uint8_t *)com);
    }
    else {
      PRINT_OUT("\nCommand not valid.\r\n> ");
    }
  }

  /* 9 - GET Fix data for single or combined Satellite navigation system */
  else if(strcmp((char *)com, "9") == 0 || strcmp((char *)com, "getgnsmsg") == 0)
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetGNSInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

  /* 10 - GET GPS Pseudorange Noise Statistics */
  else if(strcmp((char *)com, "10") == 0 || strcmp((char *)com, "getgpgst") == 0)
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetGPGSTInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

  /* 11 - GET Recommended Minimum Specific GPS/Transit data  */
  else if(strcmp((char *)com, "11") == 0 || strcmp((char *)com, "getgprmc") == 0)
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetGPRMCInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

  /* 12 - GET GPS DOP and Active Satellites */
  else if(strcmp((char *)com, "12") == 0 || strcmp((char *)com, "getgsamsg") == 0)
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetGSAInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

  /* 13 - GET GPS Satellites in View  */
  else if(strcmp((char *)com, "13") == 0 || strcmp((char *)com, "getgsvmsg") == 0)
  {
    tx_mutex_get(&gnssDataMutexHandle, TX_WAIT_FOREVER);
    GNSS_DATA_GetGSVInfo(&GNSSParser_Data);
    tx_mutex_put(&gnssDataMutexHandle);
  }

#if (configUSE_FEATURE == 1)
  /* 14 - EN-FEATURE */
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
  /* 15 - CONF-GEOFENCE */
  else if(strcmp((char *)com, "15") == 0 || strcmp((char *)com, "conf-geofence") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"Geofence-Lecce\" to config circle in Lecce \r\n");
    PRINT_OUT("Type \"Geofence-Catania\" to config circle in Catania \r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }
  /* GEOFENCE-CIRCLE */
  else if(strncmp((char *)com, "Geofence-Lecce", strlen("Geofence-Lecce")) == 0 ||
          strncmp((char *)com, "Geofence-Catania", strlen("Geofence-Catania")) == 0)
  {
    AppGeofenceCfg(com);
  }

  /* 16 - REQ-GEOFENCE */
  else if(strcmp((char *)com, "16") == 0 || strcmp((char *)com, "req-geofence") == 0)
  {
    GNSS_DATA_SendCommand("$PSTMGEOFENCEREQ");
  }
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
  /* 17 - ODOMETER (START/STOP) */
  else if(strcmp((char *)com, "17") == 0 || strcmp((char *)com, "odometer-op") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"START-ODO\" to start odometer\r\n");
    PRINT_OUT("Type \"STOP-ODO\"  to stop odometer\r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }

  /* Odometer op  */
  else if(strncmp((char *)com, "START-ODO", strlen("START-ODO")) == 0 ||
          strncmp((char *)com, "STOP-ODO", strlen("STOP-ODO")) == 0)
  {
    AppOdometerOp(com);
  }
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
  /* 18 - DATALOG (START/STOP/ERASE) */
  else if(strcmp((char *)com, "18") == 0 || strcmp((char *)com, "datalog-op") == 0)
  {
    memset(com, 0, MAX_STR_LEN);
    PRINT_OUT("Type \"CONFIG-DATALOG\" to config datalog\r\n");
    PRINT_OUT("Type \"START-DATALOG\" to start datalog\r\n");
    PRINT_OUT("Type \"STOP-DATALOG\"  to stop datalog\r\n");
    PRINT_OUT("Type \"ERASE-DATALOG\"  to erase datalog\r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }
  /* Datalog op  */
  else if(strncmp((char *)com, "CONFIG-DATALOG", strlen("CONFIG-DATALOG")) == 0 ||
          strncmp((char *)com, "START-DATALOG", strlen("START-DATALOG")) == 0 ||
          strncmp((char *)com, "STOP-DATALOG", strlen("STOP-DATALOG")) == 0 ||
          strncmp((char *)com, "ERASE-DATALOG", strlen("ERASE-DATALOG")) == 0)
  {
    AppDatalogOp(com);
  }
#endif /* configUSE_DATALOG */

  /* 19 - EXT-HELP */
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
  /* Check if data is received */
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

