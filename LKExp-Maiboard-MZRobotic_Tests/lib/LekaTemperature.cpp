#include "LekaTemperature.h"

LekaTemperature::LekaTemperature(I2C &interface, PinName pin_interrupt, uint8_t address)
	: _interface(interface), _hts221_interrupt(pin_interrupt), _address(address) {
	_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
	_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
	_register_io_function.handle	= (void *)this;
}

void LekaTemperature::init() {
	hts221_power_on_set(&_register_io_function, 1);
	//hts221_block_data_update_set(&_register_io_function, 1);
	hts221_data_rate_set(&_register_io_function, HTS221_ODR_12Hz5);
	hts221_temperature_avg_set(&_register_io_function, HTS221_T_AVG_4);
	hts221_humidity_avg_set(&_register_io_function, HTS221_H_AVG_4);
}

uint8_t LekaTemperature::getId() {
	uint8_t id = 0;
	hts221_device_id_get(&_register_io_function, &id);
	return id;
}

int16_t LekaTemperature::getRawData() {
	uint8_t data[2];

	hts221_temperature_raw_get(&_register_io_function, data);
	// hts221_humidity_raw_get(&_register_io_function, &data);

	int16_t ret = (int16_t)((((int16_t)data[1]) << 8) | (int16_t)data[0]);
	// int16_t ret = data[1] * 256 + data[0];

	return ret;
}

void LekaTemperature::showReferences() {
	uint8_t value[2];
	hts221_t1_t0_msb_t reg;

	hts221_read_reg(&_register_io_function, HTS221_T0_OUT_L, value, 2);
	printf("TO_ADC : 0x%02X 0x%02X \n", value[1], value[0]);
	hts221_read_reg(&_register_io_function, HTS221_T1_OUT_L, value, 2);
	printf("T1_ADC : 0x%02X 0x%02X \n", value[1], value[0]);

	hts221_read_reg(&_register_io_function, HTS221_T1_T0_MSB, (uint8_t *)&reg, 1);
	hts221_read_reg(&_register_io_function, HTS221_T0_DEGC_X8, value, 1);
	printf("T0_DEG : 0x%02X 0x%02X \n", reg.t0_msb, value[0]);
	hts221_read_reg(&_register_io_function, HTS221_T1_DEGC_X8, value, 1);
	printf("T1_DEG : 0x%02X 0x%02X \n\n", reg.t1_msb, value[0]);
}

float LekaTemperature::getData() {
	float_t t_0_adc, t_1_adc, t_0_deg, t_1_deg;
	float_t slope, intercept;

	hts221_temp_adc_point_0_get(&_register_io_function, &t_0_adc);
	hts221_temp_adc_point_1_get(&_register_io_function, &t_1_adc);
	hts221_temp_deg_point_0_get(&_register_io_function, &t_0_deg);
	hts221_temp_deg_point_1_get(&_register_io_function, &t_1_deg);

	printf("TO_ADC : %3.3f \nT1_ADC : %3.3f \nTO_DEG : %3.3f \nT1_DEG : %3.3f \n", t_0_adc, t_1_adc,
		   t_0_deg, t_1_deg);
	slope	  = (t_1_deg - t_0_deg) / (t_1_adc - t_0_adc);
	intercept = t_0_deg;

	return (float)((float_t)getRawData() * slope + intercept);
}

int32_t LekaTemperature::ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
							  uint16_t number_bytes_to_write) {
	return (int32_t)((LekaTemperature *)handle)->write(write_address, number_bytes_to_write, p_buffer);
}

int32_t LekaTemperature::ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
							 uint16_t number_bytes_to_read) {
	return (int32_t)((LekaTemperature *)handle)->read(read_address, number_bytes_to_read, p_buffer);
}

int32_t LekaTemperature::read(uint8_t register_address, uint16_t number_bytes_to_read, uint8_t *pBuffer) {
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

int32_t LekaTemperature::write(uint8_t register_address, uint16_t number_bytes_to_write, uint8_t *pBuffer) {
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

void LekaTemperature::runTest(int n_repetition) {

	init();

	for (int i = 0; i < n_repetition; i++) {
		printf("ID is %X\n\n", getId());

		//showReferences();
		printf("Temperature data (raw): 0x%04X \n\n", getRawData());
		printf("Temperature data (in Celsius): %3.1f \n\n\n", getData());
	}
}