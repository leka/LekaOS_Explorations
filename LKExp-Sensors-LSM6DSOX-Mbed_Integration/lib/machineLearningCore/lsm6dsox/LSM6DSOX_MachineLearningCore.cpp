

/* Includes ------------------------------------------------------------------*/
#include "LSM6DSOX_MachineLearningCore.h"

namespace MachineLearningCore {
	/* Class Implementation ------------------------------------------------------*/

	/** Constructor
	 * @param i2c object which handles the I2C of the component
	 */
	LSM6DSOX_MachineLearningCore::LSM6DSOX_MachineLearningCore(Communication::I2CBase &component_i2c,
												   PinName pin_interrupt1, PinName pin_interrupt2)
		: _lsm6dsox_component_i2c(component_i2c),
		  _mcu_pin_interrupt1(pin_interrupt1),
		  _mcu_pin_interrupt2(pin_interrupt2),
		  _lsm6dsox_interrupt(pin_interrupt1) {
		_register_io_function.write_reg = (stmdev_write_ptr)ptr_io_write;
		_register_io_function.read_reg	= (stmdev_read_ptr)ptr_io_read;
		_register_io_function.handle	= (void *)this;
	}

	/**
	 * @brief  Initializing the component
	 * @retval 0 in case of success, an error code otherwise
	 */

