/**
  ******************************************************************************
  * @file    teseo_liv3f_uart.h
  * @author  APG/SRA
  * @brief   Teseo-LIV3F UART handler
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TESEO_LIV3F_UART_H
#define TESEO_LIV3F_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "teseo_liv3f_queue.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup COMPONENTS
 * @{
 */

/** @addtogroup TESEOIII
 * @{
 */
    
/** @addtogroup TESEO_LIV3F_Exported_Constants
 * @{
 */
/* Exported defines ----------------------------------------------------------*/

/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Types
 * @{
 */

/**
 * @brief Enumeration structure that contains the status of the UART recv callback
 */
typedef enum
{
  _ok,
  _error
} Teseo_UART_CB_CallerTypedef;
/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Functions
 * @{
 */

/**
 * @brief  Low level driver function to enable/disable the communication with Teseo III via UART.
 * @param  pCtx Pointer to the Teseo relevant context
 * @param  enable Flag to enable/disable the communication
 * @retval None
 */
void teseo_uart_rx_onoff(TESEO_LIV3F_ctx_t *pCtx, uint8_t enable);

/**
 * @brief  Low level driver function to handle the UART recv callabck and update consistently the FSM.
 * @param  c     The type of callback
 * @retval None
 */
void teseo_uart_rx_callback(Teseo_UART_CB_CallerTypedef c);
/**
 * @}
 */

/** @defgroup TESEO_LIV3F_EXPORTED_MACROS EXPORTED MACROS
 * @{
 */
/**
 * @brief Wrapper to enable the UART communication
 */
#define teseo_uart_rx_enable(pCtx)	teseo_uart_rx_onoff(pCtx, 1)

/**
 * @brief Wrapper to disable the UART communication
 */
#define teseo_uart_rx_disable(pCtx)	teseo_uart_rx_onoff(pCtx, 0)


/**
 * @brief Wrapper for the UART recv complete callback
 */
#define teseo_uart_rx_callback_ok(void) teseo_uart_rx_callback(_ok)

/**
 * @brief Wrapper for the UART recv error callback
 */
#define teseo_uart_rx_callback_error(void) teseo_uart_rx_callback(_error)
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

#endif /* TESEO_LIV3F_UART_H */


