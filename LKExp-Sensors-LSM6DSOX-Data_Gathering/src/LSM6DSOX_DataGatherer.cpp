#include "LSM6DSOX_DataGatherer.h"

namespace Component {

	LSM6DSOX_DataGatherer::LSM6DSOX_DataGatherer(
		Communication::I2CBase &component_i2c, PinName pin_interrupt1, float dataRate, PinName tx,
		PinName rx, int baud, void(*callBack)())
		: _lsm6dsox_component_i2c(component_i2c),
		  _mcu_pin_interrupt1(pin_interrupt1),
		  _lsm6dsox_interrupt1(pin_interrupt1),
		  _accelerometer(component_i2c, pin_interrupt1),
		  _gyroscope(component_i2c, pin_interrupt1),
		  _dataRate(dataRate),
		  _serial(tx, rx, baud),
		  _tickerCB(callBack){

		_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
		_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
		_register_io_function.handle	= (void *)this;
		
		_queue = mbed_event_queue();
		//_serialHandler = _queue->event(this, &LSM6DSOX_DataGatherer::client_sigio, client);

		_serialInputStr.reserve(100);
	}

	Status LSM6DSOX_DataGatherer::init() {

		// Initialize the component for driver usage
		if (_accelerometer.init() != Status::OK) return Status::ERROR;
		if (_gyroscope.init() != Status::OK) return Status::ERROR;

		//// Turn off Sensors
		if (_accelerometer.setPowerMode(Component::PowerMode::OFF) != Status::OK)
			return Status::ERROR;
		if (_gyroscope.setPowerMode(Component::PowerMode::OFF) != Status::OK) return Status::ERROR;

		// Enable Block Data Update
		if (lsm6dsox_block_data_update_set(&_register_io_function, PROPERTY_ENABLE) !=
			(int32_t)Communication::Status::OK)
			return Status::ERROR;

		// Configure accelerometer
		if (_accelerometer.setRange(AccelerometerRange::_2G) != Status::OK) return Status::ERROR;
		if (_accelerometer.setDataRate(104.0f) != Status::OK) return Status::ERROR;
		if (_accelerometer.setPowerMode(PowerMode::NORMAL) != Status::OK) return Status::ERROR;

		// Configure accelerometer
		if (_gyroscope.setRange(GyroscopeRange::_500DPS) != Status::OK) return Status::ERROR;
		if (_gyroscope.setDataRate(104.0f) != Status::OK) return Status::ERROR;
		if (_gyroscope.setPowerMode(PowerMode::NORMAL) != Status::OK) return Status::ERROR;

		// Serial config
		_serial.attach(callback(this, &LSM6DSOX_DataGatherer::onSerialReceived),
					   UnbufferedSerial::RxIrq);
		//_serial.attach(_queue->event(_tickerCB), UnbufferedSerial::RxIrq);

		// Data rate and ticker config
		setDataRate(_dataRate);

		return Status::OK;
	}

