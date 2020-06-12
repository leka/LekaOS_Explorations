/**
******************************************************************************
* @file    MachineLearningCoreBase.h
* @author  Maxime Blanc and Samuel Hadjes
* @brief   Abstract base class for a machine learning core
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __MACHINELEARNINGCOREBASE_H__
#define __MACHINELEARNINGCOREBASE_H__

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"

namespace MachineLearningCore {
	/* Defines -------------------------------------------------------------------*/
	/* Enums ---------------------------------------------------------------------*/
	enum class Status : uint8_t {
		OK			  = 0,
		ERROR		  = 1,
		NOT_AVAILABLE = 2,
	};


	/* Structs -------------------------------------------------------------------*/
    /** Common data block definition **/
    typedef struct {
    uint8_t address;
    uint8_t data;
    } ucf_line_t;

    
	/* Unions --------------------------------------------------------------------*/
	/* Class Declaration ---------------------------------------------------------*/
	/**
	 * Abstract base class for any component.
	 */
	class MachineLearningCoreBase {
	  public:
		virtual ~MachineLearningCoreBase() {}
		virtual Status init()			  = 0;

		virtual Status enable()			  = 0;
		virtual Status disable()		  = 0;		

        virtual Status setDecisionTrees(const ucf_line_t ucfConfig[], uint32_t numLines)  = 0;
		virtual Status getID(uint8_t &id) = 0;



        // The data is of a specific size, which works in our case but might need to be abstracted in case of different MLC usage
		virtual Status getData(std::array<uint8_t, 8> &data)	= 0;

		// virtual  Status setPinInterrupt(PinName pin_interrupt)	  = 0;
		//virtual PinName getInterruptPin()						= 0;
		virtual Status enableInterrupt()						= 0;
		virtual Status disableInterrupt()						= 0;
		virtual Status readInterrupt(uint8_t &interrupt_status) = 0;
		virtual Status attachInterrupt(Callback<void()> func)	= 0;

		// Some component can generate more than one event, component_events list them.
		virtual Status getEventStatus(std::array<uint8_t, 16> &component_events) = 0;

		// Some component allows events to trigger the interrupt, component_events_on_interrupt list
		// them.
		virtual Status setEventsOnInterrupt(
			std::array<uint8_t, 16> &component_events_on_interrupt) = 0;
		virtual Status getEventsOnInterrupt(
			std::array<uint8_t, 16> &component_events_on_interrupt) = 0;

		// TO DO add offset
	};
}	// namespace Component

#endif	 // __MACHINELEARNINGCOREBASE__H__