/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LSM6DSOX_MLC_H__
#define __LSM6DSOX_MLC_H__


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "lsm6dsox_reg.h"

//#include "lsm6dsox_vibration_monitoring.h"
#include "lsm6dsox_six_d_position.h"

#include "LSM6DSOX_CommunicationI2C.h"
#include "LSM6DSOX_MachineLearningCore.h"
#include "mbed.h"


/* Private macro -------------------------------------------------------------*/


#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
#define PIN_LSM6DSOX_INT NC


static DigitalOut INT_1_LSM6DSOX(A5,
						  0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

static I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
static Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);



/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI, rst;
static uint8_t tx_buffer[1000];

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

void lsm6dsox_mlc(void);

#endif	 // __LSM6DSOX_MLC_H__