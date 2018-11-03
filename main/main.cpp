#include <esp_log.h>
#include <string>
#include "sdkconfig.h"
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <HttpServer.h>
#include <RESTClient.h>
#include <WebServer.h>
#include <BootWiFi.h>
#include <JSON.h>

static const char* LOG_TAG = "WiFi";

extern "C" {
	void app_main(void);
}

WiFi wifi;
HttpServer httpServer;
BootWiFi bootWifi;
RESTClient restClient;
Socket backendSocket;

static const std::string websocketHandshakeRequest = "GET /echo HTTP/1.1\r\n"
		"Host: demos.kaazing.com:443\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";

static const std::string testRequest = "GET /get HTTP/1.1\r\n"
		"Host: postman-echo.com:443\r\n"
		"Connection: keep-alive\r\n"
		"\r\n";

void sendEncodedData(char *str, uint8_t opcode, Socket* socket);

static void handle_REST_WiFi(HttpRequest *pRequest, HttpResponse *pResponse) {
	// Command from App received -> handle it here
	ESP_LOGD(LOG_TAG, " REST handler called!");
	pResponse->sendData("Hello World!");

	// Test of HTTP client - connect to a test backend server if "hello":"world" (key:value) is received
	/*std::map<std::string, std::string> queryMap;
	queryMap = pRequest->getQuery();
	if(queryMap["hello"].find("world")){
		//clientSocket.connect("http://9667db8a-0f38-4c5e-a568-c8786e66a71c.mock.pstmn.io/rewardi/", 80);
		//HttpRequest clientRequest(clientSocket.getFD());
		restClient.setURL("http://9667db8a-0f38-4c5e-a568-c8786e66a71c.mock.pstmn.io/rewardi/");
		uint32_t responseCode = (uint32_t) restClient.get();
		ESP_LOGD(LOG_TAG, " REST client answer code = %d", responseCode);
		ESP_LOGD(LOG_TAG, " REST client answer = %s", restClient.getResponse().c_str());
	}*/

	// Parse JSON object from received string
	JsonObject obj = JSON::parseObject(pRequest->getBody());
	ESP_LOGD(LOG_TAG, " Received msgID = %d", obj.getInt("msgID"));
	ESP_LOGD(LOG_TAG, " Received maxTime = %d", obj.getObject("msgData").getInt("maxTime"));

	// Send a JSON object to simulated backend (object is then echoed back)
	JsonObject responseObj = JSON::createObject();
	responseObj.setInt("msgID", 106);
	JsonObject responseObjData = JSON::createObject();
	responseObjData.setDouble("onTime", 2.15);
	responseObjData.setDouble("deltaTime", 2.16);
	responseObjData.setBoolean("isTimeout", false);
	responseObj.setObject("msgData", responseObjData);

	//restClient.setURL("http://5ede5533-a99a-4a29-a11d-e41eb9678242.mock.pstmn.io/rewardiPOST");		// HN-CHECK this or the next line blocks if URL is not reachable!!
	restClient.setURL("http://postman-echo.com/post");
	uint32_t responseCode = (uint32_t) restClient.post(responseObj.toString());
	ESP_LOGD(LOG_TAG, " REST client answer code = %d", responseCode);
	ESP_LOGD(LOG_TAG, " REST client answer = %s", restClient.getResponse().c_str());
}

void app_main(void)
{
	// Boot WiFi - ESP32 acts as an AP if no SSID and PW is set
	// User then can connect to ESP32 WiFi and set SSID and PW via browser at 192.168.4.1
	// SSID and PW are then stored in flash and used for connection to actual AP
	// Note: to set a new SSID/PW either a long button press (box) or a special command (box/socket)
	// has to be implemented
	bootWifi.boot();

	// Start HTTP server to receive commands from App
	//httpServer.start(80, false);
	//httpServer.addPathHandler("POST", "/ESP32/WiFi", handle_REST_WiFi);

	// Create TCP socket for backend connection
	backendSocket.createSocket(false);
	char* addr = "34.209.17.111";		 // websocket echo
	//char* addr = "34.235.212.72";		 // postman echo -  for HTTPS test
	backendSocket.setSSL(true);
	backendSocket.connect(addr, 443);
	backendSocket.sslHandshake();

	/*backendSocket.send(testRequest);
	ESP_LOGD(LOG_TAG, " Data sent!");
	uint8_t data[1024];
	vTaskDelay(1000);
	backendSocket.receive(data, 64, false);
	ESP_LOGD(LOG_TAG, " Test answer = %s", data);*/

	ESP_LOGD(LOG_TAG, " Websocket connected - SSL handshake done");
	ESP_LOGD(LOG_TAG, " Websocket handshake = %s", websocketHandshakeRequest.c_str());
	backendSocket.send(websocketHandshakeRequest);
	vTaskDelay(500);
	uint8_t data[1024];
	backendSocket.receive(data, 1024, false);
	data[1023] = '\0';
	ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);
	//backendSocket.send("hello world!");
	char* test = "this is test one";
	char* test2 = "{\"msgID\": 211,\"msgData\":{\"rewardiPerOpen\": 4,\"boxName\": \"Max's box\",\"ownerUserID\": 47,\"isLocked\": false}}";
	uint8_t cnt = 0;

	JsonObject sendObj = JSON::createObject();
	sendObj.setInt("msgID", 106);
	JsonObject sendObjData = JSON::createObject();
	sendObjData.setDouble("onTime", 2.15);
	sendObjData.setDouble("deltaTime", 2.16);
	sendObjData.setBoolean("isTimeout", false);
	sendObj.setObject("msgData", sendObjData);

	while(true){
		++cnt;
		if(cnt == 255)
			cnt = 0;
		if(cnt%2 == 0){
			sendEncodedData(test, 1, &backendSocket);
			backendSocket.receive(data, 128, false);
			data[strlen(test)+2] = '\0';
		}
		else{
			sendEncodedData((char*) (sendObj.toString().c_str()), 1, &backendSocket);
			backendSocket.receive(data, 128, false);
			// HN-TODO - here the Websocket header needs to be checked and then the payload extracted.
			// Afterwards a JSON object can be parsed from the payload.
			JsonObject receivedObj = JSON::parseObject((const char*) &data[2]);
			ESP_LOGD(LOG_TAG, " Received msgID = %d", receivedObj.getInt("msgID"));
			data[strlen(test2)+2] = '\0';
		}
		ESP_LOGD(LOG_TAG, " Bit 0 = %d", data[0]);
		ESP_LOGD(LOG_TAG, " Bit 1 = %d", data[1]);
		ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);
		vTaskDelay(200);
	}
}

// WebSocket protocol constants
// First byte
#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
#define RAND_MAX 256

void sendEncodedData(char *str, uint8_t opcode, Socket* socket) {
    uint8_t mask[4];
    uint8_t frameBuffer[128];
    size_t bufferIndex = 0;
    size_t bytesWritten = 0;
    int size = strlen(str);

    // Opcode; final fragment
    frameBuffer[bufferIndex] = (opcode | WS_FIN);
    bufferIndex++;

    // NOTE: no support for > 16-bit sized messages
    if (size > 125) {
        frameBuffer[bufferIndex] = (WS_SIZE16 | WS_MASK);
        frameBuffer[bufferIndex + 1] = ((uint8_t) (size >> 8));
        frameBuffer[bufferIndex + 2] = ((uint8_t) (size & 0xFF));
        bufferIndex += 3;
    } else {
        frameBuffer[bufferIndex] = ((uint8_t) size | WS_MASK);
        bufferIndex++;
    }

    mask[0] = rand();
    mask[1] = rand();
    mask[2] = rand();
    mask[3] = rand();

    frameBuffer[bufferIndex] = (mask[0]);
    frameBuffer[bufferIndex + 1] = (mask[1]);
    frameBuffer[bufferIndex + 2] = (mask[2]);
    frameBuffer[bufferIndex + 3] = (mask[3]);
    bufferIndex += 4;

    for (int i=0; i<size; ++i) {
        frameBuffer[bufferIndex] = (str[i] ^ mask[i % 4]);
        bufferIndex++;
        if (bufferIndex == 128) {
            bytesWritten += socket->send(frameBuffer, bufferIndex);
            bufferIndex = 0;
        }
    }
    if (bufferIndex > 0) {
        bytesWritten += socket->send(frameBuffer, bufferIndex);
    }
}


