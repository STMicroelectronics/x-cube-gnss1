/**
  *******************************************************************************
  * @file    gnss_if.h
  * @author  SRA
  * @brief   Provides API to access Low Level Driver - LibGNSS module middleware
  *******************************************************************************
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
#ifndef GNSS_IF_H
#define GNSS_IF_H

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

/** @defgroup GNSS_DATA_FUNCTIONS GNSS DATA FUNCTIONS
 *  @brief Prototypes of the API allowing the application to interface the driver
 *  and interact with GNSS module (sending commands, retrieving parsed NMEA info, etc.).
 *  The implementation is up to the application according to specific needs.
 *  @{
 */

/**	
 * @brief  This function puts a string on the console (via UART).
 * @param  pBuffer The string that contains the data to be written on the console
 * @retval None
 */
void GNSS_IF_ConsoleWrite(uint8_t *pBuffer);

/**
 * @brief  This function puts a single character on the console (via UART).
 * @param  pCh Charatcer to be printed
 * @retval None
 */
void GNSS_IF_ConsoleWriteChar(uint8_t *pCh);

/**	
 * @brief  This function gets data from the console (via UART).
 * @param  pBuffer The buffer where the received data are stored
 * @retval None
 */
void GNSS_IF_ConsoleRead(uint8_t *pBuffer, uint16_t size, uint32_t timeout);

/**	
 * @brief  Check if the console status is readable or not and set its status
 * @param  None
 * @retval 1 if the the console status is readable, 0 otherwise
 */
int8_t GNSS_IF_ConsoleReadable(void);

/**
 * @brief  This function sends a command to the GNSS module.
 * @param  pHandle Handler of the GNSS object. Is set to void * to guarantee the generality of the method
 * @param  pBuffer The buffer containing the command to be sent
 * @param  length  The length of the command
 * @param  timeout Timeout in ms
 * @retval None
 */
void GNSS_IF_WriteCommand(uint32_t Instance, uint8_t *pBuffer, uint16_t length, uint32_t timeout);

/**
 * @brief  osDelay wrapper
 * @param  Delay delay in ms
 * @retval None
 */
void GNSS_IF_Delay(uint32_t Delay);
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

#endif /* GNSS_IF_H */

