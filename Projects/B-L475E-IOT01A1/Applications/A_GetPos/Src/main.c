/**
  *******************************************************************************
  * @file    main.c
  * @author  SRA
  * @brief   This application shows how to enable Assisted-GNSS feature
  *          (both Predictive and Real-Time type) supported by the Teseo-LIV3F device.
  *          To this aim, the application needs to access the Internet and is customized
  *          for B-L475E-IOT01 providing the WiFi connectivity. For more details about Assisted GNSS,
  *          please refer to AN5160: RxNetworks Assisted GNSS Server Interface Specification
  *          and UM2399: ST Teseo III binary image - User manual available at st.com
  *          In addition, this application shows how real time GNSS data received by the GNSS
  *          Teseo-LIV3F device can be displayed through a serial connection and 
  *          a serial terminal on a PC.
  *
  * \section A_GetPos_Example_Description Example Description
   Main function to show how to enable the Assisted-GNSS feature supported by the Teseo-LIV3F device.
   Data received by the Teseo-LIV3F device can be displayed, through a serial connection and a serial terminal, on a PC.
   The cJSON middleware support is included to parse the messages coming from the Assisted-GNSS network provider.
   The A_GetPos application is built on top of the FreeRTOS support introducing
   a task (consumer) to parse the messages (enqueued in a shared queue)
   coming from the Teseo-LIV3F device; and a task (listener) to parse commands coming from the serial terminal.
   Furthermore this sample application shows three advanced features supported by the Teseo-LIV3F device:
   - \c Geofencing
   - \c Odometer
   - \c DataLogging
   
   The development of the application includes contribution by APG Division.
   The Teseo-LIV3F device sends via a UART (or I2C) interface the received GNSS data to the STM32 
   microcontroller, hosted on the B-L574E-IOT01 board, according to the NMEA 0183 Version 4.0 protocol.
   The user can decide at compile time the interface to receive data (setting the macro configUSE_I2C in file
   gnss_app_cfg.h).
   The user can enable a specific feature at compile time (setting the corresponding macro in file gnss_app_cfg.h).
   This A_GetPos sample application is able to:
   - establish a serial connection between the B-L574E-IOT01 and X-NUCLEO-GNSS1 boards and 
        the PC.
   - enable the Assisted-GNSS feature (both Predictive and Real-Time).
   - allow the user to select among different options for getting in a human readable format 
        information related to the acquired GNSS position, the satellites in view, the active 
        satellites, and more.

   This A_GetPos sample application allows also the user to run commands enabling three advanced features:
   - \c Geofencing - allows the Teseo-LIV3F receiver to raise a NMEA message when the resolved GNSS position is close to or entering or exiting from a specific circle
   - \c Odometer - provides information on the traveled distance using only the resolved GNSS position
   - \c DataLogging - allows the Teseo-LIV3F receiver to save locally on the flash the resolved GNSS position to be retrieved on demand from the Host

   After connecting the B-L574E-IOT01 board and the X-NUCLEO-GNSS1A1 expansion board and the 
   multi antenna to the connector on the X-NUCLEO-GNSS1A1 expansion board,
   connect the B-L574E-IOT01 board to your PC.
   Drag and drop A_GetPos*.bin (in Binary folder) on B-L574E-IOT01 drive.

   Run a Serial Terminal (e.g. TeraTerm) on your PC and open a serial connection using the 
   following parameters:
   - \c baud_rate: 115200
   - \c data: 8 bit
   - \c parity: none
   - \c stop: 1bit
   - \c flow_control: none
   - \c transmit_delay: 5msec/char, 5msec/line

   Reset the B-L574E-IOT01 board and select an option from the main menu appearing on Serial Terminal.

  * \section A_GetPos_HW_SW_Env Hardware and Software environment

  - This example runs on B-L574E-IOT01 board with GNSS STM32 expansion board
    (X-NUCLEO-GNSS1A1)

  * \section A_GetPos_Usage Usage 

 In order to make the program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
 - Open IAR toolchain (this firmware has been successfully tested with
   Embedded Workbench V7.80.4 and V8.20.2).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.24) or the System Workbench for
   STM32 (this firmware has been successfully tested with Version 2.3.1).
 - The Preprocessor symbol ASSISTED_GNSS should be included in project option.
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.
  * 
  *******************************************************************************
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "httpclient.h"
#include "a_gnss.h"

#include "cmsis_os.h"
#include "gnss_utils.h"
#include "a_gnss.h"

#include "gnss_data.h"
#include "gnss_if.h"
#include "gnss1a1_conf.h"
#include "gnss1a1_gnss.h"

#if (configUSE_FEATURE == 1)
#include "gnss_feature_cfg_data.h"
#endif /* configUSE_FEATURE */

