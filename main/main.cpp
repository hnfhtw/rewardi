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

// // device_data.bin file generieren mit trust_number aus generate_device_data.csv geht nur mit Python 2.x!!
// 1) install esptool:  pip install esptool
// 2) flash device data from terminal with: esptool.py --port COM5 write_flash 0x210000 C:/FHV/Code/ESP32/msys32/home/Harald.Netzer/esp/rewardi/build/device_data.bin

static const char* LOG_TAG = "app_main";

extern "C" {
	void app_main(void);
}

BootWiFi bootWifi;
TimerHandle_t hSysTick = NULL;      // 10ms SysTick (RTOS software timer) handle
uint32_t sysTick;                   // 10ms SysTick variable

void sysTick_notify(void){
    ++sysTick;
}

void app_main(void)
{
    SysControl* pSysControl = new SysControl();
    pSysControl->init();

    CommHandler* pCommHandler = new CommHandler();

    SocketBoard* pSocketBoard = nullptr;
    Box* pBox = nullptr;

    if(pSysControl->getTrustNumber().c_str()[0] == '2'){          // trust number starting with 2 -> device is a SocketBoard
        pSocketBoard = new SocketBoard(GPIO_NUM_32);
        pSocketBoard->init();
        pSocketBoard->setCommHandler(pCommHandler);
        pCommHandler->setSocketBoard(pSocketBoard);
        pSysControl->setDeviceType(SysControl::DeviceType::SOCKET);
        ESP_LOGD(LOG_TAG, "Device is a SocketBoard, init completed");
    }
    else if(pSysControl->getTrustNumber().c_str()[0] == '1'){     // trust number starting with 1 -> device is a Box
        pBox = new Box(GPIO_NUM_13, GPIO_NUM_25, GPIO_NUM_27, GPIO_NUM_12, GPIO_NUM_33);
        pBox->init();
        pBox->setCommHandler(pCommHandler);
        pCommHandler->setBox(pBox);
        pSysControl->setDeviceType(SysControl::DeviceType::BOX);
        pBox->m_pSysControl = pSysControl;          // HN-CHECK DEBUG ONLY - give reference to SysControl
        ESP_LOGD(LOG_TAG, "Device is a Box, init completed");
    }

    // create and initialize 10ms SysTick timer
    hSysTick = xTimerCreate("SysTick",  pdMS_TO_TICKS(10), pdTRUE, NULL, (TimerCallbackFunction_t) &sysTick_notify);
    xTimerStart(hSysTick, 0);   // start 10ms SysTick timer

    // Boot WiFi - ESP32 acts as an AP if no SSID and PW is set, user then can connect to ESP32 WiFi and set SSID and PW via browser at 192.168.4.1
    // SSID and PW are then stored in flash and used for connection to actual AP, to set a new SSID/PW a special command (sent from app->server->esp32) has to be implemented, or a long button press (Box)
    bootWifi.boot();

    // Connect to backend server
    pSysControl->connectToServer();

    // Provide websocket socket to CommHandler and start it
    pCommHandler->setSocket(pSysControl->getSocket());
    pCommHandler->start();

    if(pSysControl->getDeviceType() == SysControl::DeviceType::BOX){
        // Start box
        vTaskDelay(pdMS_TO_TICKS(2000));    // HN-CHECK DEBUG -> wait until session token is received!
        pBox->start();          // HN-CHECK do it when server connection is established!
        pBox->getRgbLedControl()->setPeriod(RgbLedControl::Period::ON);
    }

    // stay awake until desired by SysControl, afterwards close websocket connection and enter deep sleep mode
    while(pSysControl->getStayAwake()){
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    pCommHandler->stop();
    pSysControl->getSocket()->close();
    vTaskDelay(pdMS_TO_TICKS(2000));
    pSysControl->enterDeepSleepMode();
}

