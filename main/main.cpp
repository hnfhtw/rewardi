#include <esp_log.h>
#include <string>
#include "sdkconfig.h"
#include <BootWiFi.h>
#include "CommHandler.h"

static const char* LOG_TAG = "app_main";

extern "C" {
	void app_main(void);
}

BootWiFi bootWifi;
Socket backendSocket;
Box box;
CommHandler commHandler;

static const std::string websocketHandshakeRequest = "GET /echo HTTP/1.1\r\n"
		"Host: demos.kaazing.com:443\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";

void app_main(void)
{
	// Boot WiFi - ESP32 acts as an AP if no SSID and PW is set
	// User then can connect to ESP32 WiFi and set SSID and PW via browser at 192.168.4.1
	// SSID and PW are then stored in flash and used for connection to actual AP
	// Note: to set a new SSID/PW either a long button press (box) or a special command (box/socket)
	// has to be implemented
	bootWifi.boot();

	// Create TCP socket for backend connection
	backendSocket.createSocket(false);
	char* addr = "34.209.17.111";		 // websocket echo
	backendSocket.setSSL(true);
	backendSocket.connect(addr, 443);
	backendSocket.sslHandshake();
	ESP_LOGD(LOG_TAG, " Websocket connected - SSL handshake done");
	ESP_LOGD(LOG_TAG, " Websocket handshake = %s", websocketHandshakeRequest.c_str());
	backendSocket.send(websocketHandshakeRequest);
	vTaskDelay(500);
	uint8_t data[1024];
	backendSocket.receive(data, 1024, false);
	data[1023] = '\0';
	ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);

	commHandler.setBox(&box);
	commHandler.setSocket(&backendSocket);

	commHandler.sendLogin();
	vTaskDelay(500);
	commHandler.receiveData();
}


