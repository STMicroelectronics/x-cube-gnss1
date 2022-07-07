/**
  ******************************************************************************
  * @file    msg.h
  * @author  MCD Application Team
  * @brief   Trace message API
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
#ifndef __MSG_H__
#define __MSG_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef   ENABLE_IOT_INFO
#define MSG_INFO
#endif

#ifdef ENABLE_IOT_WARN
#define MSG_WARNING
#endif
#ifdef ENABLE_IOT_ERROR
#define MSG_ERROR
#endif

#ifdef ENABLE_IOT_DEBUG
#define MSG_DEBUG
#endif


/**
 * @brief Debug level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 */
#ifdef MSG_DEBUG
#define msg_debug(...)    \
	{\
	printf("DEBUG:   %s L#%d ", __func__, __LINE__);  \
	printf(__VA_ARGS__); \
	}
#else
#define msg_debug(...)
#endif


/**
 * @brief Info level logging macro.
 *
 * Macro to expose desired log message.  Info messages do not include automatic function names and line numbers.
 */
#ifdef MSG_INFO
#define msg_info(...)    \
	{\
	printf(__VA_ARGS__); \
	}
#else
#define msg_info(...)
#endif

/**
 * @brief Warn level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 */
#ifdef MSG_WARNING
#define msg_warning(...)   \
	{ \
	printf("WARN:  %s L#%d ", __func__, __LINE__);  \
	printf(__VA_ARGS__); \
	}
#else
#define msg_warning(...)
#endif

/**
 * @brief Error level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 */
#ifdef MSG_ERROR
#define msg_error(...)  \
	{ \
	printf("ERROR: %s L#%d ", __func__, __LINE__); \
	printf(__VA_ARGS__); \
	}
#else
#define msg_error(...)
#endif

#endif /* __MSG_H__ */

