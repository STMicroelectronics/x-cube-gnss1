/**
  ******************************************************************************
  * @file    teseo_liv3f_queue.c
  * @author  SRA
  * @brief   Teseo-LIV3F buffer queue manager
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
/* Includes ------------------------------------------------------------------*/
#include "teseo_liv3f_queue.h"

/* Defines -------------------------------------------------------------------*/
#define ANY_RTOS ((USE_FREE_RTOS_NATIVE_API) || (USE_AZRTOS_NATIVE_API) || (osCMSIS))

/* Private variables ---------------------------------------------------------*/
#if (USE_FREE_RTOS_NATIVE_API)
  #ifndef configLIBRARY_LOWEST_INTERRUPT_PRIORITY
    #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
  #endif
  static BaseType_t b = configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
#endif /* USE_FREE_RTOS_NATIVE_API */

/* Private functions ---------------------------------------------------------*/
/*
 * ffs - find first bit set
 * => renamed here teseo_ffs() to not conflict with the ffs() defined in strings.h for the GCC compiler
 * @x: the word to search
 *
 * This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 * Source: 
 * git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git/tree/include/asm-generic/bitops/ffs.h?h=v4.1.46
 */
static inline int32_t teseo_ffs(uint32_t _w)
{
  int32_t r = 1;
  uint32_t w = _w;
  
  if (w == 0U)
  {
    return 0;
  }
  if ((w & 0xffffU) == 0U)
  {
    w >>= 16;
    r += 16;
  }
  if ((w & 0xffU) == 0U)
  {
    w >>= 8;
    r += 8;
  }
  if ((w & 0xfU) == 0U)
  {
    w >>= 4;
    r += 4;
  }
  if ((w & 3U) == 0U)
  {
    w >>= 2;
    r += 2;
  }
  if ((w & 1U) == 0U)
  {
    r += 1;
  }
  return r;
}

#if (USE_FREE_RTOS_NATIVE_API)

#define BaseVal_t BaseType_t
#define OS_SUCCESS !(pdFAIL)

/*
 * Wrapper for task delay
 */
static inline void os_delay()
{
  vTaskDelay(portTICK_PERIOD_MS * 5U);
}

/*
 * Wrapper for Semaphore create.
 */
static inline SemaphoreHandle_t semaphore_create(void)
{
  SemaphoreHandle_t semaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(semaphore);
  return semaphore;
}

/*
 * Wrapper for Semaphore delete.
 */
static inline UINT semaphore_delete(SemaphoreHandle_t sem)
{
  /** check that no task is holding the semaphore **/
  if (xSemaphoreGetMutexHolder(sem) == NULL) 
  {
    vSemaphoreDelete(semaphore);
    return 0;
  }
  /** check if the task holding the semaphore is the current task **/
  if (xTaskGetCurrentTaskHandle() == xSemaphoreGetMutexHolder(sem))
  {
    vSemaphoreDelete(sem);
    return 0;
  } else {
  /** semaphore is held by a task different than the current task: delete is not possible **/  
  }
}

/*
 * Wrapper for Semaphore lock.
 */
static inline BaseType_t semaphore_lock(SemaphoreHandle_t sem)
{
  return xSemaphoreTake(sem, 5000);
}

/*
 * Wrapper for Semaphore release.
 */
static inline void semaphore_free(SemaphoreHandle_t sem)
{
  (void)xSemaphoreGive(sem);
}

/*
 * Wrapper for Semaphore lock from interrupt context.
 */
static inline BaseType_t semaphore_lock_irq(SemaphoreHandle_t sem)
{
  return xSemaphoreTakeFromISR(sem,  &b);
}

/*
 * Wrapper for Semaphore release from interrupt context.
 */
static inline void semaphore_free_irq(SemaphoreHandle_t sem)
{
  (void)xSemaphoreGiveFromISR(sem, &b);
}
#endif /* USE_FREE_RTOS_NATIVE_API */

#if (USE_AZRTOS_NATIVE_API)
	
#define BaseVal_t UINT
#define OS_SUCCESS TX_SUCCESS

/*
 * Wrapper for thread sleep
 */
static inline void os_delay()
{
  tx_thread_sleep(((TX_TIMER_TICKS_PER_SECOND / 100)));  // wait 10mS
}

/*
 * Wrapper for Semaphore create.
 */
static inline TX_SEMAPHORE semaphore_create(void)
{
  TX_SEMAPHORE semaphore;  
  if (tx_semaphore_create(&semaphore, "TeseoQueueSemaphore", 0) != TX_SUCCESS) {
    return semaphore;
  }
  
  if (tx_semaphore_put(&semaphore) != TX_SUCCESS) {
    tx_semaphore_delete(&semaphore);
  }
  return semaphore;
}