/* Private defines -----------------------------------------------------------*/

#define CONSUMER_STACK_SIZE (1*1024)

/* Global variables ----------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
RNG_HandleTypeDef hrng;
net_hnd_t         hnet;

/* Mutex for GNSS data access */
osMutexId gnssDataMutexHandle;

/* Tasks handle */
osThreadId teseoConsumerTaskHandle;
osThreadId consoleParseTaskHandle;
#if (USE_I2C == 1)
osThreadId backgroundTaskHandle;
#endif /* USE_I2C */

/* Private variables ---------------------------------------------------------*/

static GNSSParser_Data_t GNSSParser_Data;
static int gnss_feature = 0x0;

static UART_HandleTypeDef console_uart;
static volatile uint8_t button_flags = 0;

/* Global function prototypes ------------------------------------------------*/
void Error_Handler(void);

/* Private function prototypes -----------------------------------------------*/
static void GPIO_Config(void);
static void SystemClock_Config(void);

static void Teseo_Consumer_Task_Init(void);
#if (USE_I2C == 1)
static void Background_Task_Init(void);
#endif /* USE_I2C */
static void TeseoConsumerTask(void const * argument);

static void GNSSData_Mutex_Init(void);
static void Console_Parse_Task_Init(void);
static void ConsoleParseTask(void const * argument);

static void ConsoleRead(uint8_t *string);
static int ConsoleReadable(void);

#if (USE_I2C == 1)
static void BackgroundTask(void const * argument);
#endif /* USE_I2C */

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

static void Button_ISR(void);
void SPI_WIFI_ISR(void);

static void Console_UART_Init(void);

static void RTC_Init(void);

int main(void)
{  
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset all peripherals and initialize the Flash interface and the Systick. */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();

  Periph_Config();
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* RNG init function */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* RTC init */
  RTC_Init();
  /* UART console init */
  Console_UART_Init();

  BSP_LED_Init(LED_GREEN);

  GPIO_Config();

  /* Create the thread(s) */
  Console_Parse_Task_Init();

#if (USE_I2C == 1)
  Background_Task_Init();
#endif /* USE_I2C */
  Teseo_Consumer_Task_Init();

  //HAL_Delay(100);
  PRINT_OUT("Booting...\r\n");

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */
  
  /* Infinite loop */
  while (1) {}
  
}

/*	
 * This function creates the Mutex for GNSS Data access
 */
static void GNSSData_Mutex_Init(void)
{
  osMutexDef(mutex1);
  gnssDataMutexHandle = osMutexCreate(osMutex(mutex1));
}

/*	
 * This function creates the task reading the messages coming from Teseo
 */
static void Teseo_Consumer_Task_Init(void)
{
  osThreadDef(teseoConsumerTask, TeseoConsumerTask, osPriorityNormal, 0, CONSUMER_STACK_SIZE);

  teseoConsumerTaskHandle = osThreadCreate(osThread(teseoConsumerTask), NULL);
}

/*	
 * This function creates the task reading input from the console
 */
static void Console_Parse_Task_Init(void)
{
  osThreadDef(consoleParseTask, ConsoleParseTask, osPriorityNormal, 0, 1024);
  consoleParseTaskHandle = osThreadCreate(osThread(consoleParseTask), NULL);
}

#if (USE_I2C == 1)
/* This function creates a background task for I2C FSM */
static void Background_Task_Init(void)
{
  osThreadDef(backgroundTask, BackgroundTask, osPriorityNormal, 0, 128);
  backgroundTaskHandle = osThreadCreate(osThread(backgroundTask), NULL);
}
#endif /* USE_I2C */

/*
 * Configures GPIO
 */
static void GPIO_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * System Clock Configuration 
 */
static void SystemClock_Config(void)
{

/* The SystemClock_Config() body for L4 already present
 * in the X-CUBE-GNNS1 package (GetPos application) 
 * has been replaced by the one included in the CLD_GEN_V1.0.0 Function Pack
 * B-L475E-IOT01_httpclient example (RTC_Init() issue)
 */

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Enable MSI PLL mode */
  HAL_RCCEx_EnableMSIPLLMode();
}

#if (USE_I2C == 1)
/* BackgroundTask function */
static void BackgroundTask(void const * argument)
{
  for(;;)
  {
    GNSS1A1_GNSS_BackgroundProcess(GNSS1A1_TESEO_LIV3F);
  }
}
#endif /* USE_I2C */

/* TeseoConsumerTask function */
void TeseoConsumerTask(void const * argument)
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
  
  /* Create the mutex for accessing the GNSS Data */
  GNSSData_Mutex_Init();

  //PRINT_OUT("\n\rTeseo Consumer Task running\n\r");
  GNSS_PARSER_Init(&GNSSParser_Data);

  for(;;)
  {    
    gnssMsg = GNSS1A1_GNSS_GetMessage(GNSS1A1_TESEO_LIV3F);
    if(gnssMsg == NULL)
    {
      continue;
    }
    
    check = GNSS_PARSER_CheckSanity((uint8_t *)gnssMsg->buf, gnssMsg->len);

//    PRINT_OUT("got ");
//    (check == GNSS_PARSER_OK) ? PRINT_OUT("Good sentence: ") : PRINT_OUT("!!!Bad sentence: ");
//    PRINT_OUT((char *)gnssMsg->buf);
//    PRINT_OUT("\n\r");


    if(check != GNSS_PARSER_ERROR){

      for(int m = 0; m < NMEA_MSGS_NUM; m++) {

        osMutexWait(gnssDataMutexHandle, osWaitForever);
        status = GNSS_PARSER_ParseMsg(&GNSSParser_Data, (eNMEAMsg)m, (uint8_t *)gnssMsg->buf);
        osMutexRelease(gnssDataMutexHandle);

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMVER)) {
          GNSS_DATA_GetPSTMVerInfo(&GNSSParser_Data);
        }

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMPASSRTN)) {
          GNSS_DATA_GetPSTMPassInfo(&GNSSParser_Data);
        }

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMAGPSSTATUS)) {
          GNSS_DATA_GetPSTMAGPSInfo(&GNSSParser_Data);
        }

#if (configUSE_GEOFENCE == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMGEOFENCE)) {
          GNSS_DATA_GetGeofenceInfo(&GNSSParser_Data);
        }
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMODO)) {
          GNSS_DATA_GetOdometerInfo(&GNSSParser_Data);
        }
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMDATALOG)) {
          GNSS_DATA_GetDatalogInfo(&GNSSParser_Data);
        }
#endif /* configUSE_DATALOG */
 
        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMSGL)) {
          GNSS_DATA_GetMsglistAck(&GNSSParser_Data);
        }

        if((status != GNSS_PARSER_ERROR) && ((eNMEAMsg)m == PSTMSAVEPAR)) {
          GNSS_DATA_GetGNSSAck(&GNSSParser_Data);
        }

      }
    }

    GNSS1A1_GNSS_ReleaseMessage(GNSS1A1_TESEO_LIV3F, gnssMsg);

  }
}

/* ConsoleParseTask function */
void ConsoleParseTask(void const * argument)
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
    HAL_UART_Receive(&console_uart, &ch, 1, 100);

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

/*	
 * This function reads a string from the console and stores it in the 'string' param
 */
