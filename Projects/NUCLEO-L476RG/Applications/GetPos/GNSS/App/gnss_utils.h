/**
  ******************************************************************************
  * @file    App/gnss_utils.h
  * @author  SRA Application Team
  * @brief   This file contains application specific utilities
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _GNSS_UTILS_H_
#define _GNSS_UTILS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup GetPos
 * @{
 */

/** @addtogroup GetPos_PUBLIC_FUNCTIONS
 * @{
 */
/**
 * @brief  This function prints to console the command menu.
 * @param  None
 * @retval None
 */
void showCmds(void);

/**
 * @brief  This function prints to console the prompt character.
 * @param  None
 * @retval None
 */
void showPrompt(void);

/**
 * @brief  This function prints to console the help.
 * @param  None
 * @retval None
 */
void printHelp(void);

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

#endif /* _GNSS_UTILS_H_ */
