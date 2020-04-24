# define LCD_DISPLAY
 
/* Includes */
#include "mbed.h"
#include "XNucleoIKS01A2.h"
#ifdef LCD_DISPLAY
#include "stm32f769i_discovery_lcd.h"
#endif
 
/* Instantiate the expansion board */
static XNucleoIKS01A2 *mems_expansion_board = XNucleoIKS01A2::instance(D14, D15, D4, D5);
 

/* Retrieve the composing elements of the expansion board */
static LSM303AGRMagSensor *magnetometer = mems_expansion_board->magnetometer;
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;
static LSM6DSLSensor *acc_gyro = mems_expansion_board->acc_gyro;
static LSM303AGRAccSensor *accelerometer = mems_expansion_board->accelerometer;

DigitalOut myled(LED1);
Thread eventThread;
EventQueue queue(1 * EVENTS_EVENT_SIZE);
Mutex stdio_mutex;

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

void shakeDisplay() {
    LSM6DSL_Event_Status_t status;
    acc_gyro->get_event_status(&status);
    if (status.WakeUpStatus) {
        /* Led blinking. */
        myled = 1;
  
        /* Output data. */
#ifdef LCD_DISPLAY
        BSP_LCD_DisplayStringAt(0, 450, (uint8_t*)"Wake Up Detected!", CENTER_MODE);
#else
        stdio_mutex.lock();
        printf("Wake Up Detected!\r\n");
        stdio_mutex.unlock();
#endif
        wait_us(3000000);
//        ThisThread::sleep_for(3000);
        myled = 0;
#ifdef LCD_DISPLAY
        BSP_LCD_DisplayStringAt(0, 450, (uint8_t*)"                         ", CENTER_MODE);
#endif
    }
}

void onDataReceived() {
    myled = 1;
    queue.call(&shakeDisplay);
}

void LCD_show();
void terminal_show();

/* Simple main function */
int main() {
  eventThread.start(callback(&queue, &EventQueue::dispatch_forever));
  
  /* Attach callback to LSM6DSL INT2 */
  acc_gyro->attach_int2_irq(&onDataReceived);
    
  /* Enable all sensors */
  hum_temp->enable();
  press_temp->enable();
  magnetometer->enable();
  accelerometer->enable();
  acc_gyro->enable_x();
  acc_gyro->enable_g();
  /* Enable Wake-Up Detection. */
  acc_gyro->enable_wake_up_detection();
  
  printf("\r\n--- Starting new run ---\r\n");
 
  uint8_t id;
  hum_temp->read_id(&id);
  printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
  press_temp->read_id(&id);
  printf("LPS22HB  pressure & temperature   = 0x%X\r\n", id);
  magnetometer->read_id(&id);
  printf("LSM303AGR magnetometer            = 0x%X\r\n", id);
  accelerometer->read_id(&id);
  printf("LSM303AGR accelerometer           = 0x%X\r\n", id);
  acc_gyro->read_id(&id);
  printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);

#ifdef LCD_DISPLAY
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)"IMU example", CENTER_MODE);
  BSP_LCD_SetFont(&Font20);
#endif

#ifdef LCD_DISPLAY
    while(1) {
        LCD_show();
        wait_us(100000);
    }
#else
    while(1) {
        terminal_show();
        ThisThread::sleep_for(100);
    }
#endif
}

void LCD_show() {
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

void terminal_show() {
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
