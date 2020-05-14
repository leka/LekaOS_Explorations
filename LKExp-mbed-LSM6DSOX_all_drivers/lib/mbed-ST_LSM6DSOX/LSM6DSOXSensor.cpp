/**
 ******************************************************************************
 * @file    LSM6DSOXSensor.cpp
 * @author  SRA
 * @version V1.0.0
 * @date    February 2019
 * @brief   Implementation of an LSM6DSOX Inertial Measurement Unit (IMU) 6 axes
 *          sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

#include "LSM6DSOXSensor.h"


/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param spi object of an helper class which handles the SPI peripheral
 * @param cs_pin the chip select pin
 * @param int1_pin the interrupt 1 pin
 * @param int2_pin the interrupt 2 pin
 * @param spi_type the SPI type
 */
LSM6DSOXSensor::LSM6DSOXSensor(SPI *spi, PinName cs_pin, PinName int1_pin, PinName int2_pin, SPI_type_t spi_type) : _dev_spi(spi), _cs_pin(cs_pin), _int1_irq(int1_pin), _int2_irq(int2_pin), _spi_type(spi_type)
{
  assert (spi);
  if (cs_pin == NC) 
  {
    printf ("ERROR LSM6DSOX CS MUST NOT BE NC\n\r");       
    _dev_spi = NULL;
    _dev_i2c = NULL;
    return;
  }

  _reg_ctx.write_reg = LSM6DSOX_io_write;
  _reg_ctx.read_reg = LSM6DSOX_io_read;
  _reg_ctx.handle = (void *)this;
  _cs_pin = 1;    
  _dev_i2c = NULL;
  _address = 0;
    
  if (_spi_type == SPI3W)
  {
    /* Enable SPI 3-Wires on the component */
    uint8_t data = 0x0C;
    lsm6dsox_write_reg(&_reg_ctx, LSM6DSOX_CTRL3_C, &data, 1);
  }
}


/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 * @param int1_pin the interrupt 1 pin
 * @param int2_pin the interrupt 2 pin
 */
LSM6DSOXSensor::LSM6DSOXSensor(DevI2C *i2c, uint8_t address, PinName int1_pin, PinName int2_pin) : _dev_i2c(i2c), _address(address), _cs_pin(NC), _int1_irq(int1_pin), _int2_irq(int2_pin)
{
  assert (i2c);
  _dev_spi = NULL;
  _reg_ctx.write_reg = LSM6DSOX_io_write;
  _reg_ctx.read_reg = LSM6DSOX_io_read;
  _reg_ctx.handle = (void *)this;
}

/**
 * @brief  Initializing the component
 * @param  init pointer to device specific initalization structure
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::init(void *init)
{
  /* Enable register address automatically incremented during a multiple byte
  access with a serial interface. */
  if (lsm6dsox_auto_increment_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable BDU */
  if (lsm6dsox_block_data_update_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* FIFO mode selection */
  if (lsm6dsox_fifo_mode_set(&_reg_ctx, LSM6DSOX_BYPASS_MODE) != 0)
  {
    return 1;
  }

  /* Output data rate selection - power down. */
  if (lsm6dsox_xl_data_rate_set(&_reg_ctx, LSM6DSOX_XL_ODR_OFF) != 0)
  {
    return 1;
  }

  /* Full scale selection. */
  if (lsm6dsox_xl_full_scale_set(&_reg_ctx, LSM6DSOX_2g) != 0)
  {
    return 1;
  }

  /* Output data rate selection - power down. */
  if (lsm6dsox_gy_data_rate_set(&_reg_ctx, LSM6DSOX_GY_ODR_OFF) != 0)
  {
    return 1;
  }

  /* Full scale selection. */
  if (lsm6dsox_gy_full_scale_set(&_reg_ctx, LSM6DSOX_2000dps) != 0)
  {
    return 1;
  }

  /* Select default output data rate. */
  _x_last_odr = LSM6DSOX_XL_ODR_104Hz;

  /* Select default output data rate. */
  _g_last_odr = LSM6DSOX_GY_ODR_104Hz;

  _x_is_enabled = 0;

  _g_is_enabled = 0;

  return 0;
}

/**
 * @brief  Read component ID
 * @param  id the WHO_AM_I value
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::read_id(uint8_t *id)
{
  if (lsm6dsox_device_id_get(&_reg_ctx, id) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable the LSM6DSOX accelerometer sensor
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_x()
{
  /* Check if the component is already enabled */
  if (_x_is_enabled == 1U)
  {
    return 0;
  }

  /* Output data rate selection. */
  if (lsm6dsox_xl_data_rate_set(&_reg_ctx, _x_last_odr) != 0)
  {
    return 1;
  }

  _x_is_enabled = 1;

  return 0;
}

/**
 * @brief  Disable the LSM6DSOX accelerometer sensor
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_x()
{
  /* Check if the component is already disabled */
  if (_x_is_enabled == 0U)
  {
    return 0;
  }

  /* Get current output data rate. */
  if (lsm6dsox_xl_data_rate_get(&_reg_ctx, &_x_last_odr) != 0)
  {
    return 1;
  }

  /* Output data rate selection - power down. */
  if (lsm6dsox_xl_data_rate_set(&_reg_ctx, LSM6DSOX_XL_ODR_OFF) != 0)
  {
    return 1;
  }

  _x_is_enabled = 0;

  return 0;
}

/**
 * @brief  Get the LSM6DSOX accelerometer sensor sensitivity
 * @param  sensitivity pointer where the sensitivity is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_sensitivity(float *sensitivity)
{
  int ret = 0;
  lsm6dsox_fs_xl_t full_scale;

  /* Read actual full scale selection from sensor. */
  if (lsm6dsox_xl_full_scale_get(&_reg_ctx, &full_scale) != 0)
  {
    return 1;
  }

  /* Store the sensitivity based on actual full scale. */
  switch (full_scale)
  {
    case LSM6DSOX_2g:
      *sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_2G;
      break;

    case LSM6DSOX_4g:
      *sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_4G;
      break;

    case LSM6DSOX_8g:
      *sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_8G;
      break;

    case LSM6DSOX_16g:
      *sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_16G;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Get the LSM6DSOX accelerometer sensor output data rate
 * @param  odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_odr(float *odr)
{
  int ret = 0;
  lsm6dsox_odr_xl_t odr_low_level;

  /* Get current output data rate. */
  if (lsm6dsox_xl_data_rate_get(&_reg_ctx, &odr_low_level) != 0)
  {
    return 1;
  }

  switch (odr_low_level)
  {
    case LSM6DSOX_XL_ODR_OFF:
      *odr = 0.0f;
      break;

    case LSM6DSOX_XL_ODR_6Hz5:
      *odr = 6.5f;
      break;

    case LSM6DSOX_XL_ODR_12Hz5:
      *odr = 12.5f;
      break;

    case LSM6DSOX_XL_ODR_26Hz:
      *odr = 26.0f;
      break;

    case LSM6DSOX_XL_ODR_52Hz:
      *odr = 52.0f;
      break;

    case LSM6DSOX_XL_ODR_104Hz:
      *odr = 104.0f;
      break;

    case LSM6DSOX_XL_ODR_208Hz:
      *odr = 208.0f;
      break;

    case LSM6DSOX_XL_ODR_417Hz:
      *odr = 417.0f;
      break;

    case LSM6DSOX_XL_ODR_833Hz:
      *odr = 833.0f;
      break;

    case LSM6DSOX_XL_ODR_1667Hz:
      *odr = 1667.0f;
      break;

    case LSM6DSOX_XL_ODR_3333Hz:
      *odr = 3333.0f;
      break;

    case LSM6DSOX_XL_ODR_6667Hz:
      *odr = 6667.0f;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Set the LSM6DSOX accelerometer sensor output data rate
 * @param  odr the output data rate value to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_x_odr(float odr)
{
  /* Check if the component is enabled */
  if (_x_is_enabled == 1U)
  {
    return set_x_odr_when_enabled(odr);
  }
  else
  {
    return set_x_odr_when_disabled(odr);
  }
}

/**
 * @brief  Set the LSM6DSOX accelerometer sensor output data rate when enabled
 * @param  odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_x_odr_when_enabled(float odr)
{
  lsm6dsox_odr_xl_t new_odr;

  new_odr = (odr <=   12.5f) ? LSM6DSOX_XL_ODR_12Hz5
          : (odr <=   26.0f) ? LSM6DSOX_XL_ODR_26Hz
          : (odr <=   52.0f) ? LSM6DSOX_XL_ODR_52Hz
          : (odr <=  104.0f) ? LSM6DSOX_XL_ODR_104Hz
          : (odr <=  208.0f) ? LSM6DSOX_XL_ODR_208Hz
          : (odr <=  417.0f) ? LSM6DSOX_XL_ODR_417Hz
          : (odr <=  833.0f) ? LSM6DSOX_XL_ODR_833Hz
          : (odr <= 1667.0f) ? LSM6DSOX_XL_ODR_1667Hz
          : (odr <= 3333.0f) ? LSM6DSOX_XL_ODR_3333Hz
          :                    LSM6DSOX_XL_ODR_6667Hz;

  /* Output data rate selection. */
  if (lsm6dsox_xl_data_rate_set(&_reg_ctx, new_odr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the LSM6DSOX accelerometer sensor output data rate when disabled
 * @param  odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_x_odr_when_disabled(float odr)
{
  _x_last_odr = (odr <=   12.5f) ? LSM6DSOX_XL_ODR_12Hz5
              : (odr <=   26.0f) ? LSM6DSOX_XL_ODR_26Hz
              : (odr <=   52.0f) ? LSM6DSOX_XL_ODR_52Hz
              : (odr <=  104.0f) ? LSM6DSOX_XL_ODR_104Hz
              : (odr <=  208.0f) ? LSM6DSOX_XL_ODR_208Hz
              : (odr <=  417.0f) ? LSM6DSOX_XL_ODR_417Hz
              : (odr <=  833.0f) ? LSM6DSOX_XL_ODR_833Hz
              : (odr <= 1667.0f) ? LSM6DSOX_XL_ODR_1667Hz
              : (odr <= 3333.0f) ? LSM6DSOX_XL_ODR_3333Hz
              :                    LSM6DSOX_XL_ODR_6667Hz;

  return 0;
}


/**
 * @brief  Get the LSM6DSOX accelerometer sensor full scale
 * @param  full_scale pointer where the full scale is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_fs(float *full_scale)
{
  int ret = 0;
  lsm6dsox_fs_xl_t fs_low_level;

  /* Read actual full scale selection from sensor. */
  if (lsm6dsox_xl_full_scale_get(&_reg_ctx, &fs_low_level) != 0)
  {
    return 1;
  }

  switch (fs_low_level)
  {
    case LSM6DSOX_2g:
      *full_scale =  2.0f;
      break;

    case LSM6DSOX_4g:
      *full_scale =  4.0f;
      break;

    case LSM6DSOX_8g:
      *full_scale =  8.0f;
      break;

    case LSM6DSOX_16g:
      *full_scale = 16.0f;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Set the LSM6DSOX accelerometer sensor full scale
 * @param  full_scale the functional full scale to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_x_fs(float full_scale)
{
  lsm6dsox_fs_xl_t new_fs;

  /* Seems like MISRA C-2012 rule 14.3a violation but only from single file statical analysis point of view because
     the parameter passed to the function is not known at the moment of analysis */
  new_fs = (full_scale <= 2.0f) ? LSM6DSOX_2g
         : (full_scale <= 4.0f) ? LSM6DSOX_4g
         : (full_scale <= 8.0f) ? LSM6DSOX_8g
         :                        LSM6DSOX_16g;

  if (lsm6dsox_xl_full_scale_set(&_reg_ctx, new_fs) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX accelerometer sensor raw axes
 * @param  value pointer where the raw values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_axes_raw(int16_t *value)
{
  axis3bit16_t data_raw;

  /* Read raw data values. */
  if (lsm6dsox_acceleration_raw_get(&_reg_ctx, data_raw.u8bit) != 0)
  {
    return 1;
  }

  /* Format the data. */
  value[0] = data_raw.i16bit[0];
  value[1] = data_raw.i16bit[1];
  value[2] = data_raw.i16bit[2];

  return 0;
}


/**
 * @brief  Get the LSM6DSOX accelerometer sensor axes
 * @param  acceleration pointer where the values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_axes(int32_t *acceleration)
{
  axis3bit16_t data_raw;
  float sensitivity = 0.0f;

  /* Read raw data values. */
  if (lsm6dsox_acceleration_raw_get(&_reg_ctx, data_raw.u8bit) != 0)
  {
    return 1;
  }

  /* Get LSM6DSOX actual sensitivity. */
  if (get_x_sensitivity(&sensitivity) != 0)
  {
    return 1;
  }

  /* Calculate the data. */
  acceleration[0] = (int32_t)((float)((float)data_raw.i16bit[0] * sensitivity));
  acceleration[1] = (int32_t)((float)((float)data_raw.i16bit[1] * sensitivity));
  acceleration[2] = (int32_t)((float)((float)data_raw.i16bit[2] * sensitivity));

  return 0;
}


/**
 * @brief  Enable the LSM6DSOX gyroscope sensor
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_g()
{
  /* Check if the component is already enabled */
  if (_g_is_enabled == 1U)
  {
    return 0;
  }

  /* Output data rate selection. */
  if (lsm6dsox_gy_data_rate_set(&_reg_ctx, _g_last_odr) != 0)
  {
    return 1;
  }

  _g_is_enabled = 1;

  return 0;
}


/**
 * @brief  Disable the LSM6DSOX gyroscope sensor
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_g()
{
  /* Check if the component is already disabled */
  if (_g_is_enabled == 0U)
  {
    return 0;
  }

  /* Get current output data rate. */
  if (lsm6dsox_gy_data_rate_get(&_reg_ctx, &_g_last_odr) != 0)
  {
    return 1;
  }

  /* Output data rate selection - power down. */
  if (lsm6dsox_gy_data_rate_set(&_reg_ctx, LSM6DSOX_GY_ODR_OFF) != 0)
  {
    return 1;
  }

  _g_is_enabled = 0;

  return 0;
}

/**
 * @brief  Get the LSM6DSOX gyroscope sensor sensitivity
 * @param  sensitivity pointer where the sensitivity is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_sensitivity(float *sensitivity)
{
  int ret = 0;
  lsm6dsox_fs_g_t full_scale;

  /* Read actual full scale selection from sensor. */
  if (lsm6dsox_gy_full_scale_get(&_reg_ctx, &full_scale) != 0)
  {
    return 1;
  }

  /* Store the sensitivity based on actual full scale. */
  switch (full_scale)
  {
    case LSM6DSOX_125dps:
      *sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_125DPS;
      break;

    case LSM6DSOX_250dps:
      *sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_250DPS;
      break;

    case LSM6DSOX_500dps:
      *sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_500DPS;
      break;

    case LSM6DSOX_1000dps:
      *sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_1000DPS;
      break;

    case LSM6DSOX_2000dps:
      *sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_2000DPS;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Get the LSM6DSOX gyroscope sensor output data rate
 * @param  odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_odr(float *odr)
{
  int ret = 0;
  lsm6dsox_odr_g_t odr_low_level;

  /* Get current output data rate. */
  if (lsm6dsox_gy_data_rate_get(&_reg_ctx, &odr_low_level) != 0)
  {
    return 1;
  }

  switch (odr_low_level)
  {
    case LSM6DSOX_GY_ODR_OFF:
      *odr = 0.0f;
      break;

    case LSM6DSOX_GY_ODR_12Hz5:
      *odr = 12.5f;
      break;

    case LSM6DSOX_GY_ODR_26Hz:
      *odr = 26.0f;
      break;

    case LSM6DSOX_GY_ODR_52Hz:
      *odr = 52.0f;
      break;

    case LSM6DSOX_GY_ODR_104Hz:
      *odr = 104.0f;
      break;

    case LSM6DSOX_GY_ODR_208Hz:
      *odr = 208.0f;
      break;

    case LSM6DSOX_GY_ODR_417Hz:
      *odr = 417.0f;
      break;

    case LSM6DSOX_GY_ODR_833Hz:
      *odr = 833.0f;
      break;

    case LSM6DSOX_GY_ODR_1667Hz:
      *odr =  1667.0f;
      break;

    case LSM6DSOX_GY_ODR_3333Hz:
      *odr =  3333.0f;
      break;

    case LSM6DSOX_GY_ODR_6667Hz:
      *odr =  6667.0f;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Set the LSM6DSOX gyroscope sensor output data rate
 * @param  odr the output data rate value to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_g_odr(float odr)
{
  /* Check if the component is enabled */
  if (_g_is_enabled == 1U)
  {
    return set_g_odr_when_enabled(odr);
  }
  else
  {
    return set_g_odr_when_disabled(odr);
  }
}

/**
 * @brief  Set the LSM6DSOX gyroscope sensor output data rate when enabled
 * @param  odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_g_odr_when_enabled(float odr)
{
  lsm6dsox_odr_g_t new_odr;

  new_odr = (odr <=   12.5f) ? LSM6DSOX_GY_ODR_12Hz5
          : (odr <=   26.0f) ? LSM6DSOX_GY_ODR_26Hz
          : (odr <=   52.0f) ? LSM6DSOX_GY_ODR_52Hz
          : (odr <=  104.0f) ? LSM6DSOX_GY_ODR_104Hz
          : (odr <=  208.0f) ? LSM6DSOX_GY_ODR_208Hz
          : (odr <=  417.0f) ? LSM6DSOX_GY_ODR_417Hz
          : (odr <=  833.0f) ? LSM6DSOX_GY_ODR_833Hz
          : (odr <= 1667.0f) ? LSM6DSOX_GY_ODR_1667Hz
          : (odr <= 3333.0f) ? LSM6DSOX_GY_ODR_3333Hz
          :                    LSM6DSOX_GY_ODR_6667Hz;

  /* Output data rate selection. */
  if (lsm6dsox_gy_data_rate_set(&_reg_ctx, new_odr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the LSM6DSOX gyroscope sensor output data rate when disabled
 * @param  odr the functional output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_g_odr_when_disabled(float odr)
{
  _g_last_odr = (odr <=   12.5f) ? LSM6DSOX_GY_ODR_12Hz5
              : (odr <=   26.0f) ? LSM6DSOX_GY_ODR_26Hz
              : (odr <=   52.0f) ? LSM6DSOX_GY_ODR_52Hz
              : (odr <=  104.0f) ? LSM6DSOX_GY_ODR_104Hz
              : (odr <=  208.0f) ? LSM6DSOX_GY_ODR_208Hz
              : (odr <=  417.0f) ? LSM6DSOX_GY_ODR_417Hz
              : (odr <=  833.0f) ? LSM6DSOX_GY_ODR_833Hz
              : (odr <= 1667.0f) ? LSM6DSOX_GY_ODR_1667Hz
              : (odr <= 3333.0f) ? LSM6DSOX_GY_ODR_3333Hz
              :                    LSM6DSOX_GY_ODR_6667Hz;

  return 0;
}


/**
 * @brief  Get the LSM6DSOX gyroscope sensor full scale
 * @param  full_scale pointer where the full scale is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_fs(float *full_scale)
{
  int ret = 0;
  lsm6dsox_fs_g_t fs_low_level;

  /* Read actual full scale selection from sensor. */
  if (lsm6dsox_gy_full_scale_get(&_reg_ctx, &fs_low_level) != 0)
  {
    return 1;
  }

  switch (fs_low_level)
  {
    case LSM6DSOX_125dps:
      *full_scale =  125.0f;
      break;

    case LSM6DSOX_250dps:
      *full_scale =  250.0f;
      break;

    case LSM6DSOX_500dps:
      *full_scale =  500.0f;
      break;

    case LSM6DSOX_1000dps:
      *full_scale = 1000.0f;
      break;

    case LSM6DSOX_2000dps:
      *full_scale = 2000.0f;
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Set the LSM6DSOX gyroscope sensor full scale
 * @param  full_scale the functional full scale to be set
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_g_fs(float full_scale)
{
  lsm6dsox_fs_g_t new_fs;

  new_fs = (full_scale <= 125.0f)  ? LSM6DSOX_125dps
         : (full_scale <= 250.0f)  ? LSM6DSOX_250dps
         : (full_scale <= 500.0f)  ? LSM6DSOX_500dps
         : (full_scale <= 1000.0f) ? LSM6DSOX_1000dps
         :                           LSM6DSOX_2000dps;

  if (lsm6dsox_gy_full_scale_set(&_reg_ctx, new_fs) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX gyroscope sensor raw axes
 * @param  value pointer where the raw values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_axes_raw(int16_t *value)
{
  axis3bit16_t data_raw;

  /* Read raw data values. */
  if (lsm6dsox_angular_rate_raw_get(&_reg_ctx, data_raw.u8bit) != 0)
  {
    return 1;
  }

  /* Format the data. */
  value[0] = data_raw.i16bit[0];
  value[1] = data_raw.i16bit[1];
  value[2] = data_raw.i16bit[2];

  return 0;
}


/**
 * @brief  Get the LSM6DSOX gyroscope sensor axes
 * @param  angular_rate pointer where the values of the axes are written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_axes(int32_t *angular_rate)
{
  axis3bit16_t data_raw;
  float sensitivity;

  /* Read raw data values. */
  if (lsm6dsox_angular_rate_raw_get(&_reg_ctx, data_raw.u8bit) != 0)
  {
    return 1;
  }

  /* Get LSM6DSOX actual sensitivity. */
  if (get_g_sensitivity(&sensitivity) != 0)
  {
    return 1;
  }

  /* Calculate the data. */
  angular_rate[0] = (int32_t)((float)((float)data_raw.i16bit[0] * sensitivity));
  angular_rate[1] = (int32_t)((float)((float)data_raw.i16bit[1] * sensitivity));
  angular_rate[2] = (int32_t)((float)((float)data_raw.i16bit[2] * sensitivity));

  return 0;
}


/**
 * @brief  Get the LSM6DSOX register value
 * @param  reg address to be read
 * @param  data pointer where the value is written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::read_reg(uint8_t reg, uint8_t *data)
{
  if (lsm6dsox_read_reg(&_reg_ctx, reg, data, 1) != 0)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Set the LSM6DSOX register value
 * @param  reg address to be written
 * @param  data value to be written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::write_reg(uint8_t reg, uint8_t data)
{
  if (lsm6dsox_write_reg(&_reg_ctx, reg, &data, 1) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the interrupt latch
 * @param  status value to be written
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_interrupt_latch(uint8_t status)
{
  if (status > 1U)
  {
    return 1;
  }

  if (lsm6dsox_int_notification_set(&_reg_ctx, (lsm6dsox_lir_t)status) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable free fall detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_free_fall_detection(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(416.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* FF_DUR setting */
  if (lsm6dsox_ff_dur_set(&_reg_ctx, 0x06) != 0)
  {
    return 1;
  }

  /* WAKE_DUR setting */
  if (lsm6dsox_wkup_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* SLEEP_DUR setting */
  if (lsm6dsox_act_sleep_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* FF_THS setting */
  if (lsm6dsox_ff_threshold_set(&_reg_ctx, LSM6DSOX_FF_TSH_312mg) != 0)
  {
    return 1;
  }

  /* Enable free fall event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.md1_cfg.int1_ff = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.md2_cfg.int2_ff = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Disable free fall detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_free_fall_detection()
{
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Disable free fall event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.md1_cfg.int1_ff = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.md2_cfg.int2_ff = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* FF_DUR setting */
  if (lsm6dsox_ff_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* FF_THS setting */
  if (lsm6dsox_ff_threshold_set(&_reg_ctx, LSM6DSOX_FF_TSH_156mg) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set free fall threshold
 * @param  thr free fall detection threshold
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_free_fall_threshold(uint8_t thr)
{
  if (lsm6dsox_ff_threshold_set(&_reg_ctx, (lsm6dsox_ff_ths_t)thr) != 0)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Set free fall duration
 * @param  dur free fall detection duration
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_free_fall_duration(uint8_t dur)
{
  if (lsm6dsox_ff_dur_set(&_reg_ctx, dur) != 0)
  {
    return 1;
  }

  return 0;
}


/**
 * @brief  Enable pedometer
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_pedometer()
{
    lsm6dsox_pin_int1_route_t val;

  /* Output Data Rate selection */
  if (set_x_odr(26.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* Enable pedometer algorithm. */
  if (lsm6dsox_pedo_sens_set(&_reg_ctx, LSM6DSOX_PEDO_BASE_MODE) != 0)
  {
    return 1;
  }

  /* Enable step detector on INT1 pin */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val) != 0)
  {
    return 1;
  }

  val.emb_func_int1.int1_step_detector = PROPERTY_ENABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Disable pedometer
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_pedometer()
{
  lsm6dsox_pin_int1_route_t val1;

  /* Disable step detector on INT1 pin */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.emb_func_int1.int1_step_detector = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  /* Disable pedometer algorithm. */
  if (lsm6dsox_pedo_sens_set(&_reg_ctx, LSM6DSOX_PEDO_DISABLE) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get step count
 * @param  step_count step counter
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_step_counter(uint16_t *step_count)
{
  if (lsm6dsox_number_of_steps_get(&_reg_ctx, (uint8_t *)step_count) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable step counter reset
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::reset_step_counter()
{
  if (lsm6dsox_steps_reset(&_reg_ctx) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable tilt detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_tilt_detection(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(26.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* Enable tilt calculation. */
  if (lsm6dsox_tilt_sens_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable tilt event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.emb_func_int1.int1_tilt = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.emb_func_int2.int2_tilt = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Disable tilt detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_tilt_detection()
{
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Disable tilt event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.emb_func_int1.int1_tilt = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.emb_func_int2.int2_tilt = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* Disable tilt calculation. */
  if (lsm6dsox_tilt_sens_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable wake up detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_wake_up_detection(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(416.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* WAKE_DUR setting */
  if (lsm6dsox_wkup_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Set wake up threshold. */
  if (lsm6dsox_wkup_threshold_set(&_reg_ctx, 0x02) != 0)
  {
    return 1;
  }

  /* Enable wake up event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.md1_cfg.int1_wu = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.md2_cfg.int2_wu = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}


/**
 * @brief  Disable wake up detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_wake_up_detection()
{
    lsm6dsox_pin_int1_route_t val1;
    lsm6dsox_pin_int2_route_t val2;

  /* Disable wake up event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.md1_cfg.int1_wu = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.md2_cfg.int2_wu = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* Reset wake up threshold. */
  if (lsm6dsox_wkup_threshold_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* WAKE_DUR setting */
  if (lsm6dsox_wkup_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set wake up threshold
 * @param  thr wake up detection threshold
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_wake_up_threshold(uint8_t thr)
{
  /* Set wake up threshold. */
  if (lsm6dsox_wkup_threshold_set(&_reg_ctx, thr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set wake up duration
 * @param  dur wake up detection duration
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_wake_up_duration(uint8_t dur)
{
  /* Set wake up duration. */
  if (lsm6dsox_wkup_dur_set(&_reg_ctx, dur) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable single tap detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_single_tap_detection(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(416.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* Enable X direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_x_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable Y direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_y_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable Z direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_z_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Set tap threshold. */
  if (lsm6dsox_tap_threshold_x_set(&_reg_ctx, 0x08) != 0)
  {
    return 1;
  }

  /* Set tap shock time window. */
  if (lsm6dsox_tap_shock_set(&_reg_ctx, 0x02) != 0)
  {
    return 1;
  }

  /* Set tap quiet time window. */
  if (lsm6dsox_tap_quiet_set(&_reg_ctx, 0x01) != 0)
  {
    return 1;
  }

  /* _NOTE_: Tap duration time window - don't care for single tap. */

  /* _NOTE_: Single/Double Tap event - don't care of this flag for single tap. */

  /* Enable single tap event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.md1_cfg.int1_single_tap = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.md2_cfg.int2_single_tap = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Disable single tap detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_single_tap_detection()
{
    lsm6dsox_pin_int1_route_t val1;
    lsm6dsox_pin_int2_route_t val2;

  /* Disable single tap event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.md1_cfg.int1_single_tap = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.md2_cfg.int2_single_tap = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* Reset tap quiet time window. */
  if (lsm6dsox_tap_quiet_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Reset tap shock time window. */
  if (lsm6dsox_tap_shock_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Reset tap threshold. */
  if (lsm6dsox_tap_threshold_x_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Disable Z direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_z_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  /* Disable Y direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_y_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  /* Disable X direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_x_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable double tap detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_double_tap_detection(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(416.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* Enable X direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_x_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable Y direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_y_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Enable Z direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_z_set(&_reg_ctx, PROPERTY_ENABLE) != 0)
  {
    return 1;
  }

  /* Set tap threshold. */
  if (lsm6dsox_tap_threshold_x_set(&_reg_ctx, 0x08) != 0)
  {
    return 1;
  }

  /* Set tap shock time window. */
  if (lsm6dsox_tap_shock_set(&_reg_ctx, 0x03) != 0)
  {
    return 1;
  }

  /* Set tap quiet time window. */
  if (lsm6dsox_tap_quiet_set(&_reg_ctx, 0x03) != 0)
  {
    return 1;
  }

  /* Set tap duration time window. */
  if (lsm6dsox_tap_dur_set(&_reg_ctx, 0x08) != 0)
  {
    return 1;
  }

  /* Single and double tap enabled. */
  if (lsm6dsox_tap_mode_set(&_reg_ctx, LSM6DSOX_BOTH_SINGLE_DOUBLE) != 0)
  {
    return 1;
  }

  /* Enable double tap event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.md1_cfg.int1_double_tap = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.md2_cfg.int2_double_tap = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Disable double tap detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_double_tap_detection()
{
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Disable double tap event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.md1_cfg.int1_double_tap = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.md2_cfg.int2_double_tap = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* Only single tap enabled. */
  if (lsm6dsox_tap_mode_set(&_reg_ctx, LSM6DSOX_ONLY_SINGLE) != 0)
  {
    return 1;
  }

  /* Reset tap duration time window. */
  if (lsm6dsox_tap_dur_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Reset tap quiet time window. */
  if (lsm6dsox_tap_quiet_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Reset tap shock time window. */
  if (lsm6dsox_tap_shock_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Reset tap threshold. */
  if (lsm6dsox_tap_threshold_x_set(&_reg_ctx, 0x00) != 0)
  {
    return 1;
  }

  /* Disable Z direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_z_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  /* Disable Y direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_y_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  /* Disable X direction in tap recognition. */
  if (lsm6dsox_tap_detection_on_x_set(&_reg_ctx, PROPERTY_DISABLE) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set tap threshold
 * @param  thr tap threshold
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_tap_threshold(uint8_t thr)
{
  /* Set tap threshold. */
  if (lsm6dsox_tap_threshold_x_set(&_reg_ctx, thr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set tap shock time
 * @param  time tap shock time
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_tap_shock_time(uint8_t time)
{
  /* Set tap shock time window. */
  if (lsm6dsox_tap_shock_set(&_reg_ctx, time) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set tap quiet time
 * @param  time tap quiet time
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_tap_quiet_time(uint8_t time)
{
  /* Set tap quiet time window. */
  if (lsm6dsox_tap_quiet_set(&_reg_ctx, time) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set tap duration time
 * @param  time tap duration time
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_tap_duration_time(uint8_t time)
{
  /* Set tap duration time window. */
  if (lsm6dsox_tap_dur_set(&_reg_ctx, time) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Enable 6D orientation detection
 * @param  int_pin interrupt pin line to be used
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::enable_6d_orientation(LSM6DSOX_Interrupt_Pin_t int_pin)
{
  int ret = 0;
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Output Data Rate selection */
  if (set_x_odr(416.0f) != 0)
  {
    return 1;
  }

  /* Full scale selection */
  if (set_x_fs(2.0f) != 0)
  {
    return 1;
  }

  /* 6D orientation enabled. */
  if (lsm6dsox_6d_threshold_set(&_reg_ctx, LSM6DSOX_DEG_60) != 0)
  {
    return 1;
  }

  /* Enable 6D orientation event on either INT1 or INT2 pin */
  switch (int_pin)
  {
    case LSM6DSOX_INT1_PIN:
      if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }

      val1.md1_cfg.int1_6d = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
      {
        return 1;
      }
      break;

    case LSM6DSOX_INT2_PIN:
      if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }

      val2.md2_cfg.int2_6d = PROPERTY_ENABLE;

      if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
      {
        return 1;
      }
      break;

    default:
      ret = 1;
      break;
  }

  return ret;
}

/**
 * @brief  Disable 6D orientation detection
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::disable_6d_orientation()
{
  lsm6dsox_pin_int1_route_t val1;
  lsm6dsox_pin_int2_route_t val2;

  /* Disable 6D orientation event on both INT1 and INT2 pins */
  if (lsm6dsox_pin_int1_route_get(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  val1.md1_cfg.int1_6d = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int1_route_set(&_reg_ctx, &val1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_pin_int2_route_get(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  val2.md2_cfg.int2_6d = PROPERTY_DISABLE;

  if (lsm6dsox_pin_int2_route_set(&_reg_ctx, &val2) != 0)
  {
    return 1;
  }

  /* Reset 6D orientation. */
  if (lsm6dsox_6d_threshold_set(&_reg_ctx, LSM6DSOX_DEG_80) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set 6D orientation threshold
 * @param  thr 6D Orientation detection threshold
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_6d_orientation_threshold(uint8_t thr)
{
  if (lsm6dsox_6d_threshold_set(&_reg_ctx, (lsm6dsox_sixd_ths_t)thr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the status of XLow orientation
 * @param  xl the status of XLow orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_xl(uint8_t *xl)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *xl = data.xl;

  return 0;
}

/**
 * @brief  Get the status of XHigh orientation
 * @param  xh the status of XHigh orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_xh(uint8_t *xh)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *xh = data.xh;

  return 0;
}

/**
 * @brief  Get the status of YLow orientation
 * @param  yl the status of YLow orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_yl(uint8_t *yl)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *yl = data.yl;

  return 0;
}

/**
 * @brief  Get the status of YHigh orientation
 * @param  yh the status of YHigh orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_yh(uint8_t *yh)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *yh = data.yh;

  return 0;
}

/**
 * @brief  Get the status of ZLow orientation
 * @param  zl the status of ZLow orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_zl(uint8_t *zl)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *zl = data.zl;

  return 0;
}

/**
 * @brief  Get the status of ZHigh orientation
 * @param  zh the status of ZHigh orientation
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_6d_orientation_zh(uint8_t *zh)
{
  lsm6dsox_d6d_src_t data;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&data, 1) != 0)
  {
    return 1;
  }

  *zh = data.zh;

  return 0;
}

/**
 * @brief  Get the LSM6DSOX ACC data ready bit value
 * @param  status the status of data ready bit
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_x_drdy_status(uint8_t *status)
{
  if (lsm6dsox_xl_flag_data_ready_get(&_reg_ctx, status) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the status of all hardware events
 * @param  status the status of all hardware events
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_event_status(LSM6DSOX_Event_Status_t *status)
{
  uint8_t tilt_ia = 0U;
  lsm6dsox_wake_up_src_t wake_up_src;
  lsm6dsox_tap_src_t tap_src;
  lsm6dsox_d6d_src_t d6d_src;
  lsm6dsox_emb_func_src_t func_src;
  lsm6dsox_md1_cfg_t md1_cfg;
  lsm6dsox_md2_cfg_t md2_cfg;
  lsm6dsox_emb_func_int1_t int1_ctrl;
  lsm6dsox_emb_func_int2_t int2_ctrl;

  (void)memset((void *)status, 0x0, sizeof(LSM6DSOX_Event_Status_t));

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_WAKE_UP_SRC, (uint8_t *)&wake_up_src, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_TAP_SRC, (uint8_t *)&tap_src, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_D6D_SRC, (uint8_t *)&d6d_src, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_mem_bank_set(&_reg_ctx, LSM6DSOX_EMBEDDED_FUNC_BANK) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_EMB_FUNC_SRC, (uint8_t *)&func_src, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_EMB_FUNC_INT1, (uint8_t *)&int1_ctrl, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_EMB_FUNC_INT2, (uint8_t *)&int2_ctrl, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_mem_bank_set(&_reg_ctx, LSM6DSOX_USER_BANK) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_MD1_CFG, (uint8_t *)&md1_cfg, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_MD2_CFG, (uint8_t *)&md2_cfg, 1) != 0)
  {
    return 1;
  }

  if (lsm6dsox_tilt_flag_data_ready_get(&_reg_ctx, &tilt_ia) != 0)
  {
    return 1;
  }

  if ((md1_cfg.int1_ff == 1U) || (md2_cfg.int2_ff == 1U))
  {
    if (wake_up_src.ff_ia == 1U)
    {
      status->FreeFallStatus = 1;
    }
  }

  if ((md1_cfg.int1_wu == 1U) || (md2_cfg.int2_wu == 1U))
  {
    if (wake_up_src.wu_ia == 1U)
    {
      status->WakeUpStatus = 1;
    }
  }

  if ((md1_cfg.int1_single_tap == 1U) || (md2_cfg.int2_single_tap == 1U))
  {
    if (tap_src.single_tap == 1U)
    {
      status->TapStatus = 1;
    }
  }

  if ((md1_cfg.int1_double_tap == 1U) || (md2_cfg.int2_double_tap == 1U))
  {
    if (tap_src.double_tap == 1U)
    {
      status->DoubleTapStatus = 1;
    }
  }

  if ((md1_cfg.int1_6d == 1U) || (md2_cfg.int2_6d == 1U))
  {
    if (d6d_src.d6d_ia == 1U)
    {
      status->D6DOrientationStatus = 1;
    }
  }

  if (int1_ctrl.int1_step_detector == 1U)
  {
    if (func_src.step_detected == 1U)
    {
      status->StepStatus = 1;
    }
  }

  if ((int1_ctrl.int1_tilt == 1U) || (int2_ctrl.int2_tilt == 1U))
  {
    if (tilt_ia == 1U)
    {
      status->TiltStatus = 1;
    }
  }

  return 0;
}

/**
 * @brief  Set self test
 * @param  val the value of st_xl in reg CTRL5_C
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_x_self_test(uint8_t val)
{
  lsm6dsox_st_xl_t reg;

  reg = (val == 0U)  ? LSM6DSOX_XL_ST_DISABLE
      : (val == 1U)  ? LSM6DSOX_XL_ST_POSITIVE
      : (val == 2U)  ? LSM6DSOX_XL_ST_NEGATIVE
      :                LSM6DSOX_XL_ST_DISABLE;

  if (lsm6dsox_xl_self_test_set(&_reg_ctx, reg) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX GYRO data ready bit value
 * @param  status the status of data ready bit
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_g_drdy_status(uint8_t *status)
{
  if (lsm6dsox_gy_flag_data_ready_get(&_reg_ctx, status) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set self test
 * @param  val the value of st_xl in reg CTRL5_C
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_g_self_test(uint8_t val)
{
  lsm6dsox_st_g_t reg;

  reg = (val == 0U)  ? LSM6DSOX_GY_ST_DISABLE
      : (val == 1U)  ? LSM6DSOX_GY_ST_POSITIVE
      : (val == 2U)  ? LSM6DSOX_GY_ST_NEGATIVE
      :                LSM6DSOX_GY_ST_DISABLE;


  if (lsm6dsox_gy_self_test_set(&_reg_ctx, reg) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX FIFO number of samples
 * @param  num_samples number of samples
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_num_samples(uint16_t *num_samples)
{
  if (lsm6dsox_fifo_data_level_get(&_reg_ctx, num_samples) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX FIFO full status
 * @param  status FIFO full status
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_full_status(uint8_t *status)
{
  lsm6dsox_reg_t reg;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_FIFO_STATUS2, &reg.byte, 1) != 0)
  {
    return 1;
  }

  *status = reg.fifo_status2.fifo_full_ia;

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO full interrupt on INT1 pin
 * @param  status FIFO full interrupt on INT1 pin status
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_int1_fifo_full(uint8_t status)
{
  lsm6dsox_reg_t reg;

  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_INT1_CTRL, &reg.byte, 1) != 0)
  {
    return 1;
  }

  reg.int1_ctrl.int1_fifo_full = status;

  if (lsm6dsox_write_reg(&_reg_ctx, LSM6DSOX_INT1_CTRL, &reg.byte, 1) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO watermark level
 * @param  watermark FIFO watermark level
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_watermark_level(uint16_t watermark)
{
  if (lsm6dsox_fifo_watermark_set(&_reg_ctx, watermark) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO stop on watermark
 * @param  status FIFO stop on watermark status
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_stop_on_fth(uint8_t status)
{
  if (lsm6dsox_fifo_stop_on_wtm_set(&_reg_ctx, status) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO mode
 * @param  mode FIFO mode
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_mode(uint8_t mode)
{
  int ret = 0;

  /* Verify that the passed parameter contains one of the valid values. */
  switch ((lsm6dsox_fifo_mode_t)mode)
  {
    case LSM6DSOX_BYPASS_MODE:
    case LSM6DSOX_FIFO_MODE:
    case LSM6DSOX_STREAM_TO_FIFO_MODE:
    case LSM6DSOX_BYPASS_TO_STREAM_MODE:
    case LSM6DSOX_STREAM_MODE:
      break;

    default:
      ret = 1;
      break;
  }

  if (ret == 1)
  {
    return ret;
  }

  if (lsm6dsox_fifo_mode_set(&_reg_ctx, (lsm6dsox_fifo_mode_t)mode) != 0)
  {
    return 1;
  }

  return ret;
}

/**
 * @brief  Get the LSM6DSOX FIFO tag
 * @param  tag FIFO tag
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_tag(uint8_t *tag)
{
  lsm6dsox_fifo_tag_t tag_local;

  if (lsm6dsox_fifo_sensor_tag_get(&_reg_ctx, &tag_local) != 0)
  {
    return 1;
  }

  *tag = (uint8_t)tag_local;

  return 0;
}

/**
 * @brief  Get the LSM6DSOX FIFO raw data
 * @param  data FIFO raw data array [6]
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_data(uint8_t *data)
{
  if (lsm6dsox_read_reg(&_reg_ctx, LSM6DSOX_FIFO_DATA_OUT_X_L, data, 6) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX FIFO accelero single sample (16-bit data per 3 axes) and calculate acceleration [mg]
 * @param  acceleration FIFO accelero axes [mg]
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_x_axes(int32_t *acceleration)
{
  uint8_t data[6];
  int16_t data_raw[3];
  float sensitivity = 0.0f;
  float acceleration_float[3];

  if (get_fifo_data(data) != 0)
  {
    return 1;
  }

  data_raw[0] = ((int16_t)data[1] << 8) | data[0];
  data_raw[1] = ((int16_t)data[3] << 8) | data[2];
  data_raw[2] = ((int16_t)data[5] << 8) | data[4];

  if (get_x_sensitivity(&sensitivity) != 0)
  {
    return 1;
  }

  acceleration_float[0] = (float)data_raw[0] * sensitivity;
  acceleration_float[1] = (float)data_raw[1] * sensitivity;
  acceleration_float[2] = (float)data_raw[2] * sensitivity;

  acceleration[0] = (int32_t)acceleration_float[0];
  acceleration[1] = (int32_t)acceleration_float[1];
  acceleration[2] = (int32_t)acceleration_float[2];

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO accelero BDR value
 * @param  bdr FIFO accelero BDR value
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_x_bdr(float bdr)
{
  lsm6dsox_bdr_xl_t new_bdr;

  new_bdr = (bdr <=    0.0f) ? LSM6DSOX_XL_NOT_BATCHED
          : (bdr <=   12.5f) ? LSM6DSOX_XL_BATCHED_AT_12Hz5
          : (bdr <=   26.0f) ? LSM6DSOX_XL_BATCHED_AT_26Hz
          : (bdr <=   52.0f) ? LSM6DSOX_XL_BATCHED_AT_52Hz
          : (bdr <=  104.0f) ? LSM6DSOX_XL_BATCHED_AT_104Hz
          : (bdr <=  208.0f) ? LSM6DSOX_XL_BATCHED_AT_208Hz
          : (bdr <=  416.0f) ? LSM6DSOX_XL_BATCHED_AT_417Hz
          : (bdr <=  833.0f) ? LSM6DSOX_XL_BATCHED_AT_833Hz
          : (bdr <= 1660.0f) ? LSM6DSOX_XL_BATCHED_AT_1667Hz
          : (bdr <= 3330.0f) ? LSM6DSOX_XL_BATCHED_AT_3333Hz
          :                    LSM6DSOX_XL_BATCHED_AT_6667Hz;

  if (lsm6dsox_fifo_xl_batch_set(&_reg_ctx, new_bdr) != 0)
  {
    return 1;
  }

  return 0;
}

/**
 * @brief  Get the LSM6DSOX FIFO gyro single sample (16-bit data per 3 axes) and calculate angular velocity [mDPS]
 * @param  angular_velocity FIFO gyro axes [mDPS]
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::get_fifo_g_axes(int32_t *angular_velocity)
{
  uint8_t data[6];
  int16_t data_raw[3];
  float sensitivity = 0.0f;
  float angular_velocity_float[3];

  if (get_fifo_data(data) != 0)
  {
    return 1;
  }

  data_raw[0] = ((int16_t)data[1] << 8) | data[0];
  data_raw[1] = ((int16_t)data[3] << 8) | data[2];
  data_raw[2] = ((int16_t)data[5] << 8) | data[4];

  if (get_g_sensitivity(&sensitivity) != 0)
  {
    return 1;
  }

  angular_velocity_float[0] = (float)data_raw[0] * sensitivity;
  angular_velocity_float[1] = (float)data_raw[1] * sensitivity;
  angular_velocity_float[2] = (float)data_raw[2] * sensitivity;

  angular_velocity[0] = (int32_t)angular_velocity_float[0];
  angular_velocity[1] = (int32_t)angular_velocity_float[1];
  angular_velocity[2] = (int32_t)angular_velocity_float[2];

  return 0;
}

/**
 * @brief  Set the LSM6DSOX FIFO gyro BDR value
 * @param  bdr FIFO gyro BDR value
 * @retval 0 in case of success, an error code otherwise
 */
int LSM6DSOXSensor::set_fifo_g_bdr(float bdr)
{
  lsm6dsox_bdr_gy_t new_bdr;

  new_bdr = (bdr <=    0.0f) ? LSM6DSOX_GY_NOT_BATCHED
          : (bdr <=   12.5f) ? LSM6DSOX_GY_BATCHED_AT_12Hz5
          : (bdr <=   26.0f) ? LSM6DSOX_GY_BATCHED_AT_26Hz
          : (bdr <=   52.0f) ? LSM6DSOX_GY_BATCHED_AT_52Hz
          : (bdr <=  104.0f) ? LSM6DSOX_GY_BATCHED_AT_104Hz
          : (bdr <=  208.0f) ? LSM6DSOX_GY_BATCHED_AT_208Hz
          : (bdr <=  416.0f) ? LSM6DSOX_GY_BATCHED_AT_417Hz
          : (bdr <=  833.0f) ? LSM6DSOX_GY_BATCHED_AT_833Hz
          : (bdr <= 1660.0f) ? LSM6DSOX_GY_BATCHED_AT_1667Hz
          : (bdr <= 3330.0f) ? LSM6DSOX_GY_BATCHED_AT_3333Hz
          :                    LSM6DSOX_GY_BATCHED_AT_6667Hz;

  if (lsm6dsox_fifo_gy_batch_set(&_reg_ctx, new_bdr) != 0)
  {
    return 1;
  }

  return 0;
}



int32_t LSM6DSOX_io_write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite)
{
  return ((LSM6DSOXSensor *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

int32_t LSM6DSOX_io_read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead)
{
  return ((LSM6DSOXSensor *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}
