/**
******************************************************************************
* @file    LSM6DSOX_MachineLearningCore.cpp
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Abstract class for a machine learning core
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "LSM6DSOX_MachineLearningCore.h"

namespace MachineLearningCore {
	/* Class Implementation ------------------------------------------------------*/

	/**
	 * @brief Construct a new LSM6DSOX_MachineLearningCore object
	 * 
	 * @param component_i2c I2C communication interface
	 * @param pin_interrupt1 Device pin to which the LSM6DSOX INT1 pin is connected
	 * @param pin_interrupt2 Device pin to which the LSM6DSOX INT2 pin is connected
	 */
	LSM6DSOX_MachineLearningCore::LSM6DSOX_MachineLearningCore(Communication::I2CBase &component_i2c,
												   PinName pin_interrupt1, PinName pin_interrupt2)
		: _lsm6dsox_component_i2c(component_i2c),
		  _mcu_pin_interrupt1(pin_interrupt1),
		  _mcu_pin_interrupt2(pin_interrupt2),
		  _lsm6dsox_interrupt1(pin_interrupt1),
		  _lsm6dsox_interrupt2(pin_interrupt2){
		_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
		_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
		_register_io_function.handle	= (void *)this;
	}

	/**
	 * @brief Init the LSM6DSOX_MachineLearningCore object
	 * 
	 * This method sends an MLC init request to the sensor.
	 * It will also set the MLC data rate to its default value.
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::init() {
		/* Initialize the component for driver usage */
		if (lsm6dsox_init_set(&_register_io_function, LSM6DSOX_MLC) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

		return setDataRate(26.0f);
	}

	/**
	 * @brief Disable I3C interface
	 * 
	 * Disabling I3C on the sensor allows to use the INT1 pin as input while keeping I2C active.
	 * If I3C is not disabled and INT1 pin stops being at 0, the sensor will shut down its I2C interface.
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::disableI3C() {
		/* Disable I3C interface */
		if (lsm6dsox_i3c_disable_set(&_register_io_function, LSM6DSOX_I3C_DISABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

	/**
	 * @brief Get the ID of the LSM6DSOX sensor connected 
	 * 
	 * @param id where to store the ID
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getID(uint8_t &id) {
		uint8_t p_data = 0;
		if (lsm6dsox_device_id_get(&_register_io_function, &p_data) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		id = p_data;
		
		return Status::OK;
	}

	/**
	 * @brief Restores the default values of all the control registers of the sensor
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::restoreDefaultConfiguration()
	{
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


	//this is a temporary method to ease up developpement
	// TODO erase once no more in use 
	/**
	 * @brief Allow to recover the Register IO function from the inner class
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	stmdev_ctx_t* LSM6DSOX_MachineLearningCore::TMP_getIoFunc()
	{
		return &_register_io_function;
	}

	/**
	 * @brief Enable the machine learning core
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::enable(){
		uint8_t enableVal = 1;
		
		if (lsm6dsox_mlc_set(&_register_io_function, enableVal) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	} 

	/**
	 * @brief Disable the machine learning core
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::disable(){
		uint8_t disableVal = 0;
		
		if (lsm6dsox_mlc_set(&_register_io_function, disableVal) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

	
	/**
	 * @brief Set up the decision tree in the sensor's memory 
	 * 
	 * Setup of the decision tree from the configuration file (header generated from an ucf file)
	 * The number of lines is needed and is obtained thanks to the following operation
	 * (sizeof(ucfConfig) / sizeof(ucf_line_t) -> number of lines
	 * 
	 * @param ucfConfig Class stored in the configuration file corresponding to the decision tree
	 * @param numLines Number of lines in the configuration file
	 * @retval 0 in case of success, an error code otherwise
	 */
    Status LSM6DSOX_MachineLearningCore::configureMLC(const ucf_line_t ucfConfig[], uint32_t numLines) {
        uint8_t  rst;
        /* Restore default configuration */
        if (lsm6dsox_reset_set(&_register_io_function , PROPERTY_ENABLE) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
        do {
            if (lsm6dsox_reset_get(&_register_io_function, &rst) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		    }
        } while (rst);

        /* Start Machine Learning Core configuration */
        for (uint32_t i = 0; i < numLines; ++i )
        {
            if (lsm6dsox_write_reg(&_register_io_function, ucfConfig[i].address, (uint8_t*)&ucfConfig[i].data, 1) !=
                    (int32_t)Communication::Status::OK)
            {
                    return Status::ERROR;
            }
		}
        return Status::OK;
    }
    


	/**
	 * @brief  Set the data rate of the machine learning core
     * 
     * Quote from datasheet
     * The rate of the input data must be equal to or higher than the Machine Learning Core data rate configurable
        through the embedded function register EMB_FUNC_ODR_CFG_C (60h), as described in Table 1.
    * Example: In an activity recognition algorithm running at 26 Hz, the Machine Learning Core ODR must be selected
        at 26 Hz, while the sensor ODRs must be equal to or higher than 26 Hz.
     * 
	 * @param  data_rate is the data rate (in Hz) intended
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::setDataRate(float data_rate) {
		
        lsm6dsox_mlc_odr_t odr;
	
		if (data_rate < 26.0f) {
			odr = LSM6DSOX_ODR_PRGS_12Hz5;
		} 
        else if (data_rate < 52.0f) {
			odr = LSM6DSOX_ODR_PRGS_26Hz;
		} 
        else if (data_rate < 104.0f) {
			odr = LSM6DSOX_ODR_PRGS_52Hz;
		} 
        else odr = LSM6DSOX_ODR_PRGS_104Hz;
	

		if (lsm6dsox_mlc_data_rate_set(&_register_io_function, odr) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

	/**
	 * @brief Get the data rate of the machine learning core
	 * 
	 * @param data_rate is where to store the read data rate (in Hz) 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getDataRate(float &data_rate) {
		Status ret = Status::OK;

		lsm6dsox_mlc_odr_t odr;
		if (lsm6dsox_mlc_data_rate_get(&_register_io_function, &odr) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

		switch (odr) {

			case LSM6DSOX_ODR_PRGS_12Hz5:
				data_rate = 12.5f;
				break;
			case LSM6DSOX_ODR_PRGS_26Hz:
				data_rate = 26.0f;
				break;
			case LSM6DSOX_ODR_PRGS_52Hz:
				data_rate = 52.0f;
				break;
			case LSM6DSOX_ODR_PRGS_104Hz:
				data_rate = 104.0f;
				break;
		}
		return ret;
	}

	/**
	 * @brief Get the data returned by each decision tree
	 * 
	 * @param data is an array in which to store the read data from the MLC
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getData(std::array<uint8_t, 8> &data) {
        uint8_t buff[8];
        Data mlc	 = {{0}};
        if( lsm6dsox_mlc_out_get(&_register_io_function, buff) != 
        	(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

        for (int i = 0; i< 8; ++i)
        {
            mlc.array[i] = buff[i];
        }

        data = mlc.array;
		return Status::OK;
	}

	/**
	 * @brief Return which pin is used for interrupt signals INT1 and INT2
	 * 
	 * @param intNum is either _INT1=1 or _INT2=2
	 * @retval The name of the pin used for the interrupt
	 */
	PinName LSM6DSOX_MachineLearningCore::getInterruptPin(InterruptNumber intNum) {
		 if( intNum == InterruptNumber::_INT1  )  return _mcu_pin_interrupt1; 
		 else return _mcu_pin_interrupt2;
	}

	/**
	 * @brief Enable all interrupts on the LSM6DSOX (Should be moved elsewhere)
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::enableInterrupt() {
		_lsm6dsox_interrupt1.enable_irq();
		return Status::OK;
	}

	/**
	 * @brief Disable all interrupts on the LSM6DSOX (Should be moved elsewhere)
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::disableInterrupt() {
		_lsm6dsox_interrupt1.disable_irq();
		return Status::OK;
	}

	/**
	 * @brief Set the Interrupt behavior of the MLC trees
	 * 
	 * @param behavior pulsed or latched 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::setInterruptBehavior(InterruptBehavior behavior){
		lsm6dsox_lir_t val;
		;
		if( lsm6dsox_int_notification_get(&_register_io_function, &val)  != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
		}

		switch (val)
		{
		case lsm6dsox_lir_t::LSM6DSOX_ALL_INT_PULSED:
			if(behavior == InterruptBehavior::_LATCHED)
				val = lsm6dsox_lir_t::LSM6DSOX_BASE_PULSED_EMB_LATCHED;
			break;
		case lsm6dsox_lir_t::LSM6DSOX_BASE_LATCHED_EMB_PULSED:
			if(behavior == InterruptBehavior::_LATCHED)
				val = lsm6dsox_lir_t::LSM6DSOX_ALL_INT_LATCHED;
			break;
		case lsm6dsox_lir_t::LSM6DSOX_BASE_PULSED_EMB_LATCHED:
			if(behavior == InterruptBehavior::_PULSED)
				val = lsm6dsox_lir_t::LSM6DSOX_ALL_INT_PULSED;
			break;
		case lsm6dsox_lir_t::LSM6DSOX_ALL_INT_LATCHED:
			if(behavior == InterruptBehavior::_PULSED)
				val = lsm6dsox_lir_t::LSM6DSOX_BASE_LATCHED_EMB_PULSED;
			break;
		}

		if( lsm6dsox_int_notification_set(&_register_io_function, val)  != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
		}
		return Status::OK;
	}

	/**
	 * @brief  Enable interrupt from a given tree on INT1, INT2 or both
	 * 
	 * This function could be made simpler by only writing to the registers MLC_INT1(0Dh) and MLC_INT2(11h)
	 * but this feature is not implemented in the driver functions, so we would need to manage writing to registers in a "lower level" way
	 * 
	 * @param tree The MLC tree on which to set the interrupt
	 * @param intNum the interrupt on which you want the signal to be routed
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::enableTreeInterrupt(Tree tree, TreeInterruptNum intNum) {

		lsm6dsox_pin_int1_route_t   pin_int1_route;
		lsm6dsox_pin_int2_route_t   pin_int2_route;

		if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if( lsm6dsox_pin_int1_route_get(&_register_io_function, &pin_int1_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}

		if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if( lsm6dsox_pin_int2_route_get(&_register_io_function, NULL, &pin_int2_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}
		
		switch(tree)
		{
			case Tree::_TREE_1:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc1 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc1 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_2:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc2 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc2 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_3:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc3 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc3 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_4:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc4 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc4 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_5:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc5 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc5 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_6:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc6 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc6 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_7:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc7 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc7 = PROPERTY_ENABLE;
			break;
			case Tree::_TREE_8:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc8 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc8 = PROPERTY_ENABLE;
			break;
		}
  		
		if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if(lsm6dsox_pin_int1_route_set(&_register_io_function, pin_int1_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}

		if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if(lsm6dsox_pin_int2_route_set(&_register_io_function, NULL, pin_int2_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}
		
		return Status::OK;
	}

	/**
	 * @brief Disable the interrupt from a given tree on INT1, INT2 or both
	 * 
	 * @param tree The MLC tree on which to set the interrupt
	 * @param intNum the interrupt on which you want the signal to be routed
	 * @retval 0 in case of success, an error code otherwise 
	 */
	Status LSM6DSOX_MachineLearningCore::disableTreeInterrupt(Tree tree, TreeInterruptNum intNum) {
		
		lsm6dsox_pin_int1_route_t   pin_int1_route;
		lsm6dsox_pin_int2_route_t   pin_int2_route;

		if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if( lsm6dsox_pin_int1_route_get(&_register_io_function, &pin_int1_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}

		if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if( lsm6dsox_pin_int2_route_get(&_register_io_function, NULL, &pin_int2_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}
		
		switch(tree)
		{
			case Tree::_TREE_1:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc1 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc1 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_2:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc2 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc2 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_3:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc3 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc3 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_4:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc4 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc4 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_5:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc5 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc5 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_6:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc6 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc6 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_7:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc7 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc7 = PROPERTY_DISABLE;
			break;
			case Tree::_TREE_8:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc8 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc8 = PROPERTY_DISABLE;
			break;
		}
  		
		if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if(lsm6dsox_pin_int1_route_set(&_register_io_function, pin_int1_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}

		if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2)
		{
			if(lsm6dsox_pin_int2_route_set(&_register_io_function, NULL, pin_int2_route) != 
				(int32_t)Communication::Status::OK) {
				return Status::ERROR;
			}
		}
		
		return Status::OK;
	}

	/**
	 * @brief Get which interrupts are routed on a given decision tree 
	 * 
	 * @param tree The MLC tree on which to set the interrupt
	 * @param intNum is the TreeInterruptNum where to store the interrupt on which the signal is routed 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getTreeInterrupt(Tree tree, TreeInterruptNum &intNum)
	{
		lsm6dsox_pin_int1_route_t   pin_int1_route;
		lsm6dsox_pin_int2_route_t   pin_int2_route;

		TreeInterruptNum iNum = TreeInterruptNum::_NC;

		if( lsm6dsox_pin_int1_route_get(&_register_io_function, &pin_int1_route) != 
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

		if( lsm6dsox_pin_int2_route_get(&_register_io_function, NULL, &pin_int2_route) != 
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		
		switch(tree)
		{
			case Tree::_TREE_1:
				if( pin_int1_route.mlc1 == PROPERTY_ENABLE && pin_int2_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_2:
				if( pin_int1_route.mlc2 == PROPERTY_ENABLE && pin_int2_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_3:
				if( pin_int1_route.mlc3 == PROPERTY_ENABLE && pin_int2_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_4:
				if( pin_int1_route.mlc4 == PROPERTY_ENABLE && pin_int2_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_5:
				if( pin_int1_route.mlc5 == PROPERTY_ENABLE && pin_int2_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_6:
				if( pin_int1_route.mlc6 == PROPERTY_ENABLE && pin_int2_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_7:
				if( pin_int1_route.mlc7 == PROPERTY_ENABLE && pin_int2_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case Tree::_TREE_8:
				if( pin_int1_route.mlc8 == PROPERTY_ENABLE && pin_int2_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
		}

		intNum = iNum;

		return Status::OK;
	}

	/**
	 * @brief Get the status of the interrupt for a given tree
	 * 
	 * @param tree The MLC tree on which to set the interrupt
	 * @param treeStatus 1 in case of interrupt from the tree, 0 otherwise
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getTreeInterruptStatus(Tree tree, uint8_t &treeStatus)
	{
		lsm6dsox_all_sources_t status;
		if( lsm6dsox_all_sources_get(&_register_io_function, &status) != 
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		switch (tree){
			
		case Tree::_TREE_1:
			treeStatus = status.mlc1;
			break;
		case Tree::_TREE_2:
			treeStatus = status.mlc2;
			break;
		case Tree::_TREE_3:
			treeStatus = status.mlc3;
			break;
		case Tree::_TREE_4:
			treeStatus = status.mlc4;
			break;
		case Tree::_TREE_5:
			treeStatus = status.mlc5;
			break;
		case Tree::_TREE_6:
			treeStatus = status.mlc6;
			break;
		case Tree::_TREE_7:
			treeStatus = status.mlc7;
			break;
		case Tree::_TREE_8:
			treeStatus = status.mlc8;
			break;
		}

		return Status::OK;
	}




	/**
	 * @brief Read the interrupt state on INT1
	 * 
	 * @param interrupt_status is where the state of INT1 will be stored
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::readInterrupt(uint8_t &interrupt_status, InterruptNumber intNum) {
		if ( intNum == InterruptNumber::_INT1) interrupt_status = _lsm6dsox_interrupt1.read();
		else if ( intNum == InterruptNumber::_INT2) interrupt_status = _lsm6dsox_interrupt2.read();
		return Status::OK;
	}

	/**
	 * @brief Attach a callback function on INT1 or INT2
	 * 
	 * @param func is the callback function to attach on INT1
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::attachInterrupt(Callback<void()> func, InterruptNumber intNum) {
		if(intNum == InterruptNumber::_INT1) _lsm6dsox_interrupt1.rise(func);
		else if(intNum == InterruptNumber::_INT2) _lsm6dsox_interrupt2.rise(func);
		return Status::OK;
	}

	/**
	 * @brief Use to enable or disable rerouting of INT2 interrupts on INT1 pin
	 * 
	 * @param enable true to enable, false otherwise
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::allOnInt1Route( bool enable){
		uint8_t property = PROPERTY_DISABLE;
		if(enable) property = PROPERTY_ENABLE;
		
		if( lsm6dsox_all_on_int1_set(&_register_io_function, property) != 
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

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
	int32_t LSM6DSOX_MachineLearningCore::ptr_io_write(void *handle, uint8_t write_address,
												 uint8_t *p_buffer,
												 uint16_t number_bytes_to_write) {
		return (int32_t)((LSM6DSOX_MachineLearningCore *)handle)
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
	int32_t LSM6DSOX_MachineLearningCore::ptr_io_read(void *handle, uint8_t read_address,
												uint8_t *p_buffer, uint16_t number_bytes_to_read) {
		return (int32_t)((LSM6DSOX_MachineLearningCore *)handle)
			->_lsm6dsox_component_i2c.read(read_address, number_bytes_to_read, p_buffer);
	}
}	// namespace MachineLearningCore
