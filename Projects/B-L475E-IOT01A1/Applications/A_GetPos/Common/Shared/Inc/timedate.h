/**
  ******************************************************************************
  * @file    timedate.h
  * @author  MCD Application Team
  * @brief   Configuration of the RTC.
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
#ifndef TIMEDATE_H
#define TIMEDATE_H

#include <stdbool.h>

#define TD_OK             0
#define TD_ERR_CONNECT   -1   /**< Could not connect to the network and join the web server. */
#define TD_ERR_HTTP      -2   /**< Could not parse the time and date from the web server response. */
#define TD_ERR_RTC       -3   /**< Could not set the RTC. */
#define TD_ERR_TLS_CERT  -4   /**< The server certificate verification failed. Applicable only when force_apply is false. */

int setRTCTimeDateFromNetwork(bool force_apply);


#endif /* TIMEDATE_H */


