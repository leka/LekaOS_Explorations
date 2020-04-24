#ifndef _SHOW_H_
#define _SHOW_H_

#include "mbed.h"
#include "XNucleoIKS01A2.h"
#include "stm32f769i_discovery_lcd.h"

namespace show {
    void lcd(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSLSensor*, LSM303AGRAccSensor*);
    void terminal(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSLSensor*, LSM303AGRAccSensor*);
}
extern Mutex stdio_mutex;

#endif /* _SHOW_H_ */
