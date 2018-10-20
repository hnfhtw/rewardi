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
	httpServer.start(80, false);
	httpServer.addPathHandler("POST", "/ESP32/WiFi", handle_REST_WiFi);
}

