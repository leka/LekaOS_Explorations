// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_WIFI_H_
#define _LEKA_OS_LIB_WIFI_H_

#include <chrono>

#include "ESP8266Interface.h"
#include "connectivity/netsocket/include/netsocket/TCPSocket.h"
#include "drivers/DigitalOut.h"
#include "rtos/Thread.h"

class Wifi
{
  public:
	Wifi();
	~Wifi() {};

	void start(void);
	void connect_network(const char *network_name, const char *network_password);

	static const char *sec2str(nsapi_security_t security);
	void scan_available_networks(WiFiInterface *wifi);
	// void http_demo(NetworkInterface *net);

  private:
	ESP8266Interface _interface;
	mbed::DigitalOut _wifi_reset;
	mbed::DigitalOut _wifi_enable;

	char const *_network_name	  = MBED_CONF_NSAPI_DEFAULT_WIFI_SSID;
	char const *_network_password = MBED_CONF_NSAPI_DEFAULT_WIFI_PASSWORD;
};

#endif
