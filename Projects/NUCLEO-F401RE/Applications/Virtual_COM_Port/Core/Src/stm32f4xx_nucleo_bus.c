/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32f4xx_nucleo_bus.c
  * @brief          : source file for the BSP BUS IO driver
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_nucleo_bus.h"

__weak HAL_StatusTypeDef MX_USART1_UART_Init(UART_HandleTypeDef* huart);

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32F4XX_NUCLEO
  * @{
  */

/** @defgroup STM32F4XX_NUCLEO_BUS STM32F4XX_NUCLEO BUS
  * @{
  */

/** @defgroup STM32F4XX_NUCLEO_BUS_Exported_Variables BUS Exported Variables
  * @{
  */

UART_HandleTypeDef huart1;
/**
  * @}
  */

/** @defgroup STM32F4XX_NUCLEO_BUS_Private_Variables BUS Private Variables
  * @{
  */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
static uint32_t IsUSART1MspCbValid = 0;
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
static uint32_t USART1InitCounter = 0;

/**
  * @}
  */

/** @defgroup STM32F4XX_NUCLEO_BUS_Private_FunctionPrototypes  BUS Private Function
  * @{
  */

static void USART1_MspInit(UART_HandleTypeDef* huart);
static void USART1_MspDeInit(UART_HandleTypeDef* huart);

/**
  * @}
  */

/** @defgroup STM32F4XX_NUCLEO_LOW_LEVEL_Private_Functions STM32F4XX_NUCLEO LOW LEVEL Private Functions
  * @{
  */

/** @defgroup STM32F4XX_NUCLEO_BUS_Exported_Functions STM32F4XX_NUCLEO_BUS Exported Functions
  * @{
  */

/* BUS IO driver over USART Peripheral */
/*******************************************************************************
                            BUS OPERATIONS OVER USART
*******************************************************************************/
/**
  * @brief  Initializes USART HAL.
  * @param  Init : UART initialization parameters
  * @retval BSP status
  */
int32_t BSP_USART1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  huart1.Instance  = USART1;

  if(USART1InitCounter++ == 0)
  {
    if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_RESET)
    {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
      /* Init the UART Msp */
      USART1_MspInit(&huart1);
#else
      if(IsUSART1MspCbValid == 0U)
      {
        if(BSP_USART1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
#endif
      if(ret == BSP_ERROR_NONE)
      {
        /* Init the UART */
        if (MX_USART1_UART_Init(&huart1) != HAL_OK)
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
int32_t BSP_USART1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if (USART1InitCounter > 0)
  {
    if (--USART1InitCounter == 0)
    {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
      USART1_MspDeInit(&huart1);
#endif
      /* DeInit the UART*/
      if (HAL_UART_DeInit(&huart1) == HAL_OK)
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
int32_t BSP_USART1_Send(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_UART_Transmit(&huart1, pData, Length, BUS_USART1_POLL_TIMEOUT) == HAL_OK)
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
int32_t  BSP_USART1_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_UART_Receive(&huart1, pData, Length, BUS_USART1_POLL_TIMEOUT) == HAL_OK)
  {
      ret = BSP_ERROR_NONE;
  }
  return ret;
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
/**
  * @brief Register Default BSP USART1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_USART1_RegisterDefaultMspCallbacks (void)
{

  __HAL_UART_RESET_HANDLE_STATE(&huart1);

  /* Register MspInit Callback */
  if (HAL_UART_RegisterCallback(&huart1, HAL_UART_MSPINIT_CB_ID, USART1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if (HAL_UART_RegisterCallback(&huart1, HAL_UART_MSPDEINIT_CB_ID, USART1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsUSART1MspCbValid = 1;

  return BSP_ERROR_NONE;
}

/**
  * @brief BSP USART1 Bus Msp Callback registering
  * @param Callbacks     pointer to USART1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_USART1_RegisterMspCallbacks (BSP_UART_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_UART_RESET_HANDLE_STATE(&huart1);

   /* Register MspInit Callback */
  if (HAL_UART_RegisterCallback(&huart1, HAL_UART_MSPINIT_CB_ID, Callbacks->pMspInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Register MspDeInit Callback */
  if (HAL_UART_RegisterCallback(&huart1, HAL_UART_MSPDEINIT_CB_ID, Callbacks->pMspDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  IsUSART1MspCbValid = 1;

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

/* USART1 init function */

__weak HAL_StatusTypeDef MX_USART1_UART_Init(UART_HandleTypeDef* huart)
{
  HAL_StatusTypeDef ret = HAL_OK;

  huart->Instance = USART1;
  huart->Init.BaudRate = 9600;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void USART1_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = BUS_USART1_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = BUS_USART1_TX_GPIO_AF;
    HAL_GPIO_Init(BUS_USART1_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BUS_USART1_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = BUS_USART1_RX_GPIO_AF;
    HAL_GPIO_Init(BUS_USART1_RX_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
}

static void USART1_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(BUS_USART1_TX_GPIO_PORT, BUS_USART1_TX_GPIO_PIN);

    HAL_GPIO_DeInit(BUS_USART1_RX_GPIO_PORT, BUS_USART1_RX_GPIO_PIN);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
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

