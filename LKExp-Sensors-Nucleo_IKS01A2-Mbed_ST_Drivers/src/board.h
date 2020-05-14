/* Pin definition ------------------------------------------------------------*/
#define PIN_I2C_SDA                  (D14)
#define PIN_I2C_SCL                  (D15)
#define PIN_LSM6DSOX_INT1            (A5)

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "HTS221Sensor.h"
#include "LSM303AGRMagSensor.h"
#include "LSM6DSOXSensor.h"
#include "DevI2C.h"

/* Static variables ----------------------------------------------------------*/
static DigitalOut _errorLED(LED1);

class Board {
public:
    Board(PinName sda=PIN_I2C_SDA, PinName scl=PIN_I2C_SCL);
    
    void check_id();

    DevI2C dev_i2c;
    HTS221Sensor ht_sensor;
    LSM303AGRMagSensor magneto;
    LSM6DSOXSensor acc_gyro;
};
