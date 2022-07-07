/**
  ******************************************************************************
  * @file    timingSystem.h
  * @author  Central LAB
  * @brief   Header file for timingSystem.c
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
#ifndef __TIMING_AGENT_H
#define __TIMING_AGENT_H

#include <time.h>

extern void TimingSystemInitialize(void);
extern int TimingSystemSetSystemTime(time_t epochTimeNow);
extern time_t TimingSystemGetSystemTime(void);

#define YYEAR0           1900            /* the first year */
#define EEPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SSECS_DAY        (24L * 60L * 60L)
#define LLEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YYEARSIZE(year)  (LLEAPYEAR(year) ? 366 : 365)

// MDK
extern struct tm *gmtimeMDK(register const time_t *timer);

#endif /* __TIMING_AGENT_H */

