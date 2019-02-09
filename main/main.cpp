/********************************************************************************************
 * Project    : Rewardi
 * Created on : 09/2018 - 01/2019
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : main.cpp
 * Purpose    : Rewardi ESP32 firmware main entry point - called by FreeRTOS
 ********************************************************************************************/

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
#include "messageIDs.h"

/********************************************************************************************
 * Please note:  Before flashing this Rewardi ESP32 firmware you have to flash build/device_data_box.bin or
 *               build/device_data_socket.bin to the ESP32 address 0x210000 - this device data tells the firmware
 *               if the device is a Box or a SocketBoard. The device data contains the backend URL, the trust number
 *               (that identifies the device as a Box or SocketBoarD) and the 5 last owners + box codes in case of a Box.
 *
 * How to generate and flash the device data:
 *               1) Modify either generate_box_device_data.csv or generate_socket_device_data.csv
 *               2) Use Python 2.x and install esptool:     pip install esptool
 *               3) Generate the device data binary file with Python script nvs_partition_gen.py in console:
 *                  nvs_partition_gen.py generate_socket_device_data.csv device_data_socket.bin
 *               3) Flash device data from terminal with esptool:
 *                  esptool.py --port COM5 write_flash 0x210000 device_data_socket.bin
 *
 ********************************************************************************************/

static const char* LOG_TAG = "app_main";

extern "C" {
	void app_main(void);
}

BootWiFi bootWifi;

void app_main(void)
{
    SysControl* pSysControl = new SysControl();     // create SysControl object (common functionality of SocketBoard and Box)
    pSysControl->init();                            // init SysControl -> get device data (trust number and backend URL) from Flash

    CommHandler* pCommHandler = new CommHandler();  // create CommHandler object (responsible for communication with Rewardi server)

    SocketBoard* pSocketBoard = nullptr;
    Box* pBox = nullptr;

    // Based on the unique trust number read from Flash it is determined here if the device is a Box or a SocketBoard
    if(pSysControl->getTrustNumber().c_str()[0] == '2'){    // trust number starting with 2 -> device is a SocketBoard
        pSocketBoard = new SocketBoard(GPIO_NUM_4);         // create SocketBoard object
        pSocketBoard->init();                               // initialize it
        pSocketBoard->setCommHandler(pCommHandler);         // give SocketBoard object a pointer to CommHandler object
        pCommHandler->setSocketBoard(pSocketBoard);         // and vice versa
        pSysControl->setDeviceType(SysControl::DeviceType::SOCKET);     // tell SysControl object that device is a SocketBoard
        ESP_LOGD(LOG_TAG, "Device is a SocketBoard, init completed");
    }
    else if(pSysControl->getTrustNumber().c_str()[0] == '1'){    // trust number starting with 1 -> device is a Box
        pBox = new Box(GPIO_NUM_13, GPIO_NUM_25, GPIO_NUM_27, GPIO_NUM_12, GPIO_NUM_33);    // create Box object
        pBox->init();                                               // initialize it
        pBox->setCommHandler(pCommHandler);                         // give Box object a pointer to CommHandler object
        pCommHandler->setBox(pBox);                                 // and vice versa
        pSysControl->setDeviceType(SysControl::DeviceType::BOX);    //  tell SysControl object that device is a Box
        pBox->m_pSysControl = pSysControl;
        ESP_LOGD(LOG_TAG, "Device is a Box, init completed");
    }

    // Boot WiFi - ESP32 acts as an AP if no SSID and PW is set, user then can connect to ESP32 WiFi and set SSID and PW via browser at 192.168.4.1
    // SSID and PW are then stored in flash and used for connection to actual AP, to set a new SSID/PW a special command (sent from app->server->esp32) has to be implemented, or a long button press (Box)
    bootWifi.boot();

    // Connect to Rewardi server
    pSysControl->connectToServer();

    // Provide the established network socket to CommHandler and start it
    pCommHandler->setSocket(pSysControl->getSocket());
    pCommHandler->start();

    if(pSysControl->getDeviceType() == SysControl::DeviceType::BOX){
        while(pCommHandler->getIsConnected() == false){ }   // wait here until server connection is established
        pBox->start();  // start Box
    }

    // stay awake until desired by SysControl, afterwards close websocket connection and enter deep sleep mode
    while(pSysControl->getStayAwake()){
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    pCommHandler->stop();
    pSysControl->getSocket()->close();
    vTaskDelay(pdMS_TO_TICKS(2000));
    pSysControl->enterDeepSleepMode();
}

