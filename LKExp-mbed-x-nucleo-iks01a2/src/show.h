#ifndef _SHOW_H_
#define _SHOW_H_

# define LCD_DISPLAY 1

#include "mbed.h"
#include "XNucleoIKS01A2_Extension.h"
#if LCD_DISPLAY
#include "stm32f769i_discovery_lcd.h"
#endif

namespace show {
    void lcd(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSLSensor*, LSM303AGRAccSensor*);
    void lcd(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSOXSensor*, LSM303AGRAccSensor*);
    void terminal(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSLSensor*, LSM303AGRAccSensor*);
    void terminal(LSM303AGRMagSensor*, HTS221Sensor*, LPS22HBSensor*, LSM6DSOXSensor*, LSM303AGRAccSensor*);
}
extern Mutex stdio_mutex;

#endif /* _SHOW_H_ */
