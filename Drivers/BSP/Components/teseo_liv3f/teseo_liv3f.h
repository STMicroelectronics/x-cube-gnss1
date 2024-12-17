/**
  ******************************************************************************
  * @file    teseo_liv3f.h
  * @author  SRA
  * @brief   Teseo-LIV3F driver header file
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
#ifndef TESEO_LIV3F_H
#define TESEO_LIV3F_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "teseo_liv3f_queue.h"
#include <string.h>

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup COMPONENT COMPONENT
 * @{
 */

/** @addtogroup TESEO_LIV3F TESEO_LIV3F
 * @{
 */

/** @defgroup TESEO_LIV3F_Exported_Types TESEO_LIV3F Exported Types
 * @{
 */

/**
 * @brief 
 * Function pointer to assign BSP I2C/UART initialization function.
 */
typedef int32_t  (*TESEO_LIV3F_Init_Func)(void);

/**
 * @brief 
 * Function Pointer to assign BSP the BSP I2C/UART Deinitialization Function
 */ 
typedef int32_t  (*TESEO_LIV3F_DeInit_Func)(void);

/**
 * @brief 
 * Function Pointer to assign BSP I2C/UART Transmit function 
 */ 
 
typedef int32_t  (*TESEO_LIV3F_Transmit_IT_Func)(uint16_t, uint8_t *, uint16_t);

/**
 * @brief 
 * Function Pointer to assign BSP I2C/UART Receive function 
 */  
typedef int32_t  (*TESEO_LIV3F_Receive_IT_Func)(uint16_t, uint8_t *, uint16_t);

/**
 * @brief 
 * Function Pointer to toggle reset Teseo module using GPIO line
 */    
typedef void     (*TESEO_LIV3F_Reset_Func)(void);
typedef int32_t  (*TESEO_LIV3F_GetTick_Func)(void);
typedef void     (*TESEO_LIV3F_ClearOREF_Func)(void);

/**
 * @brief TESEO LIV3F IO Bus structure
 */
typedef struct
{
  TESEO_LIV3F_Init_Func          Init;
  TESEO_LIV3F_DeInit_Func        DeInit;
  uint32_t                       BusType; /* 0 means UART, 1 means I2C */
  uint16_t                       Address; /* I2C only */
  TESEO_LIV3F_Transmit_IT_Func   Transmit_IT;
  TESEO_LIV3F_Receive_IT_Func    Receive_IT;
  TESEO_LIV3F_Reset_Func         Reset;
  TESEO_LIV3F_GetTick_Func       GetTick;
  TESEO_LIV3F_ClearOREF_Func     ClearOREF;
} TESEO_LIV3F_IO_t;

/**
 * @brief TESEO LIV3F Status structure
 */
typedef struct
{
  unsigned int WakeUpStatus : 1;
} TESEO_LIV3F_Status_t;

/**
 * @brief TESEO LIV3F Object Info
 */
typedef struct
{
  TESEO_LIV3F_IO_t      IO;
  TESEO_LIV3F_ctx_t     Ctx;
  TESEO_LIV3F_Queue_t   *pTeseoQueue;
  uint8_t               is_initialized;
} TESEO_LIV3F_Object_t;

/**
 * @brief TESEO LIV3F Capabilities structure
 */
typedef struct
{
  uint8_t   Geofence;
  uint8_t   Datalog;
  uint8_t   Odemeter;
  uint8_t   AssistedGNSS;
} TESEO_LIV3F_Capabilities_t;

/** @defgroup TESEO_LIV3F_Exported_Constants TESEO_LIV3F Exported Constants
 * @{
 */

#define TESEO_LIV3F_OK                  (0)
#define TESEO_LIV3F_ERROR               (-1)

#define TESEO_LIV3F_UART_BUS            (0U)
#define TESEO_LIV3F_I2C_BUS             (1U)

#define TESEO_LIV3F_I2C_7BITS_ADDR      0x3A

/**
 * @}
 */

/** @addtogroup TESEO_LIV3F_Exported_Functions TESEO_LIV3F Exported Functions
 * @{
 */

