/**
  ******************************************************************************
  * @file    paho_timer.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PAHO_TIMER_H__
#define __PAHO_TIMER_H__

/* Required by MQTTClient.h */

struct Timer {
	uint32_t init_tick;
  uint32_t timeout_ms;
};

typedef struct Timer Timer;

void TimerCountdownMS(Timer* timer, unsigned int timeout_ms);
void TimerCountdown(Timer* timer, unsigned int timeout);
int TimerLeftMS(Timer* timer);
char TimerIsExpired(Timer* timer);
void TimerInit(Timer* timer);

#endif  /* __TIMER_H__ */

