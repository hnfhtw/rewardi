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
#define DEVICE_DATA_KEY_BACKENDURL  "backend_url"
#define DEVICE_DATA_SIZE_TRUSTNUMBER    37      // 36 characters + termination '\0'
#define DEVICE_DATA_SIZE_BACKENDURL     64

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
StandbyControl standbyControl(GPIO_NUM_0);

static bool read_device_data(char* trustNumber, char* backendURL);

void sysTick_notify(void){
    ++sysTick;
}

void app_main(void)
{
    char trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER+1];
    char backendURL[DEVICE_DATA_SIZE_BACKENDURL+1];

    trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER] = '\0';
    backendURL[DEVICE_DATA_SIZE_BACKENDURL] = '\0';

    read_device_data(trustNumber, backendURL);
    ESP_LOGD(LOG_TAG, "Trust number from flash = %s", trustNumber);
    ESP_LOGD(LOG_TAG, "Backend URL from flash = %s", backendURL);

    SysControl* pSysControl = new SysControl();
    pSysControl->setTrustNumber(trustNumber);
    pSysControl->setBackendURL(backendURL);

    CommHandler* pCommHandler = new CommHandler();

    SocketBoard* pSocketBoard = nullptr;
    Box* pBox = nullptr;

    if(trustNumber[0] == '0'){          // trust number starting with 0 -> device is a SocketBoard
        pSocketBoard = new SocketBoard(GPIO_NUM_32);
        pSocketBoard->init();
        pSocketBoard->setCommHandler(pCommHandler);
        pCommHandler->setSocketBoard(pSocketBoard);
        ESP_LOGD(LOG_TAG, "Device is a SocketBoard, init completed");
    }
    else if(trustNumber[0] == '1'){     // trust number starting with 0 -> device is a Box
        pBox = new Box(GPIO_NUM_12, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_33);
        pBox->init();
        pBox->setCommHandler(pCommHandler);
        pCommHandler->setBox(pBox);
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


    // test standby - START
    //standbyControl.init();
    //standbyControl.enterSleepMode();
    // test standby - END
}

static bool read_device_data(char* trustNumber, char* backendURL){
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

    length = DEVICE_DATA_SIZE_BACKENDURL;
    err = nvs_get_str(hNVS, DEVICE_DATA_KEY_BACKENDURL, backendURL, &length);
    if (err != ESP_OK){
        ESP_LOGD(LOG_TAG, " nvs_get_str() error = %d", err);
        return rtnVal;
    }

    nvs_close(hNVS);

    return rtnVal;
}

