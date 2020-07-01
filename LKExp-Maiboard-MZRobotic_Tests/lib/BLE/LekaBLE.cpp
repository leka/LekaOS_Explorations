#include "LekaBLE.h"

LekaBLE::LekaBLE(SPI &interface, DigitalOut ble_reset, PinName pin_interrupt)
	: _interface(interface), _bluenrg_reset(ble_reset), _bluenrg_interrupt(pin_interrupt) {}

void LekaBLE::init() {
	BLE &mydevicename = BLE::Instance();
	return;
}

void LekaBLE::getData() { return; }


/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

void LekaBLE::runTest() {
    printf("\nTest of BLE!\n");
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);

    BeaconDemo demo(ble, event_queue);
    demo.start();

    printf("\n");
    return;
}