/**
 * @brief Registers the bus I/O functions for the Teseo driver object.
 * 
 * This function assigns the function pointers to the Teseo driver object for 
 * initialization, deinitialization, transmit, receive, and other bus handling functions 
 * for either the UART or I2C depending on the selection made by the user.
 * 
 * @param pObj Teseo driver object which contains function pointers for bus functions, 
 * data pointers to message queue, and status of the object.
 * @param pIO Teseo bus initialization parameters identifying the type of bus, initialization,
 * and interrupt handling.
 * 
 * @return A zero value indicates a successful assignment.
 * 
 * @remark Called by GNSSA1_GNSS_Init() function and not exposed to application.
 */
int32_t TESEO_LIV3F_RegisterBusIO(
                                   TESEO_LIV3F_Object_t *pObj, 
                                   TESEO_LIV3F_IO_t *pIO
                                 );
int32_t                  TESEO_LIV3F_RegisterBusIO(TESEO_LIV3F_Object_t *pObj, TESEO_LIV3F_IO_t *pIO);

/**
 * @brief Initializes the Teseo module.
 * 
 * This function is called by the GNSS1A1_GNSS_Init() function from the application.
 * The GPIO Reset line from the MCU connected to the module is toggled to initiate 
 * a hardware reset. The function initializes the peripheral BSP where callbacks are initialized.
 * The function also initializes the Teseo data reception state machine and prepares the
 * receive wrapper function to receive data bytes.
 * 
 * @param pObj Teseo driver object which contains function pointers for bus functions.
 * 
 * @return A zero value indicates the peripheral hardware BSP and Teseo data reception
 * state machine has been initialized correctly. A non-zero value indicates an error in initialization.
 * 
 * @remark Called by GNSS1A1_GNSS_Init() function and not exposed to the application.
 */
int32_t                  TESEO_LIV3F_Init(TESEO_LIV3F_Object_t *pObj);

/**
 * @brief Deinitializes the Teseo module.
 * 
 * This function is called by the GNSS1A1_GNSS_DeInit() function from the application.
 * The function sets the UART/I2C state machine variables to their initial state.
 * The underlying BSP function is called to deinitialize UART/I2C.
 * 
 * @param pObj Teseo driver object which contains function pointers for bus functions.
 * 
 * @return A zero value indicates the peripheral hardware BSP and Teseo data reception
 * state machine have been deinitialized correctly. A non-zero value indicates an error in deinitialization.
 * 
 * @remark Called by GNSS1A1_GNSS_DeInit() function and not exposed to the application.
 */
int32_t                  TESEO_LIV3F_DeInit(TESEO_LIV3F_Object_t *pObj);

/**
 * @brief Returns the data pointer and message length from the function call of GNSS1A1_GNSS_GetMessage() function.
 * 
 * This function returns the data pointer and message length from the function call of 
 * GNSS1A1_GNSS_GetMessage() function.
 * 
 * @pre GNSS1A1_GNSS_Init() must be called before calling this function.
 * 
 * @param pObj Teseo Module object.
 * 
 * @return The queue pointer to the NMEA message is sent back to the calling function.
 *         The queue pointer information includes the address of the data and the length of the data.
 * 
 * @remark This function is not exposed to the application but is called from within the library.
 */
const TESEO_LIV3F_Msg_t* TESEO_LIV3F_GetMessage(const TESEO_LIV3F_Object_t *pObj);

/**
 * @brief Releases message from the GNSS queue and marks the queue slot as empty or writable.
 * 
 * This function releases a message from the GNSS queue and marks the queue slot as empty or writable.
 * This allows another incoming message from the module to occupy the buffer space that was previously
 * occupied by the message that was read. This function is called after GNSS1A1_GNSS_GetMessage() function.
 * 
 * @pre None.
 * 
 * @param pObj Teseo Module object that contains the address of queue slots.
 * @param Message Address that the message was read into in the application.
 * 
 * @return A zero value indicates that the queue space was marked as writable.
 *         A non-zero value indicates the message was not found in the queue and a buffer could not be marked as writable.
 * 
 * @remark None.
 */
int32_t                  TESEO_LIV3F_ReleaseMessage(const TESEO_LIV3F_Object_t *pObj, const TESEO_LIV3F_Msg_t *Message);

/**
 * @brief Sends a message to the Teseo module.
 * 
 * This function sends a message to the Teseo module.
 * 
 * @pre GNSS1A1_GNSS_Init() must be called before this function.
 * 
 * @param pObj Teseo module object used for all Teseo modules.
 * @param Message The TESEO_LIV3F_Msg_t type contains the address of the string and the length of the data.
 * 
 * @return A non-zero value indicates a failure in sending the message.
 * 
 * @remark The function is called from GNSS1A1_GNSS_Send() and not exposed to the application.
 */
