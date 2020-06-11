#include "LekaIMU.h"

LekaIMU::LekaIMU(I2C &interface, PinName pin_interrupt, uint8_t address)
	: _interface(interface), _lsm6dsox_interrupt(pin_interrupt), _address(address) {
	_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
	_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
	_register_io_function.handle	= (void *)this;
}

void LekaIMU::init() {
	DigitalOut INT_1_LSM6DSOX(_lsm6dsox_interrupt, 0);

	lsm6dsox_init_set(&_register_io_function, LSM6DSOX_DRV_RDY);
	lsm6dsox_i3c_disable_set(&_register_io_function, LSM6DSOX_I3C_DISABLE);

	// lsm6dsox_xl_power_mode_set(&_register_io_function, LSM6DSOX_LOW_NORMAL_POWER_MD);
	// lsm6dsox_xl_data_rate_set(&_register_io_function, LSM6DSOX_XL_ODR_104Hz);
	// lsm6dsox_xl_full_scale_set(&_register_io_function, LSM6DSOX_2g);
	//
	// lsm6dsox_gy_power_mode_set(&_register_io_function, LSM6DSOX_GY_NORMAL);
	// lsm6dsox_gy_data_rate_set(&_register_io_function, LSM6DSOX_GY_ODR_104Hz);
	// lsm6dsox_gy_full_scale_set(&_register_io_function, LSM6DSOX_125dps);

	lsm6dsox_mode_get(&_register_io_function, NULL, &_config);

	_config.ui.xl.odr = _config.ui.xl.LSM6DSOX_XL_UI_104Hz_NM;
	_config.ui.xl.fs  = _config.ui.xl.LSM6DSOX_XL_UI_2g;
	_config.ui.gy.odr = _config.ui.gy.LSM6DSOX_GY_UI_104Hz_NM;
	_config.ui.gy.fs  = _config.ui.gy.LSM6DSOX_GY_UI_125dps;

	lsm6dsox_mode_set(&_register_io_function, NULL, &_config);
}

uint8_t LekaIMU::getId() {
	uint8_t id = 0;
	lsm6dsox_device_id_get(&_register_io_function, &id);
	return id;
}

void LekaIMU::getData(std::array<float, 3> &xl_data, std::array<float, 3> &gy_data) {
	lsm6dsox_data_t data;
	// lsm6dsox_mode_get(&_register_io_function, NULL, &_config);

	lsm6dsox_data_get(&_register_io_function, NULL, &_config, &data);

	xl_data[0] = data.ui.xl.mg[0];
	xl_data[1] = data.ui.xl.mg[1];
	xl_data[2] = data.ui.xl.mg[2];
	gy_data[0] = data.ui.gy.mdps[0];
	gy_data[1] = data.ui.gy.mdps[1];
	gy_data[2] = data.ui.gy.mdps[2];
}

int32_t LekaIMU::ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
							  uint16_t number_bytes_to_write) {
	return (int32_t)((LekaIMU *)handle)->write(write_address, number_bytes_to_write, p_buffer);
}

int32_t LekaIMU::ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
							 uint16_t number_bytes_to_read) {
	return (int32_t)((LekaIMU *)handle)->read(read_address, number_bytes_to_read, p_buffer);
}

int32_t LekaIMU::read(uint8_t register_address, uint16_t number_bytes_to_read, uint8_t *pBuffer) {
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

int32_t LekaIMU::write(uint8_t register_address, uint16_t number_bytes_to_write, uint8_t *pBuffer) {
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

void LekaIMU::runTest(int n_repetition) {

	init();

	std::array<float, 3> xl_data;
	std::array<float, 3> gy_data;

	for (int i; i < n_repetition; i++) {
		printf("ID is %X\r\n", getId());

		getData(xl_data, gy_data);
		printf("Accelrometer data (in mg): %d %d %d \r\n", (int)xl_data[0], (int)xl_data[1],
			   (int)xl_data[2]);
		printf("Gyroscope data (in mdps): %d %d %d \r\n\n", (int)gy_data[0], (int)gy_data[1],
			   (int)gy_data[2]);
	}
}