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
#include "cmsis_os.h"

#include "stm32f4xx_nucleo.h"
#include "gnss1a1_conf.h"
#include "gnss1a1_gnss.h"
#include "teseo_liv3f_conf.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define CONSOLE_STACK_SIZE      1024
#define CONSUMER_STACK_SIZE     1024
#if (USE_I2C == 1)
  #define BACKGROUND_STACK_SIZE 1024
#else
  #define BACKGROUND_STACK_SIZE    0   /* in case of USART bus, the background task is not used */
#endif /* USE_I2C */

/* Global variables ----------------------------------------------------------*/
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
/* User Button flag */
static int btnRst = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void Teseo_Consumer_Task_Init(void);
static void Console_Parse_Task_Init(void);
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

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void MX_GNSS_Init(void)
{
  /* USER CODE BEGIN GNSS_Init_PreTreatment */

  /* USER CODE END GNSS_Init_PreTreatment */

  /* Initialize the BSP common utilities */
  if(BSP_COM_Init(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

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

/* This function creates the task reading input from the cocsole */
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

/* TeseoConsumerTask function */
#if (osCMSIS < 0x20000U)
static void TeseoConsumerTask(void const *argument)
#else
static void TeseoConsumerTask(void *argument)
#endif /* osCMSIS */
{
  const GNSS1A1_GNSS_Msg_t *gnssMsg;

  GNSS1A1_GNSS_Init(GNSS1A1_TESEO_LIV3F);

  for(;;)
  {
    if(btnRst == 1)
    {
      GNSS1A1_GNSS_Reset(GNSS1A1_TESEO_LIV3F);
      btnRst = 0;
    }

    gnssMsg = GNSS1A1_GNSS_GetMessage(GNSS1A1_TESEO_LIV3F);
    if(gnssMsg == NULL)
    {
      continue;
    }

    PRINT_OUT((char*)gnssMsg->buf);
    GNSS1A1_GNSS_ReleaseMessage(GNSS1A1_TESEO_LIV3F, gnssMsg);
  }
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

/* Serial Port Parse Task function */
#if (osCMSIS < 0x20000U)
static void ConsoleParseTask(void const *argument)
#else
static void ConsoleParseTask(void *argument)
#endif /* osCMSIS */
{
  int32_t status;
  uint8_t buffer[128];
  uint16_t rxLen;
  GNSS1A1_GNSS_Msg_t gnssMsg;

  for(;;)
  {
    while(!ConsoleReadable())
    {
      osThreadYield();
    }
    memset(buffer, 0, sizeof(buffer));
    HAL_UART_Receive(&hcom_uart[COM1], buffer, sizeof(buffer), 100);
    rxLen = strlen((char *)buffer);

    gnssMsg.buf = buffer;
    gnssMsg.len = rxLen;
    status = GNSS1A1_GNSS_Send(GNSS1A1_TESEO_LIV3F, &gnssMsg);

    if (status != BSP_ERROR_NONE)
    {
      PRINT_OUT("Error sending command\n\n");
    }
  }
}

/**
 * @brief  BSP Push Button callback
 * @param  Button Specifies the pin connected EXTI line
 * @retval None.
 */
void BSP_PB_Callback(Button_TypeDef Button)
{
  btnRst = 1;
}

static int ConsoleReadable(void)
{
  /*
   * To avoid a target blocking case, let's check for
   *  possible OVERRUN error and discard it
   */
  if(__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_ORE)) {
    __HAL_UART_CLEAR_OREFLAG(&hcom_uart[COM1]);
  }
  // Check if data is received
  return (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_RXNE) != RESET) ? 1 : 0;
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

