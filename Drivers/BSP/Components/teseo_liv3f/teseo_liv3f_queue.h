/**
  ******************************************************************************
  * @file    teseo_liv3f_queue.h
  * @author  SRA
  * @brief   Header file for teseo_liv3f_queue.c
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
#ifndef TESEO_LIV3F_QUEUE_H
#define TESEO_LIV3F_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "teseo_liv3f_conf.h"

#if (USE_FREE_RTOS_NATIVE_API)
  #include <FreeRTOS.h>
  #include <event_groups.h>
  #include "queue.h"
  #include <semphr.h>
#endif /* USE_FREE_RTOS_NATIVE_API */
  
#if (USE_AZRTOS_NATIVE_API)
  #include "tx_api.h"
#endif /* USE_AZRTOS_NATIVE_API */

#if (osCMSIS)
  #include "cmsis_os.h"
#endif /* osCMSIS */

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup COMPONENT COMPONENT
 * @{
 */

/** @addtogroup TESEO_LIV3F
 * @{
 */

/** @addtogroup TESEO_LIV3F_Exported_Constants
 * @{
 */

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Constant for the maximum message queue size
 */
#define MAX_MSG_QUEUE    (8)

/**
 * @brief Constant for the maximum number of messages in the queue
 */
#define MAX_MSG_BUF     (80)

/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Types
 * @{
 */
typedef int32_t (*TESEO_LIV3F_Transmit_ptr)(void *, uint8_t *, uint16_t);
typedef int32_t (*TESEO_LIV3F_Receive_ptr)(void *, uint8_t *, uint16_t);
typedef uint32_t (*TESEO_LIV3F_GetTick_ptr)(void *);
typedef void (*TESEO_LIV3F_ClearOREF_ptr)(void *);

typedef struct
{
  uint8_t *buf;
  uint16_t len;
} TESEO_LIV3F_Msg_t;

/**
 * @brief Data structure for the Teseo Message Queue.
 */
typedef struct
{
#if (USE_FREE_RTOS_NATIVE_API)
  SemaphoreHandle_t semaphore;
  uint32_t bitmap_unreleased_buffer_irq;
#endif /* USE_FREE_RTOS_NATIVE_API */
  
#if (USE_AZRTOS_NATIVE_API)
  TX_SEMAPHORE semaphore;
  uint32_t bitmap_unreleased_buffer_irq;
#endif /* USE_AZRTOS_NATIVE_API */

#if (osCMSIS)  
#if (osCMSIS < 0x20000U)
  osSemaphoreId semaphore;
  uint32_t bitmap_unreleased_buffer_irq;
#elif (osCMSIS >= 0x20000U)   
  osSemaphoreId_t semaphore;
  uint32_t bitmap_unreleased_buffer_irq;  
#endif /* (osCMSIS < 0x20000U) */
#endif /* osCMSIS */

  uint32_t bitmap_buffer_writable;
  uint32_t bitmap_buffer_readable;
  TESEO_LIV3F_Msg_t nmea_queue[MAX_MSG_QUEUE];
  uint8_t single_message_buffer[MAX_MSG_QUEUE * MAX_MSG_BUF];
} TESEO_LIV3F_Queue_t;

typedef struct
{
  /** Component mandatory fields **/
  TESEO_LIV3F_Transmit_ptr  Transmit;
  TESEO_LIV3F_Receive_ptr   Receive;
  TESEO_LIV3F_GetTick_ptr   GetTick;
  TESEO_LIV3F_ClearOREF_ptr ClearOREF;
  TESEO_LIV3F_Queue_t       *pQueue;
  /** Customizable optional pointer **/
  void *Handle;
} TESEO_LIV3F_ctx_t;

/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Functions
 * @{
 */

/**
 * @brief  Low level driver function that initializes the TeseoIII message queue.
 * @param  None
 * @retval The pointer to the created queue
 */
TESEO_LIV3F_Queue_t *teseo_queue_init(void);

/**
 * @brief  Low level driver function that deinitializes the TeseoIII message queue.
 * @param  The pointer to the queue
 * @retval None
 */
void teseo_queue_deinit(TESEO_LIV3F_Queue_t * queue);

/**
 * @brief  Low level driver function to get a new buffer (queue message) to be written.
 * @param  pTeseoQueue The message queue
 * @retval The message retrieved
 */
TESEO_LIV3F_Msg_t *teseo_queue_claim_wr_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue);

/**
 * @brief  Low level driver function to release a new buffer (queue message) to be written.
 * @param  pTeseoQueue The message queue
 * @param  pMsg        The message to be released for a new write
 * @retval None
 */
void teseo_queue_release_wr_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue, TESEO_LIV3F_Msg_t *pMsg);

/**
 * @brief  Low level driver function to get a new buffer (queue message) to be read.
 * @param  pTeseoQueue The message queue
 * @retval The message retrieved
 */
const TESEO_LIV3F_Msg_t *teseo_queue_claim_rd_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue);

/**
 * @brief  Low level driver function to release a new buffer (queue message) to be read.
 * @param  pTeseoQueue The message queue
 * @param  pMsg        The message to be released for a new read
 * @retval None
 */
void teseo_queue_release_rd_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue, const TESEO_LIV3F_Msg_t *pMsg);

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

#endif /* TESEO_LIV3F_QUEUE_H */





