#include "LekaBluetooth.h"

LekaBluetooth::LekaBluetooth(PinName pin_tx, PinName pin_rx, DigitalOut &reset,
                             DigitalOut &wake_up)
	: _bm64(pin_tx, pin_rx, 115200), _reset(reset), _wake_up(wake_up) {}

BufferedSerial serial(USBTX, USBRX, 115200);

void LekaBluetooth::runTest() {
	printf("\nTest of bluetooth!\n");

  _wake_up = 1;
  wait_us(25000);

  _reset = 1;
  wait_us(450000);

  bool paired = 0;

  // uint8_t buf[0x6] = {0xAA, 0x00, 0x02, 0x08, 0x00, 0xF6};
  // Enter Pairing mode : AA 00 03 02 00 50 AB
  // Ring specific tone : AA 00 03 13 02 18 D0
  // Analog/I2S and Aux/BT : AA 00 03 13 01 00/01/02/03 E6->E9 (I2S : 1x and Aux : 0x)
  uint8_t buf[0x7] = {0xAA, 0x00, 0x03, 0x02, 0x00, 0x50, 0xAB};
  uint8_t paired_buffer[0x7] = {0xAA, 0x00, 0x03, 0x02, 0x00, 0x50, 0xAB};
  uint8_t ring_tone[0x7] = {0xAA, 0x00, 0x03, 0x13, 0x02, 0x18, 0xD0};
  uint8_t tx_buffer[0x20] = {0x00};
  uint8_t rx_buffer[0x20] = {0x00};
  uint16_t tx_buffer_len = 0;
  uint16_t rx_buffer_len = 0;

  while (1) {
    if (_bm64.readable()) {
      _bm64.read(rx_buffer, 1);
      if (rx_buffer[0] == 0xAA) {
        _bm64.read(rx_buffer + 1, 2);
        rx_buffer_len = ((uint16_t)rx_buffer[1] << 8) + (uint16_t)rx_buffer[2] +
                        0x01; // Check case of rx_buffer[0] is different of 0x00
        _bm64.read(rx_buffer + 1 + 2, rx_buffer_len);

		if(rx_buffer[3] == 0x01 && rx_buffer[4] == 0x06) {paired = true;}

        printf("Char received = ");
        for (int i = 0; i < rx_buffer_len + 1 + 2; i++) {
          printf("%X ", rx_buffer[i]);
        }
        printf("\n");
      }
    } else {
      if (serial.readable()) {
        serial.read(tx_buffer, 3);
        tx_buffer_len =
            ((uint16_t)tx_buffer[1] << 8) + (uint16_t)tx_buffer[2] + 0x01;
        serial.read(tx_buffer + 1 + 2, tx_buffer_len);

        printf("Char send = ");
        for (int i = 0; i < tx_buffer_len + 1 + 2; i++) {
          printf("%X ", tx_buffer[i]);
        }
        printf("\n");

        _bm64.write(tx_buffer, tx_buffer_len + 1 + 2);
		wait_us(10000);
		_bm64.write(ring_tone, 0x7);
      } else if (!paired){
		  _bm64.write(paired_buffer, 0x07);
	  }
      wait_us(1);
    }
    wait_us(10000);
  }
  printf("\n");
}
