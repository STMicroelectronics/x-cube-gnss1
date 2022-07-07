/**
  ******************************************************************************
  * @file    heap.h
  * @author  MCD Application Team
  * @brief   Heap check functions.
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
#ifndef __HEAP_H__
#define __HEAP_H__

#include "stdint.h"
#include "stddef.h"

void stack_measure_prologue(void);
void stack_measure_epilogue(void);

#ifdef HEAP_DEBUG

void *heap_alloc(size_t a,size_t b);  
void heap_free(void  *p);
#else
#define  heap_free free
#define  heap_alloc calloc
#endif


void heap_stat(uint32_t *heap_max,uint32_t *heap_current, uint32_t *stacksize)  ;

#endif  /* __HEAP_H__ */