	Status LSM6DSOX_MachineLearningCore::init() {
		/* Initialize the component for driver usage */
		if (lsm6dsox_init_set(&_register_io_function, LSM6DSOX_MLC) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

		setDataRate(26.0f);

		return Status::OK;
	}

	/**
	 * @brief  Disable I3C
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
	 * @brief  Read component ID
	 * @param  id the WHO_AM_I value
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

	//this is a temporary method to ease up developpement
	// TODO erase once no more in use
	stmdev_ctx_t* LSM6DSOX_MachineLearningCore::TMP_getIoFunc()
	{
		return &_register_io_function;
	}


	Status LSM6DSOX_MachineLearningCore::enable(){
		uint8_t enableVal = 1;
		
		if (lsm6dsox_mlc_set(&_register_io_function, enableVal) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	} 

	Status LSM6DSOX_MachineLearningCore::disable(){
		uint8_t disableVal = 0;
		
		if (lsm6dsox_mlc_set(&_register_io_function, disableVal) !=
			(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}
		return Status::OK;
	}

    /**
	 * @brief  Read component ID
	 * @param  id the WHO_AM_I value
	 * @retval 0 in case of success, an error code otherwise
	 */
	//(sizeof(ucfConfig) / sizeof(ucf_line_t) -> number of lines
    Status LSM6DSOX_MachineLearningCore::setDecisionTrees(const ucf_line_t ucfConfig[], uint32_t numLines) {
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
     * Extract from datasheet
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
	 * @brief  Get the data rate of the component
	 * @param  data_rate is the data rate (in Hz) container
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
			default:
				ret = Status::ERROR;
				break;
		}
		return ret;
	}

	
	/**
	 * @brief  Get data from all trees
	 * @param  data is an array containing the values for the 8 trees
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getData(std::array<uint8_t, 8> &data) {
        uint8_t buff[8];
        MachineLearningCoreData mlc	 = {{0}};
        if( lsm6dsox_mlc_out_get(&_register_io_function, buff) != 
        	(int32_t)Communication::Status::OK) {
			return Status::ERROR;
		}

        for (int i = 0; i< 8; ++i)
        {
            mlc.data[i] = buff[i];
        }

        data = mlc.data;
		return Status::OK;
	}

	/**
	 * @brief  Get the pin of component interrupt on MCU
	 * @param  Numero of the interrupt pin (1 or 2)
	 * @retval Pin of component interrupt on MCU
	 */
	PinName LSM6DSOX_MachineLearningCore::getInterruptPin(InterruptNumber intNum) {
		 if( intNum == InterruptNumber::_INT1  )  return _mcu_pin_interrupt1; 
		 else return _mcu_pin_interrupt2;
	}


	
	/**
	 * @brief  Enable interrupt from component
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::enableInterrupt() {
		_lsm6dsox_interrupt.enable_irq();
		return Status::OK;
	}

	/**
	 * @brief  Disable interrupt from component
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::disableInterrupt() {
		_lsm6dsox_interrupt.disable_irq();
		return Status::OK;
	}


		
	/**
	 * @brief  Enable interrupt from a given tree on INT1, INT2 or both
	 * 
	 * This function could be made simpler by only writing to the registers MLC_INT1(0Dh) and MLC_INT2(11h)
	 * but this feature is not implemented in the driver functions, so we would need to manage writing to registers in a "low level" way
	 * 
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::enableTreeInterrupt(MachineLearningCoreTree tree, TreeInterruptNum intNum) {

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
			case MachineLearningCoreTree::_TREE_1:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc1 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc1 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_2:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc2 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc2 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_3:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc3 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc3 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_4:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc4 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc4 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_5:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc5 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc5 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_6:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc6 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc6 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_7:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc7 = PROPERTY_ENABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc7 = PROPERTY_ENABLE;
			break;
			case MachineLearningCoreTree::_TREE_8:
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
	 * @brief  Disable interrupt from a given tree on INT1, INT2 or both
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::disableTreeInterrupt(MachineLearningCoreTree tree, TreeInterruptNum intNum) {
		
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
			case MachineLearningCoreTree::_TREE_1:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc1 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc1 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_2:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc2 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc2 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_3:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc3 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc3 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_4:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc4 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc4 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_5:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc5 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc5 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_6:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc6 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc6 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_7:
				if(intNum == TreeInterruptNum::_INT1 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int1_route.mlc7 = PROPERTY_DISABLE;
				if(intNum == TreeInterruptNum::_INT2 || intNum == TreeInterruptNum::_INT1_AND_INT2) pin_int2_route.mlc7 = PROPERTY_DISABLE;
			break;
			case MachineLearningCoreTree::_TREE_8:
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

	Status LSM6DSOX_MachineLearningCore::getTreeInterrupt(MachineLearningCoreTree tree, TreeInterruptNum &intNum)
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
			case MachineLearningCoreTree::_TREE_1:
				if( pin_int1_route.mlc1 == PROPERTY_ENABLE && pin_int2_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc1 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_2:
				if( pin_int1_route.mlc2 == PROPERTY_ENABLE && pin_int2_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc2 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_3:
				if( pin_int1_route.mlc3 == PROPERTY_ENABLE && pin_int2_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc3 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_4:
				if( pin_int1_route.mlc4 == PROPERTY_ENABLE && pin_int2_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc4 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_5:
				if( pin_int1_route.mlc5 == PROPERTY_ENABLE && pin_int2_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc5 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_6:
				if( pin_int1_route.mlc6 == PROPERTY_ENABLE && pin_int2_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc6 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_7:
				if( pin_int1_route.mlc7 == PROPERTY_ENABLE && pin_int2_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc7 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
			case MachineLearningCoreTree::_TREE_8:
				if( pin_int1_route.mlc8 == PROPERTY_ENABLE && pin_int2_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1_AND_INT2;
				else if(pin_int1_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT1;
				else if(pin_int2_route.mlc8 == PROPERTY_ENABLE) iNum = TreeInterruptNum::_INT2;
			break;
		}

		intNum = iNum;

		return Status::OK;
	}



	/**
	 * @brief  Read interrupt from component
	 * @param  interrupt_status contain the value on pin interrupt of component
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::readInterrupt(uint8_t &interrupt_status) {
		interrupt_status = _lsm6dsox_interrupt;
		return Status::OK;
	}

	/**
	 * @brief  Attaching an interrupt handler to the component interrupt
	 * @param  func An interrupt handler.
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::attachInterrupt(Callback<void()> func) {
		_lsm6dsox_interrupt.rise(func);
		return Status::OK;
	}

	/**
	 * @brief  Get status of events on component
	 * @param  component_events array of events available. Check AccelerometerComponentEvents for
	 * more details
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getEventStatus(std::array<uint8_t, 16> &component_events) {
		//TODO
        //Need change       
        // AccelerometerEvents xl;
		//lsm6dsox_status_t events;

		// if (lsm6dsox_status_get(&_register_io_function, NULL, &events) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }

		// xl.event.data_ready = events.drdy_xl;

		// component_events = xl.component_events;
		return Status::OK;
	}

	/**
	 * @brief  Set events on interrupt of component
	 * @param  component_events_on_interrupt array of events on interrupt available. Check
	 * AccelerometerComponentEventsOnInterrupt for more details
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::setEventsOnInterrupt(std::array<uint8_t, 16> &component_events_on_interrupt){

        //TODO
        //Need change 
		// std::array<uint8_t, 16> &component_events_on_interrupt) {
		// AccelerometerEventsOnInterrupt xl = {{component_events_on_interrupt}};

		// lsm6dsox_pin_int1_route_t interrupts_1;
		// if (lsm6dsox_pin_int1_route_get(&_register_io_function, &interrupts_1) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }
		// interrupts_1.drdy_xl = xl.event_on_interrupt.data_ready;
		// if (lsm6dsox_pin_int1_route_set(&_register_io_function, interrupts_1) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }

		// lsm6dsox_pin_int2_route_t interrupts_2;
		// if (lsm6dsox_pin_int2_route_get(&_register_io_function, NULL, &interrupts_2) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }
		// interrupts_2.drdy_xl = xl.event_on_interrupt.data_ready;
		// if (lsm6dsox_pin_int2_route_set(&_register_io_function, NULL, interrupts_2) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }

		return Status::OK;
	}

	/**
	 * @brief  Get events on interrupt of component
	 * @param  component_events_on_interrupt array of events on interrupt available. Check
	 * AccelerometerComponentEventsOnInterrupt for more details
	 * @retval 0 in case of success, an error code otherwise
	 */
	Status LSM6DSOX_MachineLearningCore::getEventsOnInterrupt(std::array<uint8_t, 16> &component_events_on_interrupt){
		//TODO
        //Need change 
        // std::array<uint8_t, 16> &component_events_on_interrupt) {
		// AccelerometerEventsOnInterrupt xl;

		// lsm6dsox_pin_int1_route_t interrupts_1;
		// if (lsm6dsox_pin_int1_route_get(&_register_io_function, &interrupts_1) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }

		// lsm6dsox_pin_int2_route_t interrupts_2;
		// if (lsm6dsox_pin_int2_route_get(&_register_io_function, NULL, &interrupts_2) !=
		// 	(int32_t)Communication::Status::OK) {
		// 	return Status::ERROR;
		// }

		// xl.event_on_interrupt.data_ready = (interrupts_1.drdy_xl | interrupts_2.drdy_xl);

		// component_events_on_interrupt = xl.component_events_on_interrupt;
		return Status::OK;
	}

