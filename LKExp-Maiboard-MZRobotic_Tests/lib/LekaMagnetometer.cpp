#include "LekaMagnetometer.h"

LekaMagnetometer::LekaMagnetometer(I2C &interface, PinName pin_interrupt, uint8_t address)
	: _interface(interface), _lsm303agr_interrupt(pin_interrupt), _address(address) {
	_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
	_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
	_register_io_function.handle	= (void *)this;
}

void LekaMagnetometer::init() {
	lsm303agr_mag_operating_mode_set(&_register_io_function, LSM303AGR_CONTINUOUS_MODE);
	lsm303agr_mag_data_rate_set(&_register_io_function, LSM303AGR_MG_ODR_50Hz);
	lsm303agr_mag_power_mode_set(&_register_io_function, LSM303AGR_LOW_POWER);
}

uint8_t LekaMagnetometer::getId() {
	uint8_t id = 0;
	lsm303agr_mag_device_id_get(&_register_io_function, &id);
	return id;
}

void LekaMagnetometer::getData(std::array<float, 3> &mag_data) {
	uint8_t data[6];

	lsm303agr_magnetic_raw_get(&_register_io_function, data);

	mag_data[0] = (float)lsm303agr_from_lsb_to_mgauss(
		(int16_t)((((uint16_t)data[1]) << 8) | (uint16_t)data[0]));
	mag_data[1] = (float)lsm303agr_from_lsb_to_mgauss(
		(int16_t)((((uint16_t)data[3]) << 8) | (uint16_t)data[2]));
	mag_data[2] = (float)lsm303agr_from_lsb_to_mgauss(
		(int16_t)((((uint16_t)data[5]) << 8) | (uint16_t)data[4]));
}

int32_t LekaMagnetometer::ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
									   uint16_t number_bytes_to_write) {
	return (int32_t)((LekaMagnetometer *)handle)
		->write(write_address, number_bytes_to_write, p_buffer);
}

int32_t LekaMagnetometer::ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
									  uint16_t number_bytes_to_read) {
	return (int32_t)((LekaMagnetometer *)handle)
		->read(read_address, number_bytes_to_read, p_buffer);
}

int32_t LekaMagnetometer::read(uint8_t register_address, uint16_t number_bytes_to_read,
							   uint8_t *pBuffer) {
	int ret;

	/* Send component address, with no STOP condition */
	ret = _interface.write(_address, (const char *)&register_address, 1, true);
	if (!ret) {
		/* Read data, with STOP condition  */
		ret = _interface.read(_address, (char *)pBuffer, number_bytes_to_read, false);
	}

	if (ret) return 1;
	return 0;
}

int32_t LekaMagnetometer::write(uint8_t register_address, uint16_t number_bytes_to_write,
								uint8_t *pBuffer) {
	int ret;
	uint8_t tmp[kBufferSize];

	if (number_bytes_to_write >= kBufferSize) return 2;

	/* First, send component address. Then, send data and STOP condition */
	tmp[0] = register_address;
	memcpy(tmp + 1, pBuffer, number_bytes_to_write);

	ret = _interface.write(_address, (const char *)tmp, number_bytes_to_write + 1, false);

	if (ret) return 1;
	return 0;
}

void LekaMagnetometer::runTest(int n_repetition) {
	printf("\nTest of compass sensor!\n");
	init();

	std::array<float, 3> mag_data;

	for (int i; i < n_repetition; i++) {
		printf("ID is %X\n", getId());

		getData(mag_data);
		printf("Magnetometer data (in mgauss): %d %d %d\n", (int)mag_data[0], (int)mag_data[1],
			   (int)mag_data[2]);
	}
	printf("\n");
}