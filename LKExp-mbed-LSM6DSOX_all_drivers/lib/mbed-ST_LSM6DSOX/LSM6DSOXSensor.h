/**
 ******************************************************************************
 * @file    LSM6DSOXSensor.h
 * @author  SRA
 * @version V1.0.0
 * @date    February 2019
 * @brief   Abstract Class of an LSM6DSOX Inertial Measurement Unit (IMU) 6 axes
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


/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __LSM6DSOXSensor_H__
#define __LSM6DSOXSensor_H__


/* Includes ------------------------------------------------------------------*/
#include "DevI2C.h"
#include "lsm6dsox_reg.h"
#include "MotionSensor.h"
#include "GyroSensor.h"
#include <assert.h>

/* Defines -------------------------------------------------------------------*/

#define LSM6DSOX_ACC_SENSITIVITY_FS_2G   0.061f
#define LSM6DSOX_ACC_SENSITIVITY_FS_4G   0.122f
#define LSM6DSOX_ACC_SENSITIVITY_FS_8G   0.244f
#define LSM6DSOX_ACC_SENSITIVITY_FS_16G  0.488f

#define LSM6DSOX_GYRO_SENSITIVITY_FS_125DPS    4.375f
#define LSM6DSOX_GYRO_SENSITIVITY_FS_250DPS    8.750f
#define LSM6DSOX_GYRO_SENSITIVITY_FS_500DPS   17.500f
#define LSM6DSOX_GYRO_SENSITIVITY_FS_1000DPS  35.000f
#define LSM6DSOX_GYRO_SENSITIVITY_FS_2000DPS  70.000f


/* Typedefs ------------------------------------------------------------------*/

typedef enum
{
  LSM6DSOX_INT1_PIN,
  LSM6DSOX_INT2_PIN,
} LSM6DSOX_Interrupt_Pin_t;

typedef struct
{
  unsigned int FreeFallStatus : 1;
  unsigned int TapStatus : 1;
  unsigned int DoubleTapStatus : 1;
  unsigned int WakeUpStatus : 1;
  unsigned int StepStatus : 1;
  unsigned int TiltStatus : 1;
  unsigned int D6DOrientationStatus : 1;
  unsigned int SleepStatus : 1;
} LSM6DSOX_Event_Status_t;


/* Class Declaration ---------------------------------------------------------*/
   
/**
 * Abstract class of an LSM6DSOX Inertial Measurement Unit (IMU) 6 axes
 * sensor.
 */
class LSM6DSOXSensor : public MotionSensor, public GyroSensor
{
  public:
    enum SPI_type_t {SPI3W, SPI4W};
    LSM6DSOXSensor(SPI *spi, PinName cs_pin, PinName int1_pin=NC, PinName int2_pin=NC, SPI_type_t spi_type=SPI4W);
    LSM6DSOXSensor(DevI2C *i2c, uint8_t address=LSM6DSOX_I2C_ADD_H, PinName int1_pin=NC, PinName int2_pin=NC);
    virtual int init(void *init);
    virtual int read_id(uint8_t *id);
    virtual int get_x_axes(int32_t *acceleration);
    virtual int get_g_axes(int32_t *angular_rate);
    virtual int get_x_sensitivity(float *sensitivity);
    virtual int get_g_sensitivity(float *sensitivity);
    virtual int get_x_axes_raw(int16_t *value);
    virtual int get_g_axes_raw(int16_t *value);
    virtual int get_x_odr(float *odr);
    virtual int get_g_odr(float *odr);
    virtual int set_x_odr(float odr);
    virtual int set_g_odr(float odr);
    virtual int get_x_fs(float *full_scale);
    virtual int get_g_fs(float *full_scale);
    virtual int set_x_fs(float full_scale);
    virtual int set_g_fs(float full_scale);
    int enable_x(void);
    int enable_g(void);
    int disable_x(void);
    int disable_g(void);
    int enable_free_fall_detection(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT1_PIN);
    int disable_free_fall_detection(void);
    int set_free_fall_threshold(uint8_t thr);
    int set_free_fall_duration(uint8_t dur);
    int enable_pedometer(void);
    int disable_pedometer(void);
    int get_step_counter(uint16_t *step_count);
    int reset_step_counter(void);
    int enable_tilt_detection(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT1_PIN);
    int disable_tilt_detection(void);
    int enable_wake_up_detection(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT2_PIN);
    int disable_wake_up_detection(void);
    int set_wake_up_threshold(uint8_t thr);
    int set_wake_up_duration(uint8_t dur);
    int enable_single_tap_detection(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT1_PIN);
    int disable_single_tap_detection(void);
    int enable_double_tap_detection(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT1_PIN);
    int disable_double_tap_detection(void);
    int set_tap_threshold(uint8_t thr);
    int set_tap_shock_time(uint8_t time);
    int set_tap_quiet_time(uint8_t time);
    int set_tap_duration_time(uint8_t time);
    int enable_6d_orientation(LSM6DSOX_Interrupt_Pin_t pin = LSM6DSOX_INT1_PIN);
    int disable_6d_orientation(void);
    int set_6d_orientation_threshold(uint8_t thr);
    int get_6d_orientation_xl(uint8_t *xl);
    int get_6d_orientation_xh(uint8_t *xh);
    int get_6d_orientation_yl(uint8_t *yl);
    int get_6d_orientation_yh(uint8_t *yh);
    int get_6d_orientation_zl(uint8_t *zl);
    int get_6d_orientation_zh(uint8_t *zh);
    int get_event_status(LSM6DSOX_Event_Status_t *status);
    int read_reg(uint8_t reg, uint8_t *data);
    int write_reg(uint8_t reg, uint8_t data);
    int set_interrupt_latch(uint8_t status);
    int get_x_drdy_status(uint8_t *status);
    int set_x_self_test(uint8_t status);
    int get_g_drdy_status(uint8_t *status);
    int set_g_self_test(uint8_t status);
    int get_fifo_num_samples(uint16_t *num_samples);
    int get_fifo_full_status(uint8_t *status);
    int set_fifo_int1_fifo_full(uint8_t status);
    int set_fifo_watermark_level(uint16_t watermark);
    int set_fifo_stop_on_fth(uint8_t status);
    int set_fifo_mode(uint8_t mode);
    int get_fifo_tag(uint8_t *tag);
    int get_fifo_data(uint8_t *data);
    int get_fifo_x_axes(int32_t *acceleration);
    int set_fifo_x_bdr(float bdr);
    int get_fifo_g_axes(int32_t *angular_velocity);
    int set_fifo_g_bdr(float bdr);
    
