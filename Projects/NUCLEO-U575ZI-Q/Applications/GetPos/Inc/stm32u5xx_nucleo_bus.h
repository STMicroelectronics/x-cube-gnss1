/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32u5xx_nucleo_bus.h
  * @brief          : header file for the BSP BUS IO driver
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32U5XX_NUCLEO_BUS_H
#define STM32U5XX_NUCLEO_BUS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_nucleo_conf.h"
#include "stm32u5xx_nucleo_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32U5XX_NUCLEO
  * @{
  */

/** @defgroup STM32U5XX_NUCLEO_BUS STM32U5XX_NUCLEO BUS
  * @{
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Exported_Constants STM32U5XX_NUCLEO BUS Exported Constants
  * @{
  */

#define BUS_LPUART1_INSTANCE LPUART1
#define BUS_LPUART1_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_LPUART1_TX_GPIO_AF GPIO_AF8_LPUART1
#define BUS_LPUART1_TX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOG_CLK_DISABLE()
#define BUS_LPUART1_TX_GPIO_PIN GPIO_PIN_7
#define BUS_LPUART1_TX_GPIO_PORT GPIOG
#define BUS_LPUART1_RX_GPIO_AF GPIO_AF8_LPUART1
#define BUS_LPUART1_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_LPUART1_RX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOG_CLK_DISABLE()
#define BUS_LPUART1_RX_GPIO_PIN GPIO_PIN_8
#define BUS_LPUART1_RX_GPIO_PORT GPIOG

#ifndef BUS_LPUART1_BAUDRATE
   #define BUS_LPUART1_BAUDRATE  9600U /* baud rate of UARTn = 9600 baud*/
#endif
#ifndef BUS_LPUART1_POLL_TIMEOUT
   #define BUS_LPUART1_POLL_TIMEOUT                9600U
#endif

/**
  * @}
  */

/** @defgroup STM32U5XX_NUCLEO_BUS_Private_Types STM32U5XX_NUCLEO BUS Private types
  * @{
  */
#if (USE_HAL_UART_REGISTER_CALLBACKS  == 1U)
typedef struct
{
  pUART_CallbackTypeDef  pMspInitCb;
  pUART_CallbackTypeDef  pMspDeInitCb;
}BSP_UART_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1U) */

/**
  * @}
  */

/** @defgroup STM32U5XX_NUCLEO_LOW_LEVEL_Exported_Variables LOW LEVEL Exported Constants
  * @{
  */

extern UART_HandleTypeDef hlpuart1;

/**
  * @}
  */

/** @addtogroup STM32U5XX_NUCLEO_BUS_Exported_Functions
  * @{
  */

HAL_StatusTypeDef MX_LPUART1_UART_Init(UART_HandleTypeDef* huart);
int32_t BSP_LPUART1_Init(void);
int32_t BSP_LPUART1_DeInit(void);
int32_t BSP_LPUART1_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_LPUART1_Recv(uint8_t *pData, uint16_t Length);
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
int32_t BSP_LPUART1_RegisterDefaultMspCallbacks (void);
int32_t BSP_LPUART1_RegisterMspCallbacks (BSP_UART_Cb_t *Callbacks);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1U)  */

int32_t BSP_GetTick(void);

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
#ifdef __cplusplus
}
#endif

#endif /* STM32U5XX_NUCLEO_BUS_H */