int32_t                  TESEO_LIV3F_Send(const TESEO_LIV3F_Object_t *pObj, const TESEO_LIV3F_Msg_t *Message);

/**
 * @brief  Implement the API driver function to start (or resume after a given timeout) communication via I2C.
 * @retval none
 */
void                     TESEO_LIV3F_I2C_BackgroundProcess(void);

/* I2C callbacks */
/**
 * @brief I2C receive callback function.
 * 
 * This callback function will call the I2C state machine in the GNSS library to read 
 * data provided by I2C HAL. Calling this function implies that the I2C bus did 
 * not encounter any error.
 * 
 * @pre I2C driver should be initialized and callbacks should be enabled.
 * 
 * @param None.
 * 
 * @return None.
 * 
 * @remark GNSS1A1_RegisterRxCb is called with GNSS1A1_GNSS_I2C_RxCb as an argument. 
 *         GNSS1A1_GNSS_I2C_RxCb calls TESEO_LIV3F_I2C_RxCb.
 *         GNSS1A1_RegisterRxCb is defined to peripheral BSP function callback.
 *         The callbacks are registered automatically as part of Cube-MX settings and GNSS
 *         library configuration. No user action is needed.
 */
void TESEO_LIV3F_I2C_RxCb(void);

/**
 * @brief I2C error callback function.
 * 
 * This callback function will call the I2C state machine in the GNSS library with an 
 * error code and sets a timeout delay.
 * 
 * @pre I2C driver should be initialized with callbacks enabled.
 * 
 * @param None.
 * 
 * @return None.
 * 
 * @remark GNSS1A1_RegisterErrorCb is called with GNSS1A1_GNSS_I2C_ErrorCb as an argument. 
 *         GNSS1A1_GNSS_I2C_ErrorCb calls TESEO_LIV3F_I2C_ErrorCb.
 *         GNSS1A1_RegisterErrorCb is defined to peripheral BSP function callback.
 *         The callbacks are registered automatically as part of Cube-MX settings and GNSS
 *         library configuration. No user action is needed.
 */
void TESEO_LIV3F_I2C_ErrorCb(void);

/**
 * @brief I2C abort callback function.
 * 
 * This callback function sets the I2C state machine in an idle state.
 * 
 * @pre I2C driver should be initialized and callbacks enabled.
 * 
 * @param None.
 * 
 * @return None.
 * 
 * @remark GNSS1A1_RegisterAbortCb is called with GNSS1A1_GNSS_I2C_AbortCb as an argument.
 *         GNSS1A1_RegisterAbortCb is defined to peripheral BSP function callback.
 *         The callbacks are registered automatically as part of Cube-MX settings and GNSS
 *         library configuration. No user action is needed.
 */
void TESEO_LIV3F_I2C_AbortCb(void);

/**
 * @brief UART receive callback function.
 * 
 * This function is a callback function that is called once the UART data is received
 * by the UART interrupt.
 * 
 * @pre The UART peripheral must be initialized and callbacks enabled.
 * 
 * @param None.
 * 
 * @return None.
 * 
 * @remark GNSS1A1_RegisterRxCb is called with GNSS1A1_GNSS_UART_RxCb as an argument.
 *         GNSS1A1_RegisterRxCb is defined to peripheral BSP function callback.
 *         The callbacks are registered automatically as part of Cube-MX settings
 *         and GNSS library configuration. No user action is needed.
 */
void TESEO_LIV3F_UART_RxCb(void);

/**
 * @brief UART error callback function.
 * 
 * This callback function will call the UART data receive state machine to set the state
 * machine to frame discard state. It will also call the receive function to receive
 * a dummy character as a result of error.
 * 
 * @pre UART driver should be initialized with callbacks enabled.
 * 
 * @param None.
 * 
 * @return None.
 * 
 * @remark GNSS1A1_RegisterErrorCb is called with GNSS1A1_GNSS_UART_ErrorCb as an argument. 
 *         GNSS1A1_GNSS_UART_ErrorCb calls TESEO_LIV3F_UART_ErrorCb.
 *         GNSS1A1_RegisterErrorCb is defined to peripheral BSP function callback.
 *         The callbacks are registered automatically as part of Cube-MX settings and GNSS
 *         library configuration. No user action is needed.
 */
void TESEO_LIV3F_UART_ErrorCb(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

