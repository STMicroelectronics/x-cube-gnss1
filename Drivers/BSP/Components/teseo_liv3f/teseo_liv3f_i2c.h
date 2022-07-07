/**
  ******************************************************************************
  * @file    teseo_liv3f_i2c.h
  * @author  APG/SRA
  * @brief   Header file for teseo_liv3f_i2c.c
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
#ifndef TESEO_LIV3F_I2C_H
#define TESEO_LIV3F_I2C_H

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
 * @brief Enumeration structure that contains the status of the I2C recv callback
 */
typedef enum
{
  _i2c_ok,
  _i2c_error,
  _i2c_abort
} Teseo_I2C_CB_CallerTypedef;
/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Functions
 * @{
 */

/**
 * @brief  Low level driver function to enable/disable the communication with Teseo III via I2C.
 * @param  pCtx Pointer to the Teseo relevant context
 * @param  enable Flag to enable/disable the communication
 * @param  nr_msg Number of complete messages we want to receive from the GNSS module
 * @retval None
 */
void teseo_i2c_onoff(TESEO_LIV3F_ctx_t *pCtx, uint8_t enable, int8_t nr_msg);

/**
 * @brief  Low level driver function to handle the I2C recv callabck and update consistently the FSM.
 * @param  c   The type of callback
 * @retval None
 */
void teseo_i2c_rx_callback(Teseo_I2C_CB_CallerTypedef c);

/**
 * @brief  Low level driver function to start (or resume after a given timeout) communication via I2C.
 * @param  None
 * @retval None
 * NOTE: this function has to be added in 'HAL_IncTick'
 */
void teseo_i2c_background_process(void);
/**
 * @}
 */

/* Private functions ---------------------------------------------------------*/
static inline void teseo_i2c_enable(TESEO_LIV3F_ctx_t *pCtx)
{
  teseo_i2c_onoff(pCtx, 1U, -1);
}

static inline void teseo_i2c_disable(TESEO_LIV3F_ctx_t *pCtx)
{
  teseo_i2c_onoff(pCtx, 0U, -1);
}

static inline void teseo_i2c_rx_callback_ok(void)
{
  teseo_i2c_rx_callback(_i2c_ok);
}

static inline void teseo_i2c_rx_callback_error(void)
{
  teseo_i2c_rx_callback(_i2c_error);
}

static inline void teseo_i2c_rx_callback_abort(void)
{
  teseo_i2c_rx_callback(_i2c_abort);
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

#ifdef __cplusplus
}
#endif

#endif /* TESEO_LIV3F_I2C_H */

