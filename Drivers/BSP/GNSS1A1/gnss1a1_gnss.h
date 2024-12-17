/**
  ******************************************************************************
  * @file    gnss1a1_gnss.h
  * @author  SRA
  * @brief   Header file for gnss1a1_gnss.c
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
#ifndef GNSS1A1_GNSS_H
#define GNSS1A1_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gnss1a1_conf.h"

#ifndef USE_GNSS1A1_GNSS_TESEO_LIV3F
#define USE_GNSS1A1_GNSS_TESEO_LIV3F    1U
#endif

#if (USE_GNSS1A1_GNSS_TESEO_LIV3F == 1)
#include "teseo_liv3f.h"
#endif

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup GNSS1A1_GNSS GNSS1A1 GNSS
 * @{
 */

/** @defgroup GNSS1A1_GNSS_Exported_Types GNSS1A1 GNSS Exported Types
 * @{
 */

/**
 * @brief GNSS Message
 */
typedef struct
{
  uint8_t *buf;
  uint16_t len;
} GNSS1A1_GNSS_Msg_t;

/**
 * @brief GNSS Capabilities
 */
typedef struct
{
  uint8_t   Geofence;
  uint8_t   Datalog;
  uint8_t   Odemeter;
  uint8_t   AssistedGNSS;
} GNSS1A1_GNSS_Capabilities_t;

/**
 * @}
 */

/** @defgroup GNSS1A1_GNSS_Exported_Constants GNSS1A1 GNSS Exported Constants
 * @{
 */
#if (USE_GNSS1A1_GNSS_TESEO_LIV3F == 1)
#define GNSS1A1_TESEO_LIV3F 0
#endif

/**
 * @brief Indicates that a module within the Teseo family is being used.
 * 
 * This value is currently hardcoded to 1 by the macro `USE_GNSS1A1_GNSS_TESEO_LIV3F`.
 */

#define GNSS1A1_GNSS_INSTANCES_NBR      (USE_GNSS1A1_GNSS_TESEO_LIV3F)

#if (GNSS1A1_GNSS_INSTANCES_NBR == 0)
#error "No gnss instance has been selected"
#endif

/**
 * @}
 */

/** @addtogroup GNSS1A1_GNSS_Exported_Functions GNSS1A1 GNSS Exported Functions
 * @{
 */

/**********************************************************************
int32_t GNSS1A1_GNSS_Init	(
								uint32_t Instance
							)
Description:

This function initializes the Teseo module. 
The Instance parameter initializes the type of module. 
The initialization function also checks the Teseo communication bus 
that is being used i.e., UART or I2C. 
Based on the type of bus that is selected being appropriate function 
pointers are assigned to transmit and receive data.
The configuration parameter to select the type of bus is done 
during code generation.

Preconditions:
None.

Parameters:
Instance - Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be 
           used for all Teseo modules.
	 
Returns
If successful, the module returns 0 (BSP_ERROR_NONE). 
A return of non-zero value indicates an error.

Remarks
UART or I2C must be configured.
****************************************************************************/
int32_t GNSS1A1_GNSS_Init(uint32_t Instance);

/**
 * @brief Deinitializes the Teseo module object.
 *
 * This function will deinitialize the Teseo module object. The UART or I2C state machine 
 * that manages data reception is set into an idle state.
 *
 * @pre None.
 *
 * @param 
 * Instance: Hardcoded as GNSS1A1_TESEO_LIV3F. 
 *					This parameter can be used for all Teseo modules.
 *
 * @return If successful, the module returns 0 (BSP_ERROR_NONE). A return of non-zero value indicates an error.
 *
 * @remarks None.
 */
 
int32_t GNSS1A1_GNSS_DeInit(uint32_t Instance);

/**
 * @brief  Start (or resume after a given timeout) communication via I2C.
 * @param  Instance GNSS instance
 * @retval none
 */
void    GNSS1A1_GNSS_BackgroundProcess(uint32_t Instance);

/**
 * @brief Reads NMEA stream from Teseo module and returns the string.
 *
 * This function reads the NMEA stream from the Teseo module and returns the string.
 *
 * @pre GNSS1A1_GNSS_Init() and GNSS_PARSER_Init() need to be called before calling this function.
 *
 * @param Instance Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be used for all Teseo modules.
 *
 * @return Returns the latest message received from the Teseo module and also the length of the message.
 *         Returns NULL if no valid data is available in the queue.
 *
 * @remarks The I2C or UART callbacks must be registered.
 */
const   GNSS1A1_GNSS_Msg_t* GNSS1A1_GNSS_GetMessage(uint32_t Instance);

/**
 * @brief Releases message from the GNSS queue and marks the queue slot as empty or writable.
 *
 * This function releases a message from the GNSS queue and marks the queue slot as empty or writable.
 * This allows another incoming message from the module to occupy the buffer space that was previously
 * occupied by the message that was read. This function is called after GNSS1A1_GNSS_GetMessage().
 *
 * @pre GNSS1A1_GNSS_GetMessage() needs to be called before calling this function
 *
 * @param Instance Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be used for all Teseo modules.
 * @param Message Address that the message was read into in the application.
 *
 * @return A zero value indicates that the queue space was marked as writable.
 *         A non-zero value indicates the message was not found in the queue and a buffer could not be marked as writable.
 *
 * @remarks None.
 */
int32_t GNSS1A1_GNSS_ReleaseMessage(uint32_t Instance, const GNSS1A1_GNSS_Msg_t *Message);

/**
 * @brief Sends a message string to the Teseo module.
 * 
 * This function sends a message string to the Teseo module.
 * 
 * @pre GNSS1A1_GNSS_Init() must be called before this function.
 * 
 * @param Instance Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be used for all Teseo modules.
 * @param Message The GNSS1A1_GNSS_Msg_t type contains the message string that needs to be sent and the length of the message.
 * 
 * @return A non-zero value indicates a failure in sending the message.
 * 
 * @remark The I2C or UART callbacks must be registered.
 */
int32_t GNSS1A1_GNSS_Send( 
                           uint32_t Instance, 
                           const GNSS1A1_GNSS_Msg_t *Message
                         );

/**
 * @brief Returns the wake-up status of the module.
 * 
 * This function returns the wake-up status of the GNSS module.
 * 
 * @pre GNSS1A1_GNSS_Init() must be called before this function.
 * @pre I2C or UART driver needs to be initialized and callbacks should be registered.
 * 
 * @param Instance Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be used for all Teseo modules.
 * @param status Stores the logic status of the Wakeup Pin. If the pin is read in LOW state, a 0 is returned and if the pin is read in HIGH state, 1 is returned.
 * 
 * @return A zero value indicates that a pin value was read successfully.
 * 
 * @remark None
 */
int32_t GNSS1A1_GNSS_Wakeup_Status(
                                    uint32_t Instance, 
                                    uint8_t *status
                                  );

/**
 * @brief Resets the GNSS module.
 * 
 * Depending on the peripheral used, this function sets parameters that control 
 * data reception into an idle state as part of deinitialization. The module is reset.
 * The BSP initialization functions for the peripherals are invoked. The parameters 
 * that control data reception are set accordingly to receive data.
 * 
 * @pre None.
 * 
 * @param Instance Hardcoded as GNSS1A1_TESEO_LIV3F. This parameter can be used for all Teseo modules.
 * 
 * @return If successful, the module returns 0 (BSP_ERROR_NONE). A return of non-zero value indicates an error.
 * 
 * @remark UART or I2C must be configured.
 */
int32_t GNSS1A1_GNSS_Reset(
                            uint32_t Instance
                          );

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

#endif /* GNSS1A1_GNSS_H */

