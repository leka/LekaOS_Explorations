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

void testAllStatus(Accelerometer *accelerometer)
{
    PowerModeAcc powerMode;
    float dataRate;
    uint16_t fullScale;
    LSM6DSOXAccStatusTypeDef success;

    success = accelerometer->get_status(&powerMode, &dataRate, &fullScale);

    if (success == LSM6DSOX_Acc_OK) {
        printf("Test Power mode, Output Data Rate and Full Scale (accelerometer)... OK!\r\n");
        switch(powerMode)
        {
            case High_Performance_Acc:
                printf("Power mode is \"High performance mode\"\r\n");
                break;
            case Normal_Power_Acc:
                printf("Power mode is \"Normal mode\"\r\n");
                break;
            case Low_Power_Acc:
                printf("Power mode is \"Low power mode\"\r\n");
                break;
            case Ultra_Low_Power_Acc:
                printf("Power mode is \"Ultra low power mode\"\r\n");
                break;
            case Power_Off_Acc:
                printf("Accelerometer is turned off\r\n");
                break;
        }
        printf("Output Data Rate is (approximately) %dHz\r\n", (int)dataRate); //printf does not handle float, dataRate was reduced to int
        printf("Full Scale is +/-%dg\r\n", fullScale);
    } else {
        printf("Test Power mode, Output Data Rate and Full Scale (accelerometer)... Failed.\r\n");
    }


    uint8_t dataReady;

    success = accelerometer->get_int_status(&dataReady);
    if (success == LSM6DSOX_Acc_OK) {
        printf("Interrupt Data Ready (accelerometer)... OK!\r\n");
        if (dataReady)
        {
            printf("Data Ready of Accelerometer is %X, new set of data available\r\n\n", dataReady);
        } else {
            printf("Data Ready of Accelerometer is %X, no set of data available\r\n\n", dataReady);
        }
    } else {
        printf("Interrupt Data Ready... Failed.\r\n\n");
    }

    return;
}

void testAllStatus(Gyroscope *gyroscope)
{
    PowerModeGyro powerMode;
    float dataRate;
    uint16_t fullScale;
    LSM6DSOXGyroStatusTypeDef success;

    success = gyroscope->get_status(&powerMode, &dataRate, &fullScale);

    if (success == LSM6DSOX_Gyro_OK) {
        printf("Test Power mode, Output Data Rate and Full Scale (gyroscope)... OK!\r\n");
        switch(powerMode)
        {
            case High_Performance_Gyro:
                printf("Power mode is \"High performance mode\"\r\n");
                break;
            case Normal_Power_Gyro:
                printf("Power mode is \"Normal mode\"\r\n");
                break;
            case Low_Power_Gyro:
                printf("Power mode is \"Low power mode\"\r\n");
                break;
//            case Ultra_Low_Power_Gyro:
//                printf("Power mode is \"Ultra low power mode\"\r\n");
//                break;
            case Power_Off_Gyro:
                printf("Gyroscope is turned off\r\n");
                break;
        }
        printf("Output Data Rate is (approximately) %dHz\r\n", (int)dataRate); //printf does not handle float, dataRate was reduced to int
        printf("Full Scale is +/-%ddps\r\n", fullScale);
    } else {
        printf("Test Power mode, Output Data Rate and Full Scale (gyroscope)... Failed.\r\n");
    }


    uint8_t dataReady;

    success = gyroscope->get_int_status(&dataReady);
    if (success == LSM6DSOX_Gyro_OK) {
        printf("Interrupt Data Ready (gyroscope)... OK!\r\n");
        if (dataReady)
        {
            printf("Data Ready of Gyroscope is %X, new set of data available\r\n\n", dataReady);
        } else {
            printf("Data Ready of Gyroscope is %X, no set of data available\r\n\n", dataReady);
        }
    } else {
        printf("Interrupt Data Ready... Failed.\r\n\n");
    }

    return;
}

void testData(Accelerometer *accelerometer)
{
    float mg_X;
    float mg_Y;
    float mg_Z;
    LSM6DSOXAccStatusTypeDef success;

    success = accelerometer->get_data(&mg_X,&mg_Y,&mg_Z);

    if (success == LSM6DSOX_Acc_OK) {
        printf("Test Data (accelerometer)... OK!\r\n");
        printf("Values are X:%dmg, Y:%dmg, Z:%dmg\r\n\n", (int)mg_X, (int)mg_Y, (int)mg_Z);
    } else {
        printf("Test Data (accelerometer)... Failed.\r\n\n");
    }
    return;
}

void testData(Gyroscope *gyroscope)
{
    float dps_X;
    float dps_Y;
    float dps_Z;
    LSM6DSOXGyroStatusTypeDef success;

    success = gyroscope->get_data(&dps_X,&dps_Y,&dps_Z);

    if (success == LSM6DSOX_Acc_OK) {
        printf("Test Data (gyroscope)... OK!\r\n");
        printf("Values are X:%dmdps, Y:%dmdps, Z:%dmdps\r\n\n", (int)dps_X, (int)dps_Y, (int)dps_Z);
    } else {
        printf("Test Data (gyroscope)... Failed.\r\n\n");
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

    testAllStatus(&accelerometer);
    testAllStatus(&gyroscope);

    testData(&accelerometer);
    testData(&gyroscope);

    /* Tests features */

	return 0;
}
