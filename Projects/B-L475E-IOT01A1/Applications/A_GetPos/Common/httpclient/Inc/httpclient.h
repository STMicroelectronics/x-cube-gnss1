/**
  ******************************************************************************
  * @file    httpclient.h
  * @author  MCD Application Team
  * @brief   Header file for httpclient.c
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
#ifndef __httpclient_h
#define __httpclient_h
#ifdef __cplusplus
 extern "C" {
#endif

void httpclient_test(void const *arg);
int httpclient_rxn(char *rxn_server_res,
                   char *rxn_server_auth,
                   uint8_t *buffer,
                   uint32_t buffer_size,
                   char *JsonData);
int httpclient_init(void);
void httpclient_deinit(void);

#ifdef __cplusplus
}
#endif
#endif /* __httpclient_h */

