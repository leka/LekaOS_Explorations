/*
 ******************************************************************************
 * @file    machine_learning_core.c
 * @author  Sensors Software Solution Team
 * @brief   The purpose of this example is to show how use the device
 *          Machine Learning Core (MLC) starting from an ".h" file 
 *          generated through with the tool "Machine Learning Core" 
 *          of Unico GUI.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 *
 * Some MLC examples are available at:
 * https://github.com/STMicroelectronics/STMems_Machine_Learning_Core
 * the same repository is linked to this repository in folder "_resources"
 *
 * For more information about Machine Learning Core tool please refer
 * to AN5259 "LSM6DSOX: Machine Learning Core".
 *
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109V3 + STEVAL-MKI197V1
 * - NUCLEO_F411RE + STEVAL-MKI197V1
 *
 * and STM32CubeMX tool with STM32CubeF4 MCU Package
 *
 * Used interfaces:
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * NUCLEO_STM32F411RE - Host side: UART(COM) to USB bridge
 *                    - I2C(Default) / SPI(supported)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */


#include "lsm6dsox_mlc.h"



#define PIN_I2C1_SDA (D14)
#define PIN_I2C1_SCL (D15)
// #define PIN_LSM6DSOX_INT NC

// DigitalOut INT_1_LSM6DSOX(A5,
// 						  0);	// This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2

// I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
// Communication::LSM6DSOX_I2C lsm6dsox_i2c(i2c1);
// Component::LSM6DSOX_Accelerometer lsm6dsox_accelerometer_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);
// Component::LSM6DSOX_Gyroscope lsm6dsox_gyroscope_component(lsm6dsox_i2c, PIN_LSM6DSOX_INT);


/* Main Example --------------------------------------------------------------*/
void lsm6dsox_mlc(void)
{

  /* Variable declaration */
  stmdev_ctx_t              dev_ctx;
  lsm6dsox_pin_int1_route_t   pin_int1_route;
  lsm6dsox_all_sources_t      status;
  uint8_t                     mlc_out[8];
  uint32_t                    i;

  /* Initialize mems driver interface */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg  = platform_read;
  dev_ctx.handle    = NULL;

  /* Wait sensor boot time */
  ThisThread::sleep_for(10);

  /* Check device ID */
  lsm6dsox_device_id_get(&dev_ctx, &whoamI);
  if (whoamI != LSM6DSOX_ID)
    while(1);

  /* Restore default configuration */
  lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
  do {
    lsm6dsox_reset_get(&dev_ctx, &rst);
  } while (rst);


  /* Start Machine Learning Core configuration */
  for ( i = 0; i < (sizeof(lsm6dsox_six_d_position) /
                    sizeof(ucf_line_t) ); i++ ){
  
    lsm6dsox_write_reg(&dev_ctx, lsm6dsox_six_d_position[i].address,
                       (uint8_t*)&lsm6dsox_six_d_position[i].data, 1);
     
  }

  /* End Machine Learning Core configuration */

  /* At this point the device is ready to run but if you need you can also
   * interact with the device but taking in account the MLC configuration.
   *
   * For more information about Machine Learning Core tool please refer
   * to AN5259 "LSM6DSOX: Machine Learning Core".
   */

  /* Turn off Sensors */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_OFF);
  lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_OFF);
 
  /* Disable I3C interface */
  lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);

  /* Enable Block Data Update */
  lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

  /* Set full scale */
  lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_4g);
  lsm6dsox_gy_full_scale_set(&dev_ctx, LSM6DSOX_2000dps);

  /* Route signals on interrupt pin 1 */
  lsm6dsox_pin_int1_route_get(&dev_ctx, &pin_int1_route);
  pin_int1_route.mlc1 = PROPERTY_ENABLE;
  lsm6dsox_pin_int1_route_set(&dev_ctx, pin_int1_route);

  /* Configure interrupt pin mode notification */
  lsm6dsox_int_notification_set(&dev_ctx, LSM6DSOX_BASE_PULSED_EMB_LATCHED);

  /* Set Output Data Rate.
   * Selected data rate have to be equal or greater with respect
   * with MLC data rate.
   */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_26Hz);
  lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_OFF);
 
  /* Main loop */
  while(1)
  {
    /* Read interrupt source registers in polling mode (no int) */
    lsm6dsox_all_sources_get(&dev_ctx, &status);
    if (status.mlc1){
     
      lsm6dsox_mlc_out_get(&dev_ctx, mlc_out);

      // MLC0_SRC register values for 6D position
      // 0 = None
      // 1 = X-axis pointing up
      // 2 = X-axis pointing down
      // 3 = Y-axis pointing up
      // 4 = Y-axis pointing down
      // 5 = Z-axis pointing up
      // 6 = Z-axis pointing down

      switch (mlc_out[0])
      {
      case 1:
        printf("X axis up\n");
        break;
      case 2:
        printf("X axis down\n");
        break;
      case 3:
        printf("Y axis up\n");
        break;
      case 4:
        printf("Y axis down\n");
        break;
      case 5:
        printf("Z axis up\n");
        break;
      case 6:
        printf("Z axis down\n");
        break;
      default:
        printf("None (data hasn't been updated by tree)\n");
        break;
      }
    }
  }
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
    //HAL_I2C_Mem_Write(handle, LSM6DSOX_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
    lsm6dsox_i2c.write(reg, len, bufp);
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
    //HAL_I2C_Mem_Read(handle, LSM6DSOX_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
    lsm6dsox_i2c.read(reg, len, bufp);
  return 0;
}
