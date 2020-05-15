#include "mbed.h"
#include "accelerometer.h"
#include "gyroscope.h"

#define PIN_I2C_SDA                 (D14)
#define PIN_I2C_SCL                 (D15)
#define LSM6DSOX_ON_STEVAL          1

void testID(Accelerometer *accelerometer)
{
    uint8_t id;
    LSM6DSOXAccStatusTypeDef success;

    success = accelerometer->read_id(&id);

    if (success == LSM6DSOX_Acc_OK && id == LSM6DSOX_ID) {
        printf("Test ID (accelerometer)... OK!\r\n");
        printf("ID is %X\r\n\n", id);
    } else {
        printf("Test ID (accelerometer)... Failed.\r\n\n");
    }
    return;
}

void testID(Gyroscope *gyroscope)
{
    uint8_t id;
    LSM6DSOXGyroStatusTypeDef success;

    success = gyroscope->read_id(&id);

    if (success == LSM6DSOX_Gyro_OK && id == LSM6DSOX_ID) {
        printf("Test ID (gyroscope)... OK!\r\n");
        printf("ID is %X\r\n\n", id);
    } else {
        printf("Test ID (gyroscope)... Failed.\r\n\n");
    }
    return;
}

int main(void) {
    printf("\nStarting a new run!\r\n\n");


    /* Initialization */
    I2C i2c1(PIN_I2C_SDA, PIN_I2C_SCL);

    Accelerometer accelerometer(&i2c1);
    Gyroscope gyroscope(&i2c1);
#if LSM6DSOX_ON_STEVAL
    DigitalOut INT_1_LSM6DSOX (PF_9, 0); //INT1 of LSM6DSOX cannot be used. This line fix the use of LSM6DSOX on X-NUCLEO_IKS01A2
#endif

    if(accelerometer.init(NULL)==LSM6DSOX_Acc_ERROR) {
        printf("LSM6DSOX (accelerometer) init... Failed.\r\n\n");
    }
    if(gyroscope.init(NULL)==LSM6DSOX_Gyro_ERROR) {
        printf("LSM6DSOX (gyroscope) init... Failed.\r\n\n");
    }

    /* Tests configuration */
    testID(&accelerometer);
    testID(&gyroscope);


    /* Tests features */

	return 0;
}
