/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32u5xx_nucleo_bus.c
  * @brief          : source file for the BSP BUS IO driver
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_nucleo_bus.h"

__weak HAL_StatusTypeDef MX_LPUART1_UART_Init(UART_HandleTypeDef* huart);

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32U5XX_NUCLEO
  * @{
  */

/** @defgroup STM32U5XX_NUCLEO_BUS STM32U5XX_NUCLEO BUS
  * @{
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Exported_Variables BUS Exported Variables
  * @{
  */

UART_HandleTypeDef hlpuart1;
/**
  * @}
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Private_Variables BUS Private Variables
  * @{
  */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
static uint32_t IsLPUART1MspCbValid = 0;
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
static uint32_t LPUART1InitCounter = 0;

/**
  * @}
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Private_FunctionPrototypes  BUS Private Function
  * @{
  */

static void LPUART1_MspInit(UART_HandleTypeDef* huart);
static void LPUART1_MspDeInit(UART_HandleTypeDef* huart);

/**
  * @}
  */

/** @defgroup STM32U5XX_NUCLEO_LOW_LEVEL_Private_Functions STM32U5XX_NUCLEO LOW LEVEL Private Functions
  * @{
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Exported_Functions STM32U5XX_NUCLEO_BUS Exported Functions
  * @{
  */

/* BUS IO driver over UART Peripheral */
/*******************************************************************************
                            BUS OPERATIONS OVER USART
*******************************************************************************/
/**
  * @brief  Initializes USART HAL.
  * @param  Init : UART initialization parameters
  * @retval BSP status
  */
int32_t BSP_LPUART1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hlpuart1.Instance  = LPUART1;

  if(LPUART1InitCounter++ == 0)
  {
    if (HAL_UART_GetState(&hlpuart1) == HAL_UART_STATE_RESET)
    {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
      /* Init the UART Msp */
      LPUART1_MspInit(&hlpuart1);
#else
      if(IsLPUART1MspCbValid == 0U)
      {
        if(BSP_LPUART1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
#endif
      if(ret == BSP_ERROR_NONE)
      {
        /* Init the UART */
        if (MX_LPUART1_UART_Init(&hlpuart1) != HAL_OK)
        {
          ret = BSP_ERROR_BUS_FAILURE;
        }
      }
    }
  }
  return ret;
}

/**
  * @brief  DeInitializes UART HAL.
  * @retval None
  * @retval BSP status
  */
int32_t BSP_LPUART1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if (LPUART1InitCounter > 0)
  {
    if (--LPUART1InitCounter == 0)
    {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
      LPUART1_MspDeInit(&hlpuart1);
#endif
      /* DeInit the UART*/
      if (HAL_UART_DeInit(&hlpuart1) == HAL_OK)
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }
  return ret;
}

/**
  * @brief  Write Data through UART BUS.
  * @param  pData: Pointer to data buffer to send
  * @param  Length: Length of data in byte
  * @retval BSP status
  */
int32_t BSP_LPUART1_Send(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_UART_Transmit(&hlpuart1, pData, Length, BUS_LPUART1_POLL_TIMEOUT) == HAL_OK)
  {
      ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
  * @brief  Receive Data from UART BUS
  * @param  pData: Pointer to data buffer to receive
  * @param  Length: Length of data in byte
  * @retval BSP status
  */
int32_t  BSP_LPUART1_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_UART_Receive(&hlpuart1, pData, Length, BUS_LPUART1_POLL_TIMEOUT) == HAL_OK)
  {
      ret = BSP_ERROR_NONE;
  }
  return ret;
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
/**
  * @brief Register Default BSP LPUART1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_LPUART1_RegisterDefaultMspCallbacks (void)
{

  __HAL_UART_RESET_HANDLE_STATE(&hlpuart1);

  /* Register MspInit Callback */
  if (HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_MSPINIT_CB_ID, LPUART1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if (HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_MSPDEINIT_CB_ID, LPUART1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsLPUART1MspCbValid = 1;

  return BSP_ERROR_NONE;
}

/**
  * @brief BSP LPUART1 Bus Msp Callback registering
  * @param Callbacks     pointer to LPUART1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_LPUART1_RegisterMspCallbacks (BSP_UART_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_UART_RESET_HANDLE_STATE(&hlpuart1);

   /* Register MspInit Callback */
  if (HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_MSPINIT_CB_ID, Callbacks->pMspInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if (HAL_UART_RegisterCallback(&hlpuart1, HAL_UART_MSPDEINIT_CB_ID, Callbacks->pMspDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsLPUART1MspCbValid = 1;

  return BSP_ERROR_NONE;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/**
  * @brief  Return system tick in ms
  * @retval Current HAL time base time stamp
  */
int32_t BSP_GetTick(void) {
  return HAL_GetTick();
}

/* LPUART1 init function */

__weak HAL_StatusTypeDef MX_LPUART1_UART_Init(UART_HandleTypeDef* hlpuart)
{
  HAL_StatusTypeDef ret = HAL_OK;

  hlpuart->Instance = LPUART1;
  hlpuart->Init.BaudRate = 9600;
  hlpuart->Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart->Init.StopBits = UART_STOPBITS_1;
  hlpuart->Init.Parity = UART_PARITY_NONE;
  hlpuart->Init.Mode = UART_MODE_TX_RX;
  hlpuart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart->FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(hlpuart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_SetTxFifoThreshold(hlpuart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_SetRxFifoThreshold(hlpuart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_DisableFifoMode(hlpuart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void LPUART1_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK3;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Enable Peripheral clock */
    __HAL_RCC_LPUART1_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**LPUART1 GPIO Configuration
    PG7     ------> LPUART1_TX
    PG8     ------> LPUART1_RX
    */
    GPIO_InitStruct.Pin = BUS_LPUART1_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = BUS_LPUART1_TX_GPIO_AF;
    HAL_GPIO_Init(BUS_LPUART1_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BUS_LPUART1_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = BUS_LPUART1_RX_GPIO_AF;
    HAL_GPIO_Init(BUS_LPUART1_RX_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
}

static void LPUART1_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();

    /**LPUART1 GPIO Configuration
    PG7     ------> LPUART1_TX
    PG8     ------> LPUART1_RX
    */
    HAL_GPIO_DeInit(BUS_LPUART1_TX_GPIO_PORT, BUS_LPUART1_TX_GPIO_PIN);

    HAL_GPIO_DeInit(BUS_LPUART1_RX_GPIO_PORT, BUS_LPUART1_RX_GPIO_PIN);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(LPUART1_IRQn);

  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

