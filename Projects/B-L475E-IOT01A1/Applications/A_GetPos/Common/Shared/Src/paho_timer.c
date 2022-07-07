/**
  ******************************************************************************
  * @file    paho_timer.c
  * @author  MCD Application Team
  * @brief   Timer adaptation layer for the Paho MQTT client.
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
/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */
#include "main.h"
#include "paho_timer.h"  


void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
  timer->init_tick = HAL_GetTick();
  timer->timeout_ms = timeout_ms;
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{
  TimerCountdownMS(timer, timeout * 1000);
}


int TimerLeftMS(Timer* timer)
{
  int ret = 0;
  uint32_t cur_tick = HAL_GetTick();  // The HAL tick period is 1 millisecond.
  if (cur_tick < timer->init_tick)
  { // Timer wrap-around detected
    // printf("Timer: wrap-around detected from %d to %d\n", timer->init_tick, cur_tick);
    timer->timeout_ms -= 0xFFFFFFFF - timer->init_tick;
    timer->init_tick = 0;
  }
  ret = timer->timeout_ms - (cur_tick - timer->init_tick);

  return (ret >= 0) ? ret : 0;
}


char TimerIsExpired(Timer* timer)
{
  return (TimerLeftMS(timer) > 0) ? 0 : 1;
}


void TimerInit(Timer* timer)
{
  timer->init_tick = 0;
  timer->timeout_ms = 0;
}