    /**
     * @brief  Attaching an interrupt handler to the INT1 interrupt.
     * @param  fptr An interrupt handler.
     * @retval None.
     */
    void attach_int1_irq(void (*fptr)(void))
    {
        _int1_irq.rise(fptr);
    }

    /**
     * @brief  Enabling the INT1 interrupt handling.
     * @param  None.
     * @retval None.
     */
    void enable_int1_irq(void)
    {
        _int1_irq.enable_irq();
    }
    
    /**
     * @brief  Disabling the INT1 interrupt handling.
     * @param  None.
     * @retval None.
     */
    void disable_int1_irq(void)
    {
        _int1_irq.disable_irq();
    }
    
    /**
     * @brief  Attaching an interrupt handler to the INT2 interrupt.
     * @param  fptr An interrupt handler.
     * @retval None.
     */
    void attach_int2_irq(void (*fptr)(void))
    {
        _int2_irq.rise(fptr);
    }

    /**
     * @brief  Enabling the INT2 interrupt handling.
     * @param  None.
     * @retval None.
     */
    void enable_int2_irq(void)
    {
        _int2_irq.enable_irq();
    }
    
    /**
     * @brief  Disabling the INT2 interrupt handling.
     * @param  None.
     * @retval None.
     */
    void disable_int2_irq(void)
    {
        _int2_irq.disable_irq();
    }
    
    /**
     * @brief Utility function to read data.
     * @param  pBuffer: pointer to data to be read.
     * @param  RegisterAddr: specifies internal address register to be read.
     * @param  NumByteToRead: number of bytes to be read.
     * @retval 0 if ok, an error code otherwise.
     */
    uint8_t io_read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
    {        
        if (_dev_spi) {
            /* Write Reg Address */
            _dev_spi->lock();
            _cs_pin = 0;           
            if (_spi_type == SPI4W) {            
                _dev_spi->write(RegisterAddr | 0x80);
                for (int i=0; i<NumByteToRead; i++) {
                    *(pBuffer+i) = _dev_spi->write(0x00);
                }
            } else if (_spi_type == SPI3W){
                /* Write RD Reg Address with RD bit*/
                uint8_t TxByte = RegisterAddr | 0x80;    
                _dev_spi->write((char *)&TxByte, 1, (char *)pBuffer, (int) NumByteToRead);
            }            
            _cs_pin = 1;
            _dev_spi->unlock(); 
            return 0;
        }                       
        if (_dev_i2c) return (uint8_t) _dev_i2c->i2c_read(pBuffer, _address, RegisterAddr, NumByteToRead);
        return 1;
    }
    
    /**
     * @brief Utility function to write data.
     * @param  pBuffer: pointer to data to be written.
     * @param  RegisterAddr: specifies internal address register to be written.
     * @param  NumByteToWrite: number of bytes to write.
     * @retval 0 if ok, an error code otherwise.
     */
    uint8_t io_write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
    {
        if (_dev_spi) { 
            _dev_spi->lock();
            _cs_pin = 0;
            _dev_spi->write(RegisterAddr);                    
            _dev_spi->write((char *)pBuffer, (int) NumByteToWrite, NULL, 0);                     
            _cs_pin = 1;                    
            _dev_spi->unlock();
            return 0;                    
        }        
        if (_dev_i2c) return (uint8_t) _dev_i2c->i2c_write(pBuffer, _address, RegisterAddr, NumByteToWrite);    
        return 1;
    }

  private:
    int set_x_odr_when_enabled(float odr);
    int set_g_odr_when_enabled(float odr);
    int set_x_odr_when_disabled(float odr);
    int set_g_odr_when_disabled(float odr);

    /* Helper classes. */
    DevI2C *_dev_i2c;
    SPI    *_dev_spi;

    /* Configuration */
    uint8_t _address;
    DigitalOut  _cs_pin;        
    InterruptIn _int1_irq;
    InterruptIn _int2_irq;
    SPI_type_t _spi_type;
    
    uint8_t _x_is_enabled;
    lsm6dsox_odr_xl_t _x_last_odr;
    uint8_t _g_is_enabled;
    lsm6dsox_odr_g_t _g_last_odr;

    lsm6dsox_ctx_t _reg_ctx;
};

#ifdef __cplusplus
 extern "C" {
#endif
int32_t LSM6DSOX_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
int32_t LSM6DSOX_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );
#ifdef __cplusplus
  }
#endif

#endif