	/**
	 * @brief  Function used by driver to write data.
	 * @param  handle current object
	 * @param  write_address address of the register to write on
	 * @param  p_buffer contains data to write
	 * @param  number_bytes_to_write number of data to write in bytes
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_MachineLearningCore::ptr_io_write(void *handle, uint8_t write_address,
												 uint8_t *p_buffer,
												 uint16_t number_bytes_to_write) {
		return (int32_t)((LSM6DSOX_MachineLearningCore *)handle)
			->_lsm6dsox_component_i2c.write(write_address, number_bytes_to_write, p_buffer);
	}

	/**
	 * @brief  Function used by driver to read data.
	 * @param  handle current object
	 * @param  read_address address of the register to read on
	 * @param  p_buffer contains data to read
	 * @param  number_bytes_to_write number of data to read in bytes
	 * @retval 0 in case of success, an error code otherwise
	 */
	int32_t LSM6DSOX_MachineLearningCore::ptr_io_read(void *handle, uint8_t read_address,
												uint8_t *p_buffer, uint16_t number_bytes_to_read) {
		return (int32_t)((LSM6DSOX_MachineLearningCore *)handle)
			->_lsm6dsox_component_i2c.read(read_address, number_bytes_to_read, p_buffer);
	}
}	// namespace MachineLearningCore