	Status LSM6DSOX_DataGatherer::disableI3C() {
		/* Disable I3C interface */
		if (lsm6dsox_i3c_disable_set(&_register_io_function, LSM6DSOX_I3C_DISABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

	Status LSM6DSOX_DataGatherer::getID(uint8_t &id) {
		uint8_t p_data = 0;
		if (lsm6dsox_device_id_get(&_register_io_function, &p_data) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		id = p_data;

		return Status::OK;
	}

	Status LSM6DSOX_DataGatherer::restoreDefaultConfiguration() {
		uint8_t rst;
		if (lsm6dsox_reset_set(&_register_io_function, PROPERTY_ENABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		do {
			if (lsm6dsox_reset_get(&_register_io_function, &rst) !=
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		} while (rst);

		return Status::OK;
	}

	stmdev_ctx_t *LSM6DSOX_DataGatherer::TMP_getIoFunc() { return &_register_io_function; }

	void LSM6DSOX_DataGatherer::setDataRate(float dataRate) {
		if (dataRate < 0)
			dataRate = 0.0f;
		else if (dataRate > 104.0f)
			dataRate = 104.0f;
		_dataRate = dataRate;
	}

	void LSM6DSOX_DataGatherer::setDataRateWhileRunning(float dataRate) {
		setDataRate(dataRate);
		stopTicker();
		startTicker();
	}

	void LSM6DSOX_DataGatherer::getDataRate(float &data_rate) { data_rate = _dataRate; }

	Status LSM6DSOX_DataGatherer::getData(std::array<float, 6> &data) {

		DataGathererData res;
		if (_accelerometer.getData(res.values.acc.data) != Status::OK) return Status::ERROR;
		if (_gyroscope.getData(res.values.gy.data) != Status::OK) return Status::ERROR;

		data = res.array;

		return Status::OK;
	}

	PinName LSM6DSOX_DataGatherer::getInterruptPin() { return _mcu_pin_interrupt1; }

	Status LSM6DSOX_DataGatherer::enableInterrupt() {
		_lsm6dsox_interrupt1.enable_irq();
		return Status::OK;
	}

	Status LSM6DSOX_DataGatherer::disableInterrupt() {
		_lsm6dsox_interrupt1.disable_irq();
		return Status::OK;
	}

	/**
	 * @brief
	 *
	 * @param handle
	 * @param write_address
	 * @param p_buffer
	 * @param number_bytes_to_write
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_DataGatherer::ptr_io_write(void *handle, uint8_t write_address,
												uint8_t *p_buffer, uint16_t number_bytes_to_write) {
		return (int32_t)((LSM6DSOX_DataGatherer *)handle)
			->_lsm6dsox_component_i2c.write(write_address, number_bytes_to_write, p_buffer);
	}

	/**
	 * @brief
	 *
	 * @param handle
	 * @param read_address
	 * @param p_buffer
	 * @param number_bytes_to_read
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_DataGatherer::ptr_io_read(void *handle, uint8_t read_address,
											   uint8_t *p_buffer, uint16_t number_bytes_to_read) {
		return (int32_t)((LSM6DSOX_DataGatherer *)handle)
			->_lsm6dsox_component_i2c.read(read_address, number_bytes_to_read, p_buffer);
	}

	void LSM6DSOX_DataGatherer::printHeader() {
		string msg = "\n\nSTEVAL-MKI197V1 (LSM6DSOX)\n\n";
		_serial.write(msg.c_str(), msg.size());

		msg = "A_X [mg]\tA_Y [mg]\tA_Z [mg]\tG_X [mg]\tG_Y [mg]\tG_Z [mg]\n";
		_serial.write(msg.c_str(), msg.size());
	}

	void LSM6DSOX_DataGatherer::printData(DataGathererData data) {
		char nb[30];
		string msg = "";

		for (int i = 0; i < 6; ++i) {
			sprintf(nb, "%d\t", (int)(data.array[i]));
			msg.append(nb);
		}
		msg.append("\n");

		_serial.write(msg.c_str(), msg.size());
	}

	void LSM6DSOX_DataGatherer::onSerialReceived() {
		// printf("Reading serial\n");
		char chr = '\0';
		while (_serial.readable()) {
			_serial.read(&chr, 1);
			if (chr != '\n' && chr != '\0' && chr != '\r')
				_serialInputStr.push_back(chr);
			else if (chr != '\r')
				//_serialStrComplete = true;
				_queue->call(callback(this, &LSM6DSOX_DataGatherer::parseCommand));
		}
		// _serial.write("Serial!\n", 8);	
	}

	void LSM6DSOX_DataGatherer::sendIntBinary(int var) {
		char *bytePtr = (char *)&var;
		for (int i = 3; i >= 0; --i) { _serial.write((void *)(bytePtr + i), 1); }
	}

	void LSM6DSOX_DataGatherer::onTick() {
		DataGathererData d;
		//d.array = {100000000, 200000000, 300000000, 400000000, 500000000, 600000000};
		getData(d.array);
		//printData(d);

		for(int i = 0 ; i < 6; i++){
			sendIntBinary((int)d.array[i]);
		}
		
		// static char c = 'a';
		// _serial.write(&c, 1);
		// c++;
		// if(c > 'z') c = 'a';
	}

	void LSM6DSOX_DataGatherer::startTicker() {
		stopTicker();
        std::chrono::microseconds period_us((long)(1000000/_dataRate));
        _ticker.attach(_queue->event(_tickerCB), period_us);
	}

	void LSM6DSOX_DataGatherer::stopTicker() { _ticker.detach(); }

	void LSM6DSOX_DataGatherer::parseCommand() {
		//TODO change commandstring
		string commandString = _serialInputStr;

		string msg;
		char chars[100];
		int nbChars;

		msg = "Your command is: \"" + commandString + "\"\n";
		_serial.write(msg.c_str(), msg.size());

		if (commandString == "start") {
			startTicker();
		} else if (commandString == "stop") {
			stopTicker();
		} else if (commandString.substr(0, 5) == "rate ") {
			commandString = commandString.substr(5, commandString.length() - 5);
			msg			  = "Rate you want to set: " + commandString + "\n";
			_serial.write(msg.c_str(), msg.size());

			float rate;
			if (isFloat(commandString)) {
				rate = stof(commandString);
				if (rate > 0.0 && rate <= 104.0) {
					setDataRate(rate);

					nbChars = sprintf(chars, "New data rate: %d\n\n", (int)rate);
					_serial.write(chars, nbChars);
				} else {
					nbChars =
						sprintf(chars, "Data rate out of bounds, data rate still set to : '%d'\n",
								(int)_dataRate);
					_serial.write(chars, nbChars);
				}
			} else {
				nbChars = sprintf(chars, "Data rate not valid, data rate still set to : '%d'\n",
								  (int)_dataRate);
				_serial.write(chars, nbChars);
			}
		} else if (commandString.substr(0, 6) == "range ") {
			msg = "Changing range\n";
			_serial.write(msg.c_str(), msg.size());

			commandString = commandString.substr(6, commandString.length() - 6);
			if (commandString.substr(0, 3) == "xl ") {
				msg = "On accelerometer\n";
				_serial.write(msg.c_str(), msg.size());

				commandString = commandString.substr(3, commandString.length() - 3);
				Component::AccelerometerRange range;
				_accelerometer.getRange(range);

				if (commandString == "2G")
					range = Component::AccelerometerRange::_2G;
				else if (commandString == "4G")
					range = Component::AccelerometerRange::_4G;
				else if (commandString == "8G")
					range = Component::AccelerometerRange::_8G;
				else if (commandString == "16G")
					range = Component::AccelerometerRange::_16G;
				else {
					nbChars = sprintf(chars, "Invalid range input, range is still %dG\n", (int)range);
					_serial.write(chars, nbChars);
				}

				_accelerometer.setRange(range);

				nbChars = sprintf(chars, "Accelerometer range set to: %dG\n", (int)range);
				_serial.write(chars, nbChars);

			} else if (commandString.substr(0, 3) == "gy ") {
				msg = "On gyroscope\n";
				_serial.write(msg.c_str(), msg.size());

				commandString = commandString.substr(3, commandString.length() - 3);
				Component::GyroscopeRange range;
				_gyroscope.getRange(range);

				if (commandString == "125DPS")
					range = Component::GyroscopeRange::_125DPS;
				else if (commandString == "250DPS")
					range = Component::GyroscopeRange::_250DPS;
				else if (commandString == "500DPS")
					range = Component::GyroscopeRange::_500DPS;
				else if (commandString == "1000DPS")
					range = Component::GyroscopeRange::_1000DPS;
				else if (commandString == "2000DPS")
					range = Component::GyroscopeRange::_2000DPS;
				else {
					nbChars = sprintf(chars, "Invalid range input, range is still %dDPS\n",(int) range);
					_serial.write(chars, nbChars);
				}

				_gyroscope.setRange(range);

				nbChars = sprintf(chars, "Gyroscope range set to: %dDPS\n",(int) range);
				_serial.write(chars, nbChars);
			} else {
				msg = "Not a valid device chosen (xl or gy)\n";
				_serial.write(msg.c_str(), msg.size());
			}
		} else if (commandString == "help") {
			msg = "start \n";
			msg += "stop \n";
			msg += "rate [value] \n";
			msg += "range [device] [value] \n";
			_serial.write(msg.c_str(), msg.size());

		} else {

			msg = "Invalid command, type 'help' for more info\n";
			_serial.write(msg.c_str(), msg.size());
		}

		_serialStrComplete = false;
		_serialInputStr	   = "";
	}

	bool LSM6DSOX_DataGatherer::isFloat(string s) {
		int numPoints = 0;
		for (unsigned int i = 0; i < s.length(); ++i) {
			if (s[i] == '.') {
				if ((numPoints == 0) && (i != 0) && (i != s.length() - 1))
					numPoints++;
				else
					return false;
			} else if ((s[i] < '0') || (s[i] > '9'))
				return false;
		}
		return true;
	}

}	// namespace Component