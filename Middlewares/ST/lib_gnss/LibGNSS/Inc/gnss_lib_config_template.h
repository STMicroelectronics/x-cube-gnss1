/**
  ******************************************************************************
  * @file    gnss_lib_config_template.h
  * @author  SRA
  * @brief   This file is a template to be modified in the project to configure
  *          how the libGNSS is supposed to access the GNSS
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
#ifndef GNSS_LIB_CONFIG_TEMPLATE_H
#define GNSS_LIB_CONFIG_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup MIDDLEWARES
 *  @{
 */

/** @addtogroup ST
 *  @{
 */

/** @addtogroup LIB_GNSS
 *  @{
 */
 
/** @addtogroup LibGNSS
 *  @{
 */

#ifdef USE_FREE_RTOS
#include "cmsis_os.h"
#define OS_Delay osDelay
#else
#define OS_Delay GNSS_Wrapper_Delay
#endif /* USE_FREE_RTOS */

/** @defgroup GNSS_DATA_FUNCTIONS GNSS DATA FUNCTIONS
 *  @brief Prototypes of the API allowing the application to interface the driver
 *  and interact with GNSS module (sending commands, retrieving parsed NMEA info, etc.).
 *  The implementation is up to the application according to specific needs.
 *  @{
 */

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Function to send data to Teseo module.
 * 
 * @param buffer Pointer to the data that needs to be sent. 
 *               As checksum is not a requirement for the module, the buffer can be created without the checksum.
 *               However, it is advised to include the checksum.
 *               Check Teseo module software manual on creating the complete string which includes the "*<checksum>"
 * @param length Length of the data that is being sent
 * 
 * @return int32_t A zero value indicates that data transmit operation was successful.
 *                 A non-zero value indicates operation failure.
 * 
 * @pre None.
 * @remarks None.
 */
int32_t GNSS_Wrapper_Send(uint8_t *buffer, uint16_t length);
/**
 * @brief This wrapper function calls the GNSS1A1_GNSS_Reset() function which
 *        deinitializes, performs hardware reset of the module, and then reinitializes the module.
 * 
 * @return int32_t A zero value indicates the reset function executed successfully.
 * 
 * @pre None.
 * @remarks This can be called by the application.
 */
int32_t GNSS_Wrapper_Reset(void);

/**
 * @brief Generates a delay based on the SysTick Timer.
 * 
 * @param Delay Count of the delay. Interval depends on the source of SysTick.
 * 
 * @return None
 * 
 * @pre None.
 * @remarks This can be called by the application.
 */
void    GNSS_Wrapper_Delay(uint32_t Delay);

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* GNSS_LIB_CONFIG_TEMPLATE_H */

