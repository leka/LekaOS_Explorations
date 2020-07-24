/**
  ******************************************************************************
  * @file    JPEG/JPEG_DecodingUsingFs_DMA/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

#include "BSP_DISCO_F769NI/Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.h"
#include "BSP_DISCO_F769NI/Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_lcd.h"
#include "BSP_DISCO_F769NI/Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.h"

/* FatFs includes component */
#include "FatFs/ff_gen_drv.h"
#include "FatFs/drivers/sd_diskio.h"

/* JPEG utils includes */
#include "jpeg_utils.h"
#include "decode_dma.h"

/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define LCD_FRAME_BUFFER         0xC0000000
#define JPEG_OUTPUT_DATA_BUFFER  0xC0200000 

void LCD_FileErrorDisplay(void);

uint8_t LCD_Init(void);
void LTDC_Init(void);
void LCD_LayerInit(uint16_t LayerIndex, uint32_t Address);
uint32_t JPEG_FindFrameOffset(uint32_t offset, FIL *file);

void DMA2D_Init(uint32_t ImageWidth, uint32_t ImageHeight);
void DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t ImageWidth, uint16_t ImageHeight);
void DMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d);
void DMA2D_TransferErrorCallback(DMA2D_HandleTypeDef* hdma2d);
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OnError_Handler(const char*, int);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
