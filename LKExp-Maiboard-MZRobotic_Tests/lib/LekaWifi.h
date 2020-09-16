/**
******************************************************************************
* @file    LekaWifi.h
* @author  Yann Locatelli
* @brief   Wifi operational test code
******************************************************************************
*/

/* Prevent recursive inclusion -----------------------------------------------*/
#ifndef __LEKAWIFI_H__
#define __LEKAWIFI_H__

#include "ATCmdParser.h"
#include "ESP8266Interface.h"
#include "LekaInterfaces.h"
#include "TCPSocket.h"
#include "mbed.h"

class LekaWifi {
  public:
	LekaWifi(PinName pin_enable, PinName pin_reset);
	const char *sec2str(nsapi_security_t sec);

	void scan_demo(WiFiInterface *wifi);
	void http_demo(NetworkInterface *net);
	void init();
	void runTest();
	void testATCmdParser();
	void testWifiAPI();

  private:
	PinName _pin_enable;
	PinName _pin_reset;
};

#endif	 // __LEKAWIFI_H__
