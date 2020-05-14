#include "board.h"

Board::Board(PinName sda, PinName scl) :
    dev_i2c(sda, scl),
    ht_sensor(&dev_i2c),
    magneto(&dev_i2c),
    acc_gyro(&dev_i2c, LSM6DSOX_I2C_ADD_L)
{
    ht_sensor.init(NULL);
    magneto.init(NULL);
    acc_gyro.init(NULL);
    
    DigitalOut LSM6DSOX_INT1 (PIN_LSM6DSOX_INT1, 0); //Set to 1 to disable I2C and enable only I3C on LSM6DSOX
}

void Board::check_id() {
    uint8_t id;
    
    ht_sensor.read_id(&id);
    if (id != HTS221_WHO_AM_I_VAL) _errorLED = 1; //(0xBC expected)
    magneto.read_id(&id);
    if (id != LSM303AGR_MAG_WHO_AM_I) _errorLED = 1; //(0x40 expected)
    acc_gyro.read_id(&id);
    if (id != LSM6DSOX_ID) _errorLED = 1; //(0x6C expected)
}
