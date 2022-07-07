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

#include "stm32f4xx_nucleo.h"
#include "gnss1a1_conf.h"
#include "gnss1a1_gnss.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
#define BUFFER_SIZE		(16)

/* Global variables ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t fromPC[BUFFER_SIZE];
static uint8_t fromGNSS[BUFFER_SIZE];
static volatile unsigned long idxPC;
static volatile unsigned long idxGNSS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void MX_GNSS_Init(void)
{
  /* USER CODE BEGIN GNSS_Init_PreTreatment */

  /* USER CODE END GNSS_Init_PreTreatment */

  idxPC = 0;
  idxGNSS = 0;

  if(BSP_COM_Init(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  GNSS1A1_GNSS_Init(GNSS1A1_TESEO_LIV3F);

  /* USER CODE BEGIN GNSS_Init_PostTreatment */

  /* USER CODE END GNSS_Init_PostTreatment */
}

void MX_GNSS_Process(void)
{
  /* USER CODE BEGIN GNSS_Process_PreTreatment */

  /* USER CODE END GNSS_Process_PreTreatment */

  if (HAL_UART_Receive(&hcom_uart[COM1], &fromPC[idxPC], 1, 0) == HAL_OK)
  {
    HAL_UART_Transmit(&hgnss_uart, &fromPC[idxPC], 1, 0);
    idxPC++;
    idxPC %= BUFFER_SIZE;
  }

  if (HAL_UART_Receive(&hgnss_uart, &fromGNSS[idxGNSS], 1, 0) == HAL_OK)
  {
    HAL_UART_Transmit(&hcom_uart[COM1], &fromGNSS[idxGNSS], 1, 0);
    idxGNSS++;
    idxGNSS %= BUFFER_SIZE;
  }

  /* USER CODE BEGIN GNSS_Process_PostTreatment */

  /* USER CODE END GNSS_Process_PostTreatment */
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

