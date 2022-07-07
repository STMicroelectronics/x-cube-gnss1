/**
  ******************************************************************************
  * @file    timer.h
  * @author  MCD Application Team
  * @brief   Timer basic functions.
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
#ifndef __TIMER_H__
#define __TIMER_H__

struct Timer {
	unsigned long end_time;
    int           init_tick;
};

typedef struct Timer Timer;


bool has_timer_expired(Timer *timer);
void countdown_ms(Timer *timer, uint32_t timeout);
uint32_t left_ms(Timer *timer);
void countdown_sec(Timer *timer, uint32_t timeout);
void init_timer(Timer *timer);
int getTimestamp(char *buff, int buffSize);
#endif  /* __TIMER_H__ */

