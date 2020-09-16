#include "LekaWifi.h"




LekaWifi::LekaWifi(PinName pin_enable, PinName pin_reset)
	: _pin_enable(pin_enable), _pin_reset(pin_reset) {}


const char *LekaWifi::sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}


void LekaWifi::scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("Scan:\r\n");

    int count = wifi->scan(NULL, 0);

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[15];

    count = wifi->scan(ap, count);
    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\r\n", count);

    delete[] ap;
}

void LekaWifi::http_demo(NetworkInterface *net)
{
    // Open a socket on the network interface, and create a TCP connection to mbed.org
    TCPSocket socket;
    socket.open(net);

    SocketAddress a;
    net->gethostbyname("ifconfig.io", &a);
    a.set_port(80);
    socket.connect(a);
    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\n\r\n";
    int scount = socket.send(sbuffer, sizeof sbuffer);
    printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n") - sbuffer, sbuffer);

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") - rbuffer, rbuffer);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}

void LekaWifi::testATCmdParser(){
    /* Test based on ATCmdParser example on os.mbed.com */

    BufferedSerial LEKA_WIFI_INTERFACE(PIN_USART2_ESP_TX, PIN_USART2_ESP_RX, MBED_CONF_ESP8266_SERIAL_BAUDRATE);
    ATCmdParser LEKA_WIFI(&LEKA_WIFI_INTERFACE);
    LEKA_WIFI.debug_on(1);
    LEKA_WIFI.set_delimiter( "\r\n" );
    //Now get the FW version number of ESP8266 by sending an AT command
    printf("\nATCmdParser: Retrieving FW version");
    LEKA_WIFI.send("AT+GMR"); //Previous try: "AT+CWMODE=3"
    int version;
    if(LEKA_WIFI.recv("SDK version:%d", &version) && LEKA_WIFI.recv("OK")) {
        printf("\nATCmdParser: FW version: %d", version);
        printf("\nATCmdParser: Retrieving FW version success");
    } else {
        printf("\nATCmdParser: Retrieving FW version failed");
        return;
    }
    printf("\nDone\n");
    ThisThread::sleep_for(1s);
}

void LekaWifi::testWifiAPI(){
    /* Test based on Wi-Fi example on os.mbed.com */

    SocketAddress a;

    printf("WiFi example\r\n\r\n");

    scan_demo(&wifi);

    printf("\r\nConnecting...\r\n");
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\r\nConnection error\r\n");
		return;
    }

    printf("Success\r\n\r\n");
    printf("MAC: %s\r\n", wifi.get_mac_address());
    wifi.get_ip_address(&a);
    printf("IP: %s\r\n", a.get_ip_address());
    wifi.get_netmask(&a);
    printf("Netmask: %s\r\n", a.get_ip_address());
    wifi.get_gateway(&a);
    printf("Gateway: %s\r\n", a.get_ip_address());
    printf("RSSI: %d\r\n\r\n", wifi.get_rssi());

    http_demo(&wifi);

    wifi.disconnect();

}

void LekaWifi::init(){
	DigitalOut enable(_pin_enable, 0);
	DigitalOut reset(_pin_reset, 0);
	enable = 1;
	reset = 0;
	ThisThread::sleep_for(1s);
	reset = 1;
}


void LekaWifi::runTest() {
	printf("\nTest of wifi!\n");

    init();

    testATCmdParser();
    // testWifiAPI();

    printf("\r\nDone\r\n");

	printf("\n");
}
