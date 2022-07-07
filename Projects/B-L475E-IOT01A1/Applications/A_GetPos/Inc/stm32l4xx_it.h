/**
  ******************************************************************************
  * @file    stm32l4xx_it.h
  * @author  SRA
  * @brief   This file contains the headers of the interrupt handlers
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
#ifndef STM32L4xx_IT_H
#define STM32L4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup A_GetPos
 * @{
 */
		
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @addtogroup A_GetPos_PUBLIC_FUNCTIONS
 * @{
 */

/**
 * @brief  This function handles System tick timer.
 * @param  None
 * @retval None
 */ 
void SysTick_Handler(void);

/**
 * @brief  This function handles UART4 interrupt request.
 * @param  None
 * @retval None
 * @Note
 */
void UART4_IRQHandler(void);

/**
 * @brief  This function handles USART1 interrupt request.
 * @param  None
 * @retval None
 * @Note
 */
void USART1_IRQHandler(void);

/**
 * @brief  This function handles I2C1 event interrupt.
 * @param  None
 * @retval None
 * @Note
 */
void I2C1_EV_IRQHandler(void);

/**
 * @brief  This function handles I2C1 error interrupt.  
 * @param  None
 * @retval None
 * @Note
 */
void I2C1_ER_IRQHandler(void);

/**
  * @brief  This function handles external line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void);

/**
  * @brief  This function handles external line 13 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void);

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

#endif /* STM32L4xx_IT_H */

