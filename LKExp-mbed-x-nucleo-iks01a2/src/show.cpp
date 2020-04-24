#include "show.h"

/* Helper function for printing floats & doubles */
static char *print_double(char* str, double v, int decimalDigits=2)
{
  int i = 1;
  int intPart, fractPart;
  int len;
  char *ptr;
 
  /* prepare decimal digits multiplicator */
  for (;decimalDigits!=0; i*=10, decimalDigits--);
 
  /* calculate integer & fractinal parts */
  intPart = (int)v;
  fractPart = (int)((v-(double)(int)v)*i);
 
  /* fill in integer part */
  sprintf(str, "%i.", intPart);
 
  /* prepare fill in of fractional part */
  len = strlen(str);
  ptr = &str[len];
 
  /* fill in leading fractional zeros */
  for (i/=10;i>1; i/=10, ptr++) {
    if (fractPart >= i) {
      break;
    }
    *ptr = '0';
  }
 
  /* fill in (rest of) fractional part */
  sprintf(ptr, "%i", fractPart);
 
  return str;
}

namespace show {
    void lcd(LSM303AGRMagSensor* magnetometer, HTS221Sensor* hum_temp, LPS22HBSensor* press_temp, LSM6DSLSensor* acc_gyro, LSM303AGRAccSensor* accelerometer) {
        float value1, value2;
        char buffer1[32], buffer2[32];
        int32_t axes[3];
        uint8_t lcd_string[100];
        
        hum_temp->get_temperature(&value1);
        hum_temp->get_humidity(&value2);
        sprintf((char*)lcd_string,"HTS221: [temp] %7s C,   [hum] %s%%               ", print_double(buffer1, value1), print_double(buffer2, value2));
        BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)lcd_string, LEFT_MODE);

        press_temp->get_temperature(&value1);
        press_temp->get_pressure(&value2);
        sprintf((char*)lcd_string,"LPS22HB: [temp] %7s C, [press] %s mbar               ", print_double(buffer1, value1), print_double(buffer2, value2));
        BSP_LCD_DisplayStringAt(0, 150, (uint8_t*)lcd_string, LEFT_MODE);
        
        magnetometer->get_m_axes(axes);
        sprintf((char*)lcd_string,"LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld               ", axes[0], axes[1], axes[2]);
        BSP_LCD_DisplayStringAt(0, 200, (uint8_t*)lcd_string, LEFT_MODE);

        accelerometer->get_x_axes(axes);
        sprintf((char*)lcd_string,"LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld               ", axes[0], axes[1], axes[2]);
        BSP_LCD_DisplayStringAt(0, 250, (uint8_t*)lcd_string, LEFT_MODE);

        acc_gyro->get_x_axes(axes);
        sprintf((char*)lcd_string,"LSM6DSL [acc/mg]: %6ld, %6ld, %6ld               ", axes[0], axes[1], axes[2]);
        BSP_LCD_DisplayStringAt(0, 300, (uint8_t*)lcd_string, LEFT_MODE);

        acc_gyro->get_g_axes(axes);
        sprintf((char*)lcd_string,"LSM6DSL [gyro/mdps]: %6ld, %6ld, %6ld               ", axes[0], axes[1], axes[2]);
        BSP_LCD_DisplayStringAt(0, 350, (uint8_t*)lcd_string, LEFT_MODE);
    }

    void terminal(LSM303AGRMagSensor* magnetometer, HTS221Sensor* hum_temp, LPS22HBSensor* press_temp, LSM6DSLSensor* acc_gyro, LSM303AGRAccSensor* accelerometer, Mutex stdio_mutex) {
        float value1, value2;
        char buffer1[32], buffer2[32];
        int32_t axes[3];
        
        stdio_mutex.lock();
        printf("\r\n");
        
        hum_temp->get_temperature(&value1);
        hum_temp->get_humidity(&value2);
        printf("HTS221: [temp] %7s C,   [hum] %s%%\r\n", print_double(buffer1, value1), print_double(buffer2, value2));

        press_temp->get_temperature(&value1);
        press_temp->get_pressure(&value2);
        printf("LPS22HB: [temp] %7s C, [press] %s mbar\r\n", print_double(buffer1, value1), print_double(buffer2, value2));
        printf("---\r\n");
        
        magnetometer->get_m_axes(axes);
        printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

        accelerometer->get_x_axes(axes);
        printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

        acc_gyro->get_x_axes(axes);
        printf("LSM6DSL [acc/mg]:      %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

        acc_gyro->get_g_axes(axes);
        printf("LSM6DSL [gyro/mdps]:   %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

        printf("---\r\n");
        stdio_mutex.unlock();
    }
}