/*
 * Wrapper for Semaphore delete.
 */
static inline UINT semaphore_delete(TX_SEMAPHORE semaphore)
{ /* deregister call back waiting for notification on semaphore if any */
  if (tx_semaphore_put_notify(&semaphore, TX_NULL)!= TX_SUCCESS) {
    return 0;
  }
  return (tx_semaphore_delete(&semaphore));
}

/*
 * Wrapper for Semaphore lock.
 */
static inline UINT semaphore_lock(TX_SEMAPHORE sem)
{
  return tx_semaphore_get(&sem, TX_WAIT_FOREVER);
}

/*
 * Wrapper for Semaphore release.
 */
static inline void semaphore_free(TX_SEMAPHORE sem)
{
  tx_semaphore_put(&sem);
}

/*
 * Wrapper for Semaphore lock from interrupt context.
 */
static inline UINT semaphore_lock_irq(TX_SEMAPHORE sem)
{
  return tx_semaphore_get(&sem, TX_NO_WAIT);
}

/*
 * Wrapper for Semaphore release from interrupt context.
 */
static inline void semaphore_free_irq(TX_SEMAPHORE sem)
{
  tx_semaphore_put(&sem);  
}
#endif /* USE_AZRTOS_NATIVE_API */

#if (osCMSIS)
#if (osCMSIS < 0x20000U)
	
#define BaseVal_t uint32_t
#define OS_SUCCESS osOK

/*
 * Wrapper for os delay
 */
static inline void os_delay()
{
  osDelay(500U);
}

/*
 * Wrapper for Semaphore create.
 */
static inline osSemaphoreId semaphore_create(void)
{
  osSemaphoreDef(TeseoQueue_SEM);
  return (osSemaphoreCreate(osSemaphore(TeseoQueue_SEM), 1));  
}

/*
 * Wrapper for Semaphore delete.
 */
static inline osStatus semaphore_delete(osSemaphoreId semaphore)
{ 
  return (osSemaphoreDelete(semaphore));
}

/*
 * Wrapper for Semaphore lock.
 */
static inline uint32_t semaphore_lock(osSemaphoreId sem)
{
  return osSemaphoreWait(sem, osWaitForever);
}

/*
 * Wrapper for Semaphore release.
 */
static inline void semaphore_free(osSemaphoreId sem)
{
  osSemaphoreRelease(sem);
}

/*
 * Wrapper for Semaphore lock from interrupt context.
 */
static inline uint32_t semaphore_lock_irq(osSemaphoreId sem)
{
  return osSemaphoreWait(sem, 0);
}

/*
 * Wrapper for Semaphore release from interrupt context.
 */
static inline void semaphore_free_irq(osSemaphoreId sem)
{
  osSemaphoreRelease(sem);
}
#elif (osCMSIS >= 0x20000U)
	
#define BaseVal_t osStatus_t
#define OS_SUCCESS osOK

/*
 * Wrapper for os delay
 */
static inline void os_delay()
{
  osDelay(500U);
}

/*
 * Wrapper for Semaphore create.
 */
static inline osSemaphoreId_t semaphore_create(void)
{
  return(osSemaphoreNew(1, 1, NULL));
}

/*
 * Wrapper for Semaphore delete.
 */
static inline osStatus_t semaphore_delete(osSemaphoreId semaphore)
{ 
  return (osSemaphoreDelete(semaphore));
}

/*
 * Wrapper for Semaphore lock.
 */
static inline osStatus_t semaphore_lock(osSemaphoreId_t sem)
{
  return osSemaphoreAcquire(sem, osWaitForever);
}

/*
 * Wrapper for Semaphore release.
 */
static inline void semaphore_free(osSemaphoreId_t sem)
{
  osSemaphoreRelease(sem);
}

/*
 * Wrapper for Semaphore lock from interrupt context.
 */
static inline osStatus_t semaphore_lock_irq(osSemaphoreId_t sem)
{
  return osSemaphoreAcquire(sem, 0);
}

/*
 * Wrapper for Semaphore release from interrupt context.
 */
static inline void semaphore_free_irq(osSemaphoreId_t sem)
{
  osSemaphoreRelease(sem);
}
#endif /* (osCMSIS < 0x20000U) */
#endif /* osCMSIS */ 

/*
 * Returns the static instance of a Queue object.
 */
