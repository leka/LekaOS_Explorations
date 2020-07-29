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

//#define PIN_SWCLK (PA_14)
//#define PIN_SWO (PB_3)
//#define PIN_VCP_TX (PA_9)
//#define PIN_VCP_RX (PA_10)

#define PIN_RFID_RX (PA_11)
#define PIN_RFID_TX (PA_12)

#define PIN_SPI2_CLK (PB_13)
#define PIN_SPI2_MOSI (PB_15)
#define PIN_SPI6_CLK (PG_13)
#define PIN_SPI6_MOSI (PG_14)

#define PIN_I2C3_SCL (PH_7)
#define PIN_I2C3_SDA (PC_9)
#define PIN_MUX_RST (PI_12)
#define PIN_MUX_INTA (PI_13)
#define PIN_MUX_INTB (PG_10)


#endif	 // __LEKAPINS_H__
