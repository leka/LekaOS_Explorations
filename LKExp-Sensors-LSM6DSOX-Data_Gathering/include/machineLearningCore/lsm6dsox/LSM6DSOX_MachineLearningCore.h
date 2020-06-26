/**
******************************************************************************
* @file    LSM6DSOX_MachineLearningCore.h
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Implementation of MAchineLearningCoreBase for LSM6DSOX MachineLearningCore
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LSM6DSOX_MACHINELEARNINGCORE_H__
#define __LSM6DSOX_MACHINELEARNINGCORE_H__

/* Includes ------------------------------------------------------------------*/
#include "CommunicationI2CBase.h"
#include "MachineLearningCoreBase.h"
#include "lsm6dsox_reg.h"

namespace MachineLearningCore {
	/* Defines -------------------------------------------------------------------*/
	/* Enums ---------------------------------------------------------------------*/

	/**
	 * @brief Enum of possible interrupt number
	 * 
	 */
	enum class InterruptNumber : uint8_t {
		_INT1	 = 1,
		_INT2	 = 2
	};

	/**
	 * @brief Possible interrupts to route a tree signal to.
	 * 
	 */
	enum class TreeInterruptNum : uint8_t {
		_NC = 0, 			/// Not connected
		_INT1	 = 1,		/// Interrupt INT1
		_INT2	 = 2,		/// Interrupt INT2
		_INT1_AND_INT2 = 3	/// Interrupt INT1 and INT2
	};

	/**
	 * @brief The different trees present on the MLC
	 * 
	 */
	enum class Tree : uint8_t {
		_TREE_1 = 0,
		_TREE_2 = 1,
		_TREE_3 = 2,
		_TREE_4 = 3,
		_TREE_5 = 4,
		_TREE_6 = 5,
		_TREE_7 = 6,
		_TREE_8 = 7,
	};

	/**
	 * @brief Possible behaviors for the MLC trees interrupts
	 * 
	 */
	enum class InterruptBehavior : uint8_t{
		_PULSED            = 0,
		_LATCHED           = 1
	};

	/* Structs -------------------------------------------------------------------*/
	/* Unions --------------------------------------------------------------------*/

	/**
	 * @brief Union of trees and their respective results
	 * 
	 */
	union Data {
		std::array<uint8_t, 8> array;
		struct {
			uint8_t tree1;
			uint8_t tree2;
			uint8_t tree3;
			uint8_t tree4;
			uint8_t tree5;
			uint8_t tree6;
			uint8_t tree7;
			uint8_t tree8;
		} trees;
	};

	/* Class Declaration ---------------------------------------------------------*/
	/**
	 * @brief Machine Leraning Core class, inherit from MachineLearningCoreBase
	 * 
	 */
	class LSM6DSOX_MachineLearningCore : MachineLearningCoreBase {
	  public:
		LSM6DSOX_MachineLearningCore(Communication::I2CBase &component_i2c, PinName pin_interrupt1, PinName pin_interrupt2);

		virtual Status init();
		virtual Status disableI3C();
		virtual Status getID(uint8_t &id);

		virtual Status restoreDefaultConfiguration();

		//temporary, to simplify developpement
		// TODO erase this once no more in use
		virtual stmdev_ctx_t* TMP_getIoFunc();
		
		virtual Status enable();
		virtual Status disable();

        virtual Status configureMLC(const ucf_line_t ucfConfig[], uint32_t numLines);

		virtual Status setDataRate(float data_rate);
		virtual Status getDataRate(float &data_rate);

		virtual Status getData(std::array<uint8_t, 8> &data);

		virtual PinName getInterruptPin(InterruptNumber intNum);
		virtual Status enableInterrupt();
		virtual Status disableInterrupt();

		virtual Status setInterruptBehavior(InterruptBehavior behavior);

		virtual Status enableTreeInterrupt(Tree tree, TreeInterruptNum intNum);
		virtual Status disableTreeInterrupt(Tree tree, TreeInterruptNum intNum);

		virtual Status getTreeInterrupt(Tree tree, TreeInterruptNum &intNum);
		virtual Status getTreeInterruptStatus(Tree tree, uint8_t &treeStatus);

		virtual Status readInterrupt(uint8_t &interrupt_status, InterruptNumber intNum);
		virtual Status attachInterrupt(Callback<void()> func, InterruptNumber intNum);

		virtual Status allOnInt1Route(bool enable);

		// Some component can generate more than one event, component_events list them.
		virtual Status getEventStatus(std::array<uint8_t, 16> &component_events);

		// Some component allows events to trigger the interrupt, component_events_on_interrupt list
		// them.
		virtual Status setEventsOnInterrupt(std::array<uint8_t, 16> &component_events_on_interrupt);
		virtual Status getEventsOnInterrupt(std::array<uint8_t, 16> &component_events_on_interrupt);

	  protected:
		static int32_t ptr_io_write(void *handle, uint8_t write_address, uint8_t *p_buffer,
									uint16_t number_bytes_to_write);
		static int32_t ptr_io_read(void *handle, uint8_t read_address, uint8_t *p_buffer,
								   uint16_t number_bytes_to_read);

	  private:
		/**
		 * @brief I2C interface 
		 *  
		 */
		Communication::I2CBase &_lsm6dsox_component_i2c;

		/**
		 * @brief In Out function for LSM6DSOX driver functions calls
		 * 
		 */
		stmdev_ctx_t _register_io_function;
		
		/**
		 * @brief Pin corresponding to INT1 on our board
		 * 
		 */
		PinName _mcu_pin_interrupt1;

		/**
		 * @brief Pin corresponding to INT2 on our board
		 * 
		 */
		PinName _mcu_pin_interrupt2;

		/**
		 * @brief Interrupt object linked to INT1 pin
		 * 
		 */
		InterruptIn _lsm6dsox_interrupt1;

		/**
		 * @brief Interrupt object linked to INT2 pin
		 * 
		 */
		InterruptIn _lsm6dsox_interrupt2;
	};
}	// namespace Component

#endif	 // __LSM6DSOX_MACHINELEARNINGCORE_H__