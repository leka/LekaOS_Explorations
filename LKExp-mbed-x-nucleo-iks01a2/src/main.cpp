# define LCD_DISPLAY 0
# define LSM6DSOX_USE 0
 
/* Includes */
#include "mbed.h"
#include "show.h"
#if LSM6DSOX_USE
#include "XNucleoIKS01A2_Extension.h"
#else
#include "XNucleoIKS01A2.h"
#endif
#if LCD_DISPLAY
#include "stm32f769i_discovery_lcd.h"
#endif
 
/* Instantiate the expansion board */
#if LSM6DSOX_USE
static XNucleoIKS01A2_Extension *mems_expansion_board = XNucleoIKS01A2_Extension::instance(D14, D15, D4, D5, A5, A4);
#else
static XNucleoIKS01A2 *mems_expansion_board = XNucleoIKS01A2::instance(D14, D15, D4, D5);
#endif
 

/* Retrieve the composing elements of the expansion board */
static LSM303AGRMagSensor *magnetometer = mems_expansion_board->magnetometer;
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;
static LSM6DSLSensor *acc_gyro = mems_expansion_board->acc_gyro;
static LSM303AGRAccSensor *accelerometer = mems_expansion_board->accelerometer;
#if LSM6DSOX_USE
static LSM6DSOXSensor *acc_gyro_ext = mems_expansion_board->acc_gyro_ext;
#endif

DigitalOut myled(LED1);
Thread eventThread;
EventQueue queue(1 * EVENTS_EVENT_SIZE);
extern Mutex stdio_mutex;

void shakeDisplay() {
    LSM6DSL_Event_Status_t status;
    acc_gyro->get_event_status(&status);
    if (status.WakeUpStatus) {
        /* Led blinking. */
        myled = 1;
  
        /* Output data. */
#if LCD_DISPLAY
        BSP_LCD_DisplayStringAt(0, 450, (uint8_t*)"Wake Up Detected!", CENTER_MODE);
#else
        stdio_mutex.lock();
        printf("Wake Up Detected!\r\n");
        stdio_mutex.unlock();
#endif
        wait_us(3000000);
//        ThisThread::sleep_for(3000);
        myled = 0;
#if LCD_DISPLAY
        BSP_LCD_DisplayStringAt(0, 450, (uint8_t*)"                         ", CENTER_MODE);
#endif
    }
}

void onDataReceived() {
    myled = 1;
    queue.call(&shakeDisplay);
}

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
#if LSM6DSOX_USE
  acc_gyro_ext->enable_x();
  acc_gyro_ext->enable_g();
#endif
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
#if LSM6DSOX_USE
  acc_gyro_ext->read_id(&id);
  printf("LSM6DSOX accelerometer & gyroscope = 0x%X\r\n", id);
#endif

#if LCD_DISPLAY
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)"IMU example", CENTER_MODE);
  BSP_LCD_SetFont(&Font20);
#endif

#if LCD_DISPLAY
    while(1) {
        show::lcd(magnetometer, hum_temp, press_temp, acc_gyro, accelerometer);
        wait_us(100000);
    }
#else
    while(1) {
        show::terminal(magnetometer, hum_temp, press_temp, acc_gyro, accelerometer);
        ThisThread::sleep_for(100);
    }
#endif
}
