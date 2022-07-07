/**
  ******************************************************************************
  * @file    teseo_liv3f_uart.c
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
/* Includes ------------------------------------------------------------------*/
#include "teseo_liv3f_uart.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/*
 * Enumeration structure that contains the UART FSM states
 */
typedef enum
{
  fsm_discard,
  fsm_synch,
  fsm_stop
} Teseo_UART_FsmTypeDef;

/*
 * UART private data handler
 */
typedef struct
{
  volatile Teseo_UART_FsmTypeDef fsm_state;
  volatile Teseo_UART_FsmTypeDef fsm_next_state;
  uint8_t dummy_char;
  TESEO_LIV3F_Msg_t *wr_msg;
  volatile int8_t fsm_restart;

  TESEO_LIV3F_ctx_t *pCtx;
} Teseo_UART_DataTypeDef;

/*
 * Instance of UART private data handler
 */
static Teseo_UART_DataTypeDef Teseo_UART_Data;

/* Private functions ---------------------------------------------------------*/

/*
 * Private callback handler for UART rx complete
 */
static void teseo_rx_callback_ok(void)
{
  TESEO_LIV3F_ctx_t *pCtx = Teseo_UART_Data.pCtx;
  
  Teseo_UART_Data.fsm_restart = 1;

  while(Teseo_UART_Data.fsm_restart == 1)
  {
  
    Teseo_UART_Data.fsm_state = Teseo_UART_Data.fsm_next_state;
    
    switch (Teseo_UART_Data.fsm_state)
    {
      
    case fsm_discard:
      Teseo_UART_Data.fsm_restart = 0;
      Teseo_UART_Data.fsm_next_state = fsm_discard;
      
      if (Teseo_UART_Data.dummy_char != (uint8_t)'$')
      {
        //PRINT_DBG(".");
        pCtx->Receive(pCtx->Handle, &Teseo_UART_Data.dummy_char, 1);
        break;
      }
      
      if (Teseo_UART_Data.wr_msg == NULL)
      {
        Teseo_UART_Data.wr_msg = teseo_queue_claim_wr_buffer(pCtx->pQueue);
      }
      
      if (Teseo_UART_Data.wr_msg == NULL)
      {
        //PRINT_DBG(".");
        pCtx->Receive(pCtx->Handle, &Teseo_UART_Data.dummy_char, 1);
        break;
      }
      //PRINT_DBG("S");

      /* save '$' */
      Teseo_UART_Data.wr_msg->buf[Teseo_UART_Data.wr_msg->len] = Teseo_UART_Data.dummy_char;
      Teseo_UART_Data.wr_msg->len++;
      Teseo_UART_Data.fsm_next_state = fsm_synch ;
      pCtx->Receive(pCtx->Handle, (uint8_t *)&Teseo_UART_Data.wr_msg->buf[Teseo_UART_Data.wr_msg->len], 1);
       break;
       
    case fsm_synch:
      Teseo_UART_Data.fsm_restart = 0;
      
      if (Teseo_UART_Data.wr_msg->buf[Teseo_UART_Data.wr_msg->len] == (uint8_t)'$')
      {
        Teseo_UART_Data.dummy_char = (uint8_t)'$';
        Teseo_UART_Data.wr_msg->buf[Teseo_UART_Data.wr_msg->len] = (uint8_t)'\0';
        teseo_queue_release_wr_buffer(pCtx->pQueue, Teseo_UART_Data.wr_msg);
        Teseo_UART_Data.wr_msg = NULL;
        Teseo_UART_Data.fsm_next_state = fsm_discard ;
        /* check if we can resynch the new sentence */
        Teseo_UART_Data.fsm_restart = 1;
        break;
      }
      
      //PRINT_DBG("+");
      Teseo_UART_Data.wr_msg->len++;
#if 0
      if (Teseo_UART_Data.wr_msg->len == (uint32_t)MAX_MSG_BUF)
      {
        PRINT_DBG("MESSAGE TOO LONG\n\r");
        /* it seems we lost some char and the sentence is too much long...
        * reset the msg->len and discand all teh buffer and wait a new '$' sentence
        */
        Teseo_UART_Data.fsm_next_state = fsm_discard;
        Teseo_UART_Data.wr_msg->len = 0;
        Teseo_UART_Data.fsm_restart = 1;
        break;
      }
#endif
      Teseo_UART_Data.fsm_next_state = fsm_synch;
      pCtx->Receive(pCtx->Handle, (uint8_t *)&Teseo_UART_Data.wr_msg->buf[Teseo_UART_Data.wr_msg->len], 1);
      break;
      
    case fsm_stop:
    default:
      Teseo_UART_Data.fsm_restart = 0;
      break;
    }
  }
}

/*
 * Private callback handler for UART rx error
 */
static void teseo_rx_callback_error(void)
{
  const TESEO_LIV3F_ctx_t *pCtx = Teseo_UART_Data.pCtx;

  PRINT_DBG("E");
  
  Teseo_UART_Data.fsm_next_state = fsm_discard;
  Teseo_UART_Data.wr_msg->len = 0;
  pCtx->ClearOREF(pCtx->Handle);
  //__HAL_UART_CLEAR_FLAG(teseo_io_data->huart, UART_FLAG_ORE);
  pCtx->Receive(pCtx->Handle, &Teseo_UART_Data.dummy_char, 1);
}

/* Exported functions --------------------------------------------------------*/
void teseo_uart_rx_callback(Teseo_UART_CB_CallerTypedef c)
{
  switch (c)
  {
  case _ok:
    teseo_rx_callback_ok();
    break;
  case _error:
    teseo_rx_callback_error();
    break;
  default:
    break;
  }
}

void teseo_uart_rx_onoff(TESEO_LIV3F_ctx_t *pCtx, uint8_t enable)
{
  Teseo_UART_Data.pCtx = pCtx;

  if ((Teseo_UART_Data.fsm_state == fsm_discard) && (enable == 0U))
  {
    return;
  }

  if (enable == 0U)
  {
    Teseo_UART_Data.fsm_next_state = fsm_stop;
    
    while (Teseo_UART_Data.fsm_state != fsm_stop) {};
    return;
  }

  Teseo_UART_Data.wr_msg = NULL;
  Teseo_UART_Data.fsm_state = fsm_discard;
  Teseo_UART_Data.fsm_next_state = fsm_discard;
  
  pCtx->Receive(pCtx->Handle, &Teseo_UART_Data.dummy_char, 1);
}