static void ConsoleRead(uint8_t *string)
{
  uint8_t ch;

  while(1)
  {
    ch = '\0';

    HAL_UART_Receive(&console_uart, &ch, 1, 1000);

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

/* Parse the input command from console */
static void AppCmdProcess(char *com)
{    
  uint8_t tracks[MAX_STR_LEN];
  uint8_t secs[MAX_STR_LEN];
  char msg[128];
  uint8_t status;
  static int32_t tracked = 0;
  int seedMask;

  // 0 - FWUPG
  if((strcmp((char *)com, "0") == 0 || strcmp((char *)com, "agnss") == 0)) {

    PRINT_OUT("Type \"GENPASS\" to get password\r\n");
    PRINT_OUT("Type \"DOWNLOAD-PR-DATA,x\" - x constellation flag:\r\n");
    PRINT_OUT("G or g: only GPS \n");
    PRINT_OUT("R or r: only GLO \n");
    PRINT_OUT("E or e: only GAL \n");
    PRINT_OUT("C or c: only BEI \n");
    //PRINT_OUT("A or a: FULL constellation GPS + GLO + GAL\n");

    PRINT_OUT("Type \"DOWNLOAD-RT-DATA,x\" - x constellation flag:\r\n");
    PRINT_OUT("G or g: only GPS \n");
    PRINT_OUT("R or r: only GLO \n");
    PRINT_OUT("E or e: only GAL \n");
    PRINT_OUT("C or c: only BEI \n");

    PRINT_OUT("Type \"GETAGPSSTATUS\" to get A-GNSS status\r\n");
//    PRINT_OUT("Type \"SCALE-CLOCK-DOWN\" after conversion completed\r\n");

    showPrompt();
  }

  // GENPASS
  else if(strncmp((char *)com, "GENPASS", strlen("GENPASS")) == 0) {
    AppAGNSS_PassGen();
  }
  // DOWNLOAD-PR-DATA,x
  else if(strncmp((char *)com, "DOWNLOAD-PR-DATA", strlen("DOWNLOAD-PR-DATA")) == 0) {
    seedMask = AppAGNSS_ParseSeedOp(com);
    if(seedMask != 0)
    {
      if(AppAGNSS_DownloadSeed(&GNSSParser_Data, seedMask, PR_SEED) == 0)
      {
//        GNSS_DATA_SendCommand(&pGNSS, "$PSTMGPSSUSPEND");
        AppAGNSS_ConvertPRSeed(seedMask);
      }
    }
  }
  // DOWNLOAD-RT-DATA,x
  else if(strncmp((char *)com, "DOWNLOAD-RT-DATA", strlen("DOWNLOAD-RT-DATA")) == 0) {
    seedMask = AppAGNSS_ParseSeedOp(com);
    if(seedMask != 0)
    {
      if(AppAGNSS_DownloadSeed(&GNSSParser_Data, seedMask, RT_SEED) == 0)
      {
        AppAGNSS_ConvertRTSeed(seedMask);
      }
    }
  }
  // GETAGPSSTATUS
  else if(strncmp((char *)com, "GETAGPSSTATUS", strlen("GETAGPSSTATUS")) == 0) {
    GNSS_DATA_SendCommand((uint8_t *)"$PSTMGETAGPSSTATUS");
  }
  // SCALE CLOCK DOWN
//  else if(strncmp((char *)com, "SCALE-CLOCK-DOWN", strlen("SCALE-CLOCK-DOWN")) == 0) {
//    GNSS_DATA_SendCommand(&pGNSS, "$PSTMGPSRESTART");
//  }

  else if(strcmp((char *)com, "y") == 0) {
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

	else if(strcmp((char *)com, "n") == 0) {
    PRINT_OUT("\r\n>"); 
  }
	
  // 1 - GETPOS / 2 - LASTPOS
  else if((strcmp((char *)com, "1") == 0 || strcmp((char *)com, "getpos") == 0) ||
     (strcmp((char *)com, "2") == 0 || strcmp((char *)com, "lastpos") == 0)) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetValidInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 3 - WAKEUPSTATUS
  else if(strcmp((char *)com, "3") == 0 || strcmp((char *)com, "wakestatus") == 0) {
    GNSS1A1_GNSS_Wakeup_Status(GNSS1A1_TESEO_LIV3F, &status);
    
    PRINT_OUT("WakeUp Status: "); 
    status == 0 ? PRINT_OUT("0") : PRINT_OUT("1");
    PRINT_OUT("\r\n>");
  }   
  
  // 4 - HELP
  else if(strcmp((char *)com, "4") == 0 || strcmp((char *)com, "help") == 0) {
    showCmds();
  }

  // 5 - DEBUG
  else if(strcmp((char *)com, "5") == 0 || strcmp((char *)com, "debug") == 0) {
    GNSSParser_Data.debug = (GNSSParser_Data.debug == DEBUG_ON ? DEBUG_OFF : DEBUG_ON);
    if(GNSSParser_Data.debug == DEBUG_OFF)
      PRINT_OUT("Debug: OFF\r\n>");
    else
      PRINT_OUT("Debug: ON\r\n>");
  }

  // 6 - TRACKPOS
  else if(strcmp((char *)com, "6") == 0 || strcmp((char *)com, "track") == 0) {
    uint32_t t, s;
    do {
      memset(tracks, 0, 16);
      sprintf(msg, "How many positions do you want to track? (max allowed %d)\r\n>", (int)MAX_STOR_POS);
      PRINT_OUT(msg);                
      ConsoleRead((uint8_t *)tracks);
    } while(atoi((char *)tracks) < 0 || atoi((char *)tracks) > MAX_STOR_POS);
    do {
      memset(secs, 0, 16);
      PRINT_OUT("How many seconds do you want to delay while tracking? (>= 0)\r\n> ");
      ConsoleRead((uint8_t *)secs);
    } while(atoi((char *)secs) < 0);
    t = strtoul((char *)tracks, NULL, 10);
    s = strtoul((char *)secs, NULL, 10);

    tracked = GNSS_DATA_TrackGotPos(&GNSSParser_Data, t, s);
    if(tracked > 0){
      PRINT_OUT("Last tracking process went good.\r\n\n>");  
    }
    else
      PRINT_OUT("Last tracking process went bad.\r\n\n>");        
  }

  // 7 - LASTTRACK
  else if(strcmp((char *)com, "7") == 0 || strcmp((char *)com, "lasttrack") == 0) {
    if(tracked > 0){
      PRINT_OUT("Acquired positions:\r\n");
      GNSS_DATA_PrintTrackedPositions(tracked);
    }
    else
      PRINT_OUT("Last tracking process went bad.\r\n\n>");
  }
  
  // 8 - GETFWVER
  else if(strcmp((char *)com, "8") == 0 || strcmp((char *)com, "getfwver") == 0) {
    PRINT_OUT("Type \"$PSTMGETSWVER\"   to get the GNSSLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,1\" to get the OS20LIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,2\" to get the GPSAPP version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,4\" to get the WAASLIB version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,6\" to get the BINIMG version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,7\" to get the board version \r\n");
    PRINT_OUT("Type \"$PSTMGETSWVER,8\" to get the STAGPSLIB version \r\n");
    PRINT_OUT("\nType the command now:\r\n> ");
  }
  
  // GETFWVER,x
  else if(strncmp((char *)com, "$PSTMGETSWVER", strlen("$PSTMGETSWVER")) == 0) {
    GNSS_DATA_SendCommand((uint8_t *)com);
  }

  // 9 - GET Fix data for single or combined Satellite navigation system
  else if(strcmp((char *)com, "9") == 0 || strcmp((char *)com, "getgnsmsg") == 0) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGNSInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 10 - GET GPS Pseudorange Noise Statistics
  else if(strcmp((char *)com, "10") == 0 || strcmp((char *)com, "getgpgst") == 0) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGPGSTInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 11 - GET Recommended Minimum Specific GPS/Transit data
  else if(strcmp((char *)com, "11") == 0 || strcmp((char *)com, "getgprmc") == 0) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGPRMCInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

  // 12 - GET GPS DOP and Active Satellites
  else if(strcmp((char *)com, "12") == 0 || strcmp((char *)com, "getgsamsg") == 0) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGSAInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }
  
  // 13 - GET GPS Satellites in View
  else if(strcmp((char *)com, "13") == 0 || strcmp((char *)com, "getgsvmsg") == 0) {
    osMutexWait(gnssDataMutexHandle, osWaitForever);
    GNSS_DATA_GetGSVInfo(&GNSSParser_Data);
    osMutexRelease(gnssDataMutexHandle);
  }

#if (configUSE_FEATURE == 1)
  // 14 - EN-FEATURE
  else if(strcmp((char *)com, "14") == 0 || strcmp((char *)com, "en-feature") == 0) {
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
          strncmp((char *)com, "DATALOG,0", strlen("DATALOG,0")) == 0) {
    AppEnFeature(com);
  }
#endif /* configUSE_FEATURE */

#if (configUSE_GEOFENCE == 1)
  // 15 - CONF-GEOFENCE
  else if(strcmp((char *)com, "15") == 0 || strcmp((char *)com, "conf-geofence") == 0) {
    PRINT_OUT("Type \"Geofence-Lecce\" to config circle in Lecce \r\n");
    PRINT_OUT("Type \"Geofence-Catania\" to config circle in Catania \r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }
  // GEOFENCE-CIRCLE
  else if(strncmp((char *)com, "Geofence-Lecce", strlen("Geofence-Lecce")) == 0 ||
          strncmp((char *)com, "Geofence-Catania", strlen("Geofence-Catania")) == 0) {
    AppGeofenceCfg(com);
  }
  
  // 16 - REQ-GEOFENCE
  else if(strcmp((char *)com, "16") == 0 || strcmp((char *)com, "req-geofence") == 0) {
    GNSS_DATA_SendCommand((uint8_t *)"$PSTMGEOFENCEREQ");
  }
#endif /* configUSE_GEOFENCE */

#if (configUSE_ODOMETER == 1)
  // 17 - ODOMETER (START/STOP)
  else if(strcmp((char *)com, "17") == 0 || strcmp((char *)com, "odometer-op") == 0) {
    PRINT_OUT("Type \"START-ODO\" to start odometer\r\n");
    PRINT_OUT("Type \"STOP-ODO\"  to stop odometer\r\n");
    PRINT_OUT("Type the command:\r\n> ");
  }

  // Odometer op
  else if(strncmp((char *)com, "START-ODO", strlen("START-ODO")) == 0 ||
          strncmp((char *)com, "STOP-ODO", strlen("STOP-ODO")) == 0) {
    AppOdometerOp(com);
  }
#endif /* configUSE_ODOMETER */

#if (configUSE_DATALOG == 1)  
  // 18 - DATALOG (START/STOP/ERASE)
  else if(strcmp((char *)com, "18") == 0 || strcmp((char *)com, "datalog-op") == 0) {
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
          strncmp((char *)com, "ERASE-DATALOG", strlen("ERASE-DATALOG")) == 0) {
    AppDatalogOp(com);
  }
#endif /* configUSE_DATALOG */

  // 19 - EXT-HELP
  else if(strcmp((char *)com, "19") == 0 || strcmp((char *)com, "ext-help") == 0) {
    printHelp();
  }
  
  else {
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
  if(strcmp(command, "GEOFENCE,1") == 0) {
    GNSS_DATA_EnableGeofence(1);
  }
  if(strcmp(command, "GEOFENCE,0") == 0) {
    GNSS_DATA_EnableGeofence(0);
  }
  if(strcmp(command, "ODO,1") == 0) {
    GNSS_DATA_EnableOdo(1);
  }
  if(strcmp(command, "ODO,0") == 0) {
    GNSS_DATA_EnableOdo(0);
  }
  if(strcmp(command, "DATALOG,1") == 0) {
    GNSS_DATA_EnableDatalog(1);
  }
  if(strcmp(command, "DATALOG,0") == 0) {
    GNSS_DATA_EnableDatalog(0);
  }
}
#endif /* configUSE_FEATURE */

/* Geofence configure */
#if (configUSE_GEOFENCE == 1)
static void AppGeofenceCfg(char *command)
{ 
  if(strcmp(command, "Geofence-Lecce") == 0) {
    GNSS_DATA_ConfigGeofence(&Geofence_STLecce);
  }
  if(strcmp(command, "Geofence-Catania") == 0) {
    GNSS_DATA_ConfigGeofence(&Geofence_Catania);
  }
}
#endif /* configUSE_GEOFENCE */

/* Odometer configure */
#if (configUSE_ODOMETER == 1)
static void AppOdometerOp(char *command)
{ 
  if(strcmp(command, "START-ODO") == 0) {
    GNSS_DATA_StartOdo(1);
  }
  if(strcmp((char *)command, "STOP-ODO") == 0) {
    GNSS_DATA_StopOdo();
  }
}
#endif /* configUSE_ODOMETER */

/* Datalog configure */
#if (configUSE_DATALOG == 1)
static void AppDatalogOp(char *command)
{
  if(strcmp(command, "CONFIG-DATALOG") == 0) {
    GNSS_DATA_ConfigDatalog(&SampleDatalog);
  }
  if(strcmp(command, "START-DATALOG") == 0) {
    GNSS_DATA_StartDatalog();
  }
  if(strcmp(command, "STOP-DATALOG") == 0) {
    GNSS_DATA_StopDatalog();
  }
  if(strcmp(command, "ERASE-DATALOG") == 0) {
    GNSS_DATA_EraseDatalog();
  }
}
#endif /* configUSE_DATALOG */

/*
 * This function is executed in case of error occurrence.
 */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(200);
  }
}

#ifdef USE_FULL_ASSERT

/*
 * Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif 

/**
  * RTC init function
  */
static void RTC_Init(void)
{
    /**Initialize RTC Only */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
#ifdef RTC_OUTPUT_REMAP_NONE
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
#endif
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  *  Update button ISR status
  */
static void Button_ISR(void)
{
  button_flags++;
}

/**
  * Waiting for button to be pushed
  */
uint8_t Button_WaitForPush(uint32_t delay)
{
  uint32_t time_out = HAL_GetTick()+delay;
  do
  {
    if (button_flags > 1)
    {
      button_flags = 0;
      return BP_MULTIPLE_PUSH;
    }

    if (button_flags == 1)
    {
      button_flags = 0;
      return BP_SINGLE_PUSH;
    }
  }
  while( HAL_GetTick() < time_out);
  return BP_NOT_PUSHED;
}

/**
  * EXTI line detection callback.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
  case (GPIO_PIN_13):
    {
      Button_ISR();
      break;
    }

  case (GPIO_PIN_1):
    {
      SPI_WIFI_ISR();
      break;
    }

  default:
    {
      break;
    }
  }
}

void SPI3_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi);
}

static int ConsoleReadable(void)
{
  /*  To avoid a target blocking case, let's check for
  *  possible OVERRUN error and discard it
  */
  if(__HAL_UART_GET_FLAG(&console_uart, UART_FLAG_ORE)) {
    __HAL_UART_CLEAR_OREFLAG(&console_uart);
  }
  // Check if data is received
  return (__HAL_UART_GET_FLAG(&console_uart, UART_FLAG_RXNE) != RESET) ? 1 : 0;
}

/* @cond DOXYGEN_EXCLUDE
 *
 */
#if (defined(__GNUC__) && !defined(__CC_ARM))
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
  * @brief UART console init function
  */
static void Console_UART_Init(void)
{
  console_uart.Instance = USART1;
  console_uart.Init.BaudRate = 115200;
  console_uart.Init.WordLength = UART_WORDLENGTH_8B;
  console_uart.Init.StopBits = UART_STOPBITS_1;
  console_uart.Init.Parity = UART_PARITY_NONE;
  console_uart.Init.Mode = UART_MODE_TX_RX;
  console_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  console_uart.Init.OverSampling = UART_OVERSAMPLING_16;
#ifdef UART_ONE_BIT_SAMPLE_DISABLE
  console_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  console_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
#endif
  BSP_COM_Init(COM1,&console_uart);
}

/**
  * Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&console_uart, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
  * Retargets the C library scanf function to the USART.
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character on USART2 and loop until the end of read */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&console_uart, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}

int GNSS_PRINT(char *pBuffer)
{
  if (HAL_UART_Transmit(&console_uart, (uint8_t*)pBuffer, (uint16_t)strlen((char *)pBuffer), 1000) != HAL_OK)
  {
    return 1;
  }
  fflush(stdout);

  return 0;
}

int GNSS_PUTC(char pChar)
{
  if (HAL_UART_Transmit(&console_uart, (uint8_t*)&pChar, 1, 1000) != HAL_OK)
  {
    return 1;
  }
  fflush(stdout);

  return 0;
}

/* @endcond
 *
 */

