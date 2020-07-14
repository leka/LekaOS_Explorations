/**
******************************************************************************
* @file    LekaPins.h
* @author  Yann Locatelli
* @brief   Temporary file gathering PinName
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAPINS_H__
#define __LEKAPINS_H__

/* Defines  ------------------------------------------------------------------*/
/*
//#define PIN_SWCLK (PA_14)
//#define PIN_SWO (PB_3)
//#define PIN_VCP_TX (PA_9)
//#define PIN_VCP_RX (PA_10)

#define PIN_Q_SPI_CLK (PB_2)
#define PIN_Q_SPI_IO0 (PD_11)
#define PIN_Q_SPI_IO1 (PC_10)
#define PIN_Q_SPI_IO2 (PE_2)
#define PIN_Q_SPI_IO3 (PD_13)
#define PIN_Q_SPI_CE_1 (PD_12)
#define PIN_Q_SPI_CE_2 (PH_6)
#define PIN_Q_SPI_CE_3 (PJ_4)
*/
#define PIN_SD_SPI_EN (PC_4)
#define PIN_SD_SPI_SCK (PA_5)
#define PIN_SD_SPI_MOSI (PA_7)
#define PIN_SD_SPI_MISO (PA_6)
/*
#define PIN_WE (PH_5)
#define PIN_CAS (PG_15)
#define PIN_RAS (PF_11)
#define PIN_CS (PH_3)
#define PIN_CKE (PH_2)
#define PIN_CLK (PG_8)
#define PIN_A0 (PF_0)
#define PIN_A1 (PF_1)
#define PIN_A2 (PF_2)
#define PIN_A3 (PF_3)
#define PIN_A4 (PF_4)
#define PIN_A5 (PF_5)
#define PIN_A6 (PF_12)
#define PIN_A7 (PF_13)
#define PIN_A8 (PF_14)
#define PIN_A9 (PF_15)
#define PIN_A10 (PG_0)
#define PIN_A11 (PG_1)
#define PIN_BA0 (PG_4)
#define PIN_BA1 (PG_5)
#define PIN_DQ0 (PD_14)
#define PIN_DQ1 (PD_15)
#define PIN_DQ2 (PD_0)
#define PIN_DQ3 (PD_1)
#define PIN_DQ4 (PE_7)
#define PIN_DQ5 (PE_8)
#define PIN_DQ6 (PE_9)
#define PIN_DQ7 (PE_10)
#define PIN_DQ8 (PE_11)
#define PIN_DQ9 (PE_12)
#define PIN_DQ10 (PE_13)
#define PIN_DQ11 (PE_14)
#define PIN_DQ12 (PE_15)
#define PIN_DQ13 (PD_8)
#define PIN_DQ14 (PD_9)
#define PIN_DQ15 (PD_10)
#define PIN_DQ16 (PH_8)
#define PIN_DQ17 (PH_9)
#define PIN_DQ18 (PH_10)
#define PIN_DQ19 (PH_11)
#define PIN_DQ20 (PH_12)
#define PIN_DQ21 (PH_13)
#define PIN_DQ22 (PH_14)
#define PIN_DQ23 (PH_15)
#define PIN_DQ24 (PI_0)
#define PIN_DQ25 (PI_1)
#define PIN_DQ26 (PI_2)
#define PIN_DQ27 (PI_3)
#define PIN_DQ28 (PI_6)
#define PIN_DQ29 (PI_7)
#define PIN_DQ30 (PI_9)
#define PIN_DQ31 (PI_10)
#define PIN_DQM0 (PE_0)
#define PIN_DQM1 (PE_1)
#define PIN_DQM2 (PI_4)
#define PIN_DQM3 (PI_5)
*/
#define PIN_I2C1_SCL (PB_8)
#define PIN_I2C1_SDA (PB_9)
#define PIN_INT1_IMU (PG_9)
#define PIN_DTRD_MAG (PA_15)
#define PIN_DRDY_TEMP (PB_5)

#define PIN_MICRO_MCU (PF_10)

#define PIN_V_LUMINOSITE (PB_1)
/*
#define PIN_IN1 (PH_0)
#define PIN_IN2 (PH_1)
#define PIN_ENA (PA_1)
#define PIN_ENB (PA_2)
#define PIN_IN3 (PE_4)
#define PIN_IN4 (PE_5)
*/
#define PIN_SPI5_NSS (PF_6)
#define PIN_SPI5_SCK (PF_7)
#define PIN_SPI5_MOSI (PF_9)
#define PIN_SPI5_MISO (PF_8)
#define PIN_RST_BLE (PE_3)
#define PIN_IRQ_BLE (PI_11)

#define PIN_USART2_ESP_TX (PD_5)
#define PIN_USART2_ESP_RX (PA_3)
// #define PIN_USART2_ESP_CTS (PD_3)
// #define PIN_USART2_ESP_RTS (PD_4)
#define PIN_RST_ESP (PJ_3)
#define PIN_ESP_ENABLE (PC_12)
/*
#define PIN_USART6_BTTX (PA_3)
#define PIN_USART6_BTRX (PD_5)
#define PIN_RST_BM64 (PC_8)
#define PIN_BT_WAKEUP (PG_7)

#define PIN_MCU_BT_WAKEUP (PA_0)
*/
#define PIN_SON_ON (PA_8)
#define PIN_MCU_SON_OUT (PA_4)

#endif	 // __LEKAPINS_H__
