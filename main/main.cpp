#include <esp_log.h>
#include <string>
#include "sdkconfig.h"
#include <BootWiFi.h>
#include <freertos/timers.h>
#include "CommHandler.h"
#include "SysControl.h"
#include "LockDriver.h"
#include "StandbyControl.h"
#include "RelaisDriver.h"
#include "nvs_flash.h"

//const char* DEVICE_DATA_PARTITION = "device_data\0";
#define DEVICE_DATA_PARTITION       "device_data"
#define DEVICE_DATA_NAMESPACE       "device_data"
#define DEVICE_DATA_KEY_TRUSTNUMBER "trust_number"
#define DEVICE_DATA_SIZE_TRUSTNUMBER    37      // 36 characters + termination '\0'
// // device_data.bin file generieren mit trust_number aus generate_device_data.csv geht nur mit Python 2.x!!
// 1) install esptool:  pip install esptool
// 2) flash device data from terminal with: esptool.py --port COM5 write_flash 0x210000 C:/FHV/Code/ESP32/msys32/home/Harald.Netzer/esp/rewardi/build/device_data.bin

static const char* LOG_TAG = "app_main";
char trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER+1];

extern "C" {
	void app_main(void);
}

BootWiFi bootWifi;
Socket backendSocket;
SocketBoard socketBoard;
SysControl sysControl;
CommHandler commHandler;
Box box;

RelaisDriver relaisDriver;

TimerHandle_t hSysTick = NULL;      // 10ms SysTick (RTOS software timer) handle
uint32_t sysTick;                   // 10ms SysTick variable
LockDriver lockDriver;
StandbyControl standbyControl(GPIO_NUM_0);

static const std::string websocketHandshakeRequest = "GET /ws/sockets/5A43FE9E-F138-4856-BB09-33FC6925EA6D HTTP/1.1\r\n"     // HN-Original "GET /echo HTTP/1.1\r\n"
        "Host: 37.60.168.102:443\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n";

static bool read_device_data(char* trustNumber);

void sysTick_notify(void){
    ++sysTick;
}

void app_main(void)
{
    trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER] = '\0';
    read_device_data(trustNumber);
    ESP_LOGD(LOG_TAG, " Trust number from flash = %s", trustNumber);

    /*hSysTick = xTimerCreate("SysTick",  pdMS_TO_TICKS(10), pdTRUE, NULL, (TimerCallbackFunction_t) &sysTick_notify);    // create and initialize 10ms SysTick
    xTimerStart(hSysTick, 0);   // start 10ms SysTick

    // Boot WiFi - ESP32 acts as an AP if no SSID and PW is set
    // User then can connect to ESP32 WiFi and set SSID and PW via browser at 192.168.4.1
    // SSID and PW are then stored in flash and used for connection to actual AP
    // Note: to set a new SSID/PW either a long button press (box) or a special command (box/socket)
    // has to be implemented
    bootWifi.boot();
    //sysControl.setBackendURL(bootWifi.getBackendUrl());
    sysControl.setBackendURL("37.60.168.102");

    backendSocket.createSocket(false);  // Create TCP socket for backend connection
    backendSocket.setSSL(true);         // initialize SSL (mbedTLS stack)
    backendSocket.connect((char*) sysControl.getBackendURL().c_str(), 443);     // establish TCP connection to backend
    backendSocket.sslHandshake();       // perform SSL handshake to establish SSL connection
        ESP_LOGD(LOG_TAG, " Websocket connected - SSL handshake done");     // HN-CHECK DEBUG
        ESP_LOGD(LOG_TAG, " Websocket handshake = %s", websocketHandshakeRequest.c_str());  // HN-CHECK DEBUG
    backendSocket.send(websocketHandshakeRequest);  // send websocket handshake request -> this requests a protocol change from HTTP to Websocket
    vTaskDelay(500);
    uint8_t data[1024];
    backendSocket.receive(data, 1024, false);       // receive websocket handshake response
    data[1023] = '\0';
        ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);  // HN-CHECK DEBUG

    relaisDriver.setPin(GPIO_NUM_32);
    relaisDriver.init();
    socketBoard.setRelaisDriver(&relaisDriver);
    socketBoard.init();
    //sysControl.setCommHandler(&commHandler);
    commHandler.setBox(&box);
    commHandler.setSocketBoard(&socketBoard);
    commHandler.setSocket(&backendSocket);
    //commHandler.setSysControl(&sysControl);


    //commHandler.sendLogin();
    //vTaskDelay(500);
    //commHandler.receiveData();
    commHandler.start();

    //vTaskDelay(60000);
*/
    //commHandler.closeWebsocket();
    // 10ms SysTick test - START
    /*uint32_t sysTickOld = 0;
    while(true){
        if(sysTick != sysTickOld){
            ESP_LOGD(LOG_TAG, "SysTick = %d", sysTick);
            sysTickOld = sysTick;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }*/
    // 10ms SysTick test - END

    // test lock driver - START
    //lockDriver.setPin(GPIO_NUM_1);
    //lockDriver.init();
    //lockDriver.switchOn();
    // test lock driver - END

    // test standby - START
    //standbyControl.init();
    //standbyControl.enterSleepMode();
    // test standby - END
}

static bool read_device_data(char* trustNumber)
{
    bool rtnVal = false;

    esp_err_t err = nvs_flash_init_partition(DEVICE_DATA_PARTITION);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_flash_init_partition() error = %d", err);
        return rtnVal;
    }

    nvs_handle hNVS;
    err = nvs_open_from_partition(DEVICE_DATA_PARTITION, DEVICE_DATA_NAMESPACE, NVS_READWRITE, &hNVS);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_open_from_partition() error = %d", err);
        return rtnVal;
    }

    size_t length = DEVICE_DATA_SIZE_TRUSTNUMBER;
    err = nvs_get_str(hNVS, DEVICE_DATA_KEY_TRUSTNUMBER, trustNumber, &length);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_str() error = %d", err);
        return rtnVal;
    }

    nvs_close(hNVS);

    return rtnVal;
}

