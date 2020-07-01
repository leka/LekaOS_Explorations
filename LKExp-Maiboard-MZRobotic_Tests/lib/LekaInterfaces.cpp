#include "LekaInterfaces.h"
/*
QSPI qspi1(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_1);	  //+ Mode polarity at 0
QSPI qspi2(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_2);	  //+ Mode polarity at 0
QSPI qspi3(PIN_Q_SPI_IO0, PIN_Q_SPI_IO1, PIN_Q_SPI_IO2, PIN_Q_SPI_IO3, PIN_Q_SPI_CLK,
		   PIN_Q_SPI_CE_3);	  //+ Mode polarity at 0
*/
SDBlockDevice sd(PIN_SD_SPI_MOSI, PIN_SD_SPI_MISO, PIN_SD_SPI_SCK, PIN_SD_SPI_EN);

I2C i2c1(PIN_I2C1_SDA, PIN_I2C1_SCL);
DigitalIn int_imu(PIN_INT1_IMU);
DigitalIn int_mag(PIN_DTRD_MAG);
DigitalIn int_temp(PIN_DRDY_TEMP);

AnalogIn microphone(PIN_MICRO_MCU);

AnalogIn brightness(PIN_V_LUMINOSITE);
/*
PwmOut motor_left(PIN_ENA);
PwmOut motor_right(PIN_ENB);
*/
SPI spi5(PIN_SPI5_MOSI, PIN_SPI5_MISO, PIN_SPI5_SCK, PIN_SPI5_NSS);
DigitalOut ble_reset(PIN_RST_BLE);
//DigitalIn ble_interrupt(PIN_IRQ_BLE);

ESP8266Interface wifi(PIN_USART2_ESP_TX, PIN_USART2_ESP_RX);
// BufferedSerial uart2(PIN_USART_ESP_TX, PIN_USART2_ESP_RX);
// DigitalOut wifi_reset(PIN_RST_ESP);
// DigitalOut wifi_enable(PIN_ESP_ENABLE);
/*
Serial uart6(PIN_USART6_BTTX, PIN_USART6_BTRX);
DigitalOut bluetooth_reset(PIN_RST_BM64);
DigitalOut bluetooth_wakeup(PIN_BT_WAKEUP);

DigitalOut sound_off(PIN_SON_OFF);
AnalogOut sound_out(PIN_MCU_SON_OUT);
*/