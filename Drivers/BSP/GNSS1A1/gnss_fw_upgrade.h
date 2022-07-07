/**
  ******************************************************************************
  * @file    gnss_fw_upgrade.h
  * @author  SRA
  * @brief   Header file for gnss_fw_upgrade.c
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
#ifndef GNSS_FW_UPGRADE_H
#define GNSS_FW_UPGRADE_H

/** @addtogroup DRIVERS
  * @{
  */
  
/** @addtogroup BSP
 * @{
 */

/** @addtogroup X_NUCLEO_GNSS1A1
  * @{
  */

/** @addtogroup GNSS1A1Driver_EXPORTED_DEFINES
 * @{
 */

/**	
 * @brief  Function managing the FW upgrading process. This feature must be used
 *         with the Flash Updater Tool enclosed in this package in folder 
 *         Utilities\PC_Software\FirmwareUpdaterTool
 * @param  None
 * @retval None
 */
void FWUpgrade(void);
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
 
#endif /* GNSS_FW_UPGRADE_H */