static TESEO_LIV3F_Queue_t *Teseo_Queue_static_alloc(void)
{
  static TESEO_LIV3F_Queue_t Teseo_Queue_Instance;
  return &Teseo_Queue_Instance;
}

/*
 * Returns the index of a message in the queue.
 */
static int32_t __find_msg_idx(const TESEO_LIV3F_Queue_t *pTeseoQueue, const TESEO_LIV3F_Msg_t *pTeseoMsg)
{
  int32_t i;
  
  for (i = 0; i < MAX_MSG_QUEUE; ++i)
  {
    if (&pTeseoQueue->nmea_queue[i] == pTeseoMsg)
    {
      return i;
    }
  }
  
  return -1;
}

/*
 * Checks for buffers not yet released.
 */
static void __check_unreleased_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue)
{
#if (ANY_RTOS)
  if (pTeseoQueue->bitmap_unreleased_buffer_irq == 0UL)
  {
    return;
  }
  
  PRINT_DBG("R\n\r");
  pTeseoQueue->bitmap_buffer_readable |= pTeseoQueue->bitmap_unreleased_buffer_irq;
  pTeseoQueue->bitmap_unreleased_buffer_irq = 0;
#endif /* ANY_RTOS */
}

/*
 * Checks for messages longer than MAX_MSG_BUF.
 */
static void check_longer_msg(TESEO_LIV3F_Queue_t *pTeseoQueue, const TESEO_LIV3F_Msg_t *pTeseoMsg, uint32_t idx, int8_t wr_or_rd)
{
  if (pTeseoMsg->len <= (uint32_t)MAX_MSG_BUF)
  {
    return;
  }
  
  uint32_t j;
  uint32_t extra_bufs = (pTeseoMsg->len / (uint32_t)MAX_MSG_BUF) + (((pTeseoMsg->len % (uint32_t)MAX_MSG_BUF) != 0U) ? 1U : 0U);
  
  for (j = 0; j < extra_bufs; j++)
  {
    uint32_t rhs = (idx + j);

    if (wr_or_rd == 1)
    {
      if(rhs < 32U)
      {
        pTeseoQueue->bitmap_buffer_writable &= ~(1UL << rhs);
      }
    }
    else
    {
      if(rhs < 32U)
      {
        pTeseoQueue->bitmap_buffer_writable |= (1UL << rhs);
      }
    }
  }
}

/* Exported functions --------------------------------------------------------*/
TESEO_LIV3F_Queue_t *teseo_queue_init(void)
{
  TESEO_LIV3F_Queue_t *pTeseoQueue = Teseo_Queue_static_alloc();
  int8_t i;
  TESEO_LIV3F_Msg_t *pTeseoMsg;
  
  PRINT_DBG("teseo_queue_init...\n\r" );
  pTeseoQueue->bitmap_buffer_writable = 0;
  for (i = 0;  i < MAX_MSG_QUEUE; ++i)
  {
    pTeseoMsg = &pTeseoQueue->nmea_queue[i];

    pTeseoMsg->buf = &pTeseoQueue->single_message_buffer[i * MAX_MSG_BUF];
    pTeseoMsg->len = 0;
    pTeseoQueue->bitmap_buffer_writable |=  (1UL << (uint8_t)i);
  }
  pTeseoQueue->bitmap_buffer_readable = 0;
#if (ANY_RTOS)
  pTeseoQueue->bitmap_unreleased_buffer_irq = 0;
  pTeseoQueue->semaphore = semaphore_create();  
#endif /* ANY_RTOS */
  PRINT_DBG("teseo_queue_init: Done\n\r");
  
  return pTeseoQueue;
}
  
