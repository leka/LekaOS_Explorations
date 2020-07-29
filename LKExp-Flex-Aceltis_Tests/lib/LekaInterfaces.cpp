#include "LekaInterfaces.h"

// BufferedSerial rfid(PIN_RFID_TX, PIN_RFID_RX, 57600);

I2C i2c3(PIN_I2C3_SDA, PIN_I2C3_SCL);

SPI leds_ears(PIN_SPI2_MOSI, NC, PIN_SPI2_CLK);
SPI leds_belt(PIN_SPI6_MOSI, NC, PIN_SPI6_CLK);

DigitalOut mux_reset(PIN_MUX_RST); //SET TO 1 if necessary
DigitalIn mux_inta(PIN_MUX_INTA);
DigitalIn mux_intb(PIN_MUX_INTB);
