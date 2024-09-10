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

#include "stm32u5xx_nucleo.h"
#include "gnss1a1_conf.h"
#include "gnss1a1_gnss.h"
#include "teseo_liv3f_conf.h"
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
/* Tasks handle */
TX_THREAD teseoConsumerTaskHandle;
TX_THREAD consoleParseTaskHandle;
#if (USE_I2C == 1)
  TX_THREAD backgroundTaskHandle;
#endif /* USE_I2C */

/* Private variables ---------------------------------------------------------*/
/* User Button flag */
static int btnRst = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static UINT Teseo_Consumer_Task_Init(VOID *memory_ptr);
static UINT Console_Parse_Task_Init(VOID *memory_ptr);
#if (USE_I2C == 1)
  static UINT Background_Task_Init(VOID *memory_ptr);
#endif /* USE_I2C */

static void TeseoConsumerTask(ULONG argument);
static void ConsoleParseTask(ULONG argument);
#if (USE_I2C == 1)
  static void BackgroundTask(ULONG argument);
#endif /* USE_I2C */

static int ConsoleReadable(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void MX_GNSS_PreOSInit(void)
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
                         pointer, BACKGROUND_STACK_SIZE, TESEO_BACKGROUND_TASK_PRIORITY, teseo_background_task_preemption_th,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  return ret;
}
#endif /* USE_I2C */

/* This function creates the task reading input from the cocsole */
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
                         pointer, CONSOLE_STACK_SIZE, TESEO_CONSOLE_TASK_PRIORITY, teseo_console_task_preemption_th,
                         TX_NO_TIME_SLICE, TX_AUTO_START);

  return ret;
}

/* TeseoConsumerTask function */
static void TeseoConsumerTask(ULONG argument)
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
static void BackgroundTask(ULONG argument)
{
  for(;;)
  {
    GNSS1A1_GNSS_BackgroundProcess(GNSS1A1_TESEO_LIV3F);
  }
}
#endif /* USE_I2C */

/* Serial Port Parse Task function */
static void ConsoleParseTask(ULONG argument)
{
  int32_t status;
  uint8_t buffer[128];
  uint16_t rxLen;
  GNSS1A1_GNSS_Msg_t gnssMsg;

  for(;;)
  {
    while(!ConsoleReadable())
    {
      tx_thread_relinquish();
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