void teseo_queue_deinit(TESEO_LIV3F_Queue_t * pTeseoQueue)
{ /** as msg pool is statically allocated no need to free it **/
#if (ANY_RTOS)
  /** delete the semaphore  **/ 
  semaphore_delete(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */
}

TESEO_LIV3F_Msg_t *teseo_queue_claim_wr_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue)
{
  int32_t i;
  TESEO_LIV3F_Msg_t *pTeseoMsg;

#if (ANY_RTOS)
  BaseVal_t ret;
  ret = semaphore_lock_irq(pTeseoQueue->semaphore);

  /* semaphore already taken.... */
  if (ret != OS_SUCCESS) {
    return NULL;
  }
#endif /* ANY_RTOS */

  /* first available buffer writable */
  i = teseo_ffs(pTeseoQueue->bitmap_buffer_writable);
  
  if (i == 0) {
#if (ANY_RTOS)
    /* release the semaphore */
    semaphore_free_irq(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */
    PRINT_DBG("-\n\r");
    return NULL;
  }
  
  i--; /* first available buffer writable */
  
  /* buffer no more writable */
  pTeseoQueue->bitmap_buffer_writable &= ~(1U << (uint32_t)i);
  
  __check_unreleased_buffer(pTeseoQueue);

#if (ANY_RTOS)
  semaphore_free_irq(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */

  pTeseoMsg = &pTeseoQueue->nmea_queue[i];
  pTeseoMsg->len = 0;
  
  return pTeseoMsg;
}

void teseo_queue_release_wr_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue, TESEO_LIV3F_Msg_t *pTeseoMsg)
{
  int32_t i;

#if (ANY_RTOS)
  BaseVal_t ret;
#endif /* ANY_RTOS */
  
  if (pTeseoMsg == NULL) {
    return;
  }
  
  i = __find_msg_idx(pTeseoQueue, pTeseoMsg);
  if (i < 0) {
    /* this should NEVER happen... this means an external buffer was provided */
    return;
  }
  
#if (ANY_RTOS)
  ret = semaphore_lock_irq(pTeseoQueue->semaphore);
  
  /* semaphore already taken.... */
  if (ret != OS_SUCCESS) {
    PRINT_DBG("+\n\r" );
    /* mark the buffer as 'un-released'... and go-ahead... */
    pTeseoQueue->bitmap_unreleased_buffer_irq |= (1UL << (uint32_t)i);
    return;
  }
#endif /* ANY_RTOS */
  
  /* mark the buffer as readable */
  pTeseoQueue->bitmap_buffer_readable |= (1UL << (uint32_t)i);

  /* check for longer message */
  check_longer_msg(pTeseoQueue, pTeseoMsg, (uint32_t)i, 1);

#if (ANY_RTOS)
  __check_unreleased_buffer(pTeseoQueue);
  semaphore_free_irq(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */
}

const TESEO_LIV3F_Msg_t *teseo_queue_claim_rd_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue)
{
  const TESEO_LIV3F_Msg_t *pTeseoMsg = NULL;
  int32_t i;
  uint8_t rd_buffer_re_try = 1;
  
#if (ANY_RTOS)
  BaseVal_t ret;
#endif /* ANY_RTOS */
  
  while(rd_buffer_re_try == 1U) {
#if (ANY_RTOS)
    ret = semaphore_lock(pTeseoQueue->semaphore);

    /* semaphore already taken.... */
    if (ret != OS_SUCCESS) {
      return NULL;
    }
#endif /* ANY_RTOS */
    
    /* first available readable buffer */
    i = teseo_ffs(pTeseoQueue->bitmap_buffer_readable);
    
    if (i == 0) {
#if (ANY_RTOS)
      /* release the semaphore */
      semaphore_free(pTeseoQueue->semaphore);
      //PRINT_DBG("No read buffer available... going to sleep...\n\r");
	  os_delay();
      continue;
#else   /* no RTOS: bare metal */
      break;
#endif /* ANY_RTOS */
    }

    /* first available readable buffer */
    i -= 1;
    /* buffer no more readable */
    pTeseoQueue->bitmap_buffer_readable &= ~(1UL << (uint32_t)i);
    pTeseoMsg = &pTeseoQueue->nmea_queue[i];

#if (ANY_RTOS)
    semaphore_free(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */

    rd_buffer_re_try = 0;
  } /* while */
  return pTeseoMsg;
}

void teseo_queue_release_rd_buffer(TESEO_LIV3F_Queue_t *pTeseoQueue, const TESEO_LIV3F_Msg_t *pTeseoMsg)
{ 
  int32_t i;
  
#if (ANY_RTOS)
  BaseVal_t ret;
#endif /* ANY_RTOS */
  
  i = __find_msg_idx(pTeseoQueue, pTeseoMsg);
  if (i < 0) {
    /* this should NEVER happen... this means an external buffer was provided */
    return;
  }

#if (ANY_RTOS)
  ret = semaphore_lock(pTeseoQueue->semaphore);

  /* semaphore already taken.... */
  if (ret != OS_SUCCESS) {
    return;
  }  
#endif /* ANY_RTOS */

  /* mark the buffer as writeable */
  pTeseoQueue->bitmap_buffer_writable |= (1UL << (uint32_t)i);

  /* check for longer message */
  check_longer_msg(pTeseoQueue, pTeseoMsg, (uint32_t)i, 0);

#if (ANY_RTOS)
  semaphore_free(pTeseoQueue->semaphore);
#endif /* ANY_RTOS */
}



