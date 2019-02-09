/********************************************************************************************
 * Project    : Rewardi
 * Created on : 16.11.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : SysControl.cpp
 * Purpose    : Manages behavior (e.g. connect to Rewardi server)
 *              and data that is common to both Rewardi Box and SocketBoard
 ********************************************************************************************/

#include <esp_log.h>
#include "nvs_flash.h"
#include "SysControl.h"

#define DEVICE_DATA_PARTITION       "device_data"
#define DEVICE_DATA_NAMESPACE       "device_data"
#define DEVICE_DATA_KEY_TRUSTNUMBER "trust_number"
#define DEVICE_DATA_KEY_BACKENDURL  "backend_url"
#define DEVICE_DATA_SIZE_TRUSTNUMBER    37      // 36 characters + termination '\0'
#define DEVICE_DATA_SIZE_BACKENDURL     64

static const char* LOG_TAG = "SysControl";

/**
 * @brief SysControl constructor
 */
SysControl::SysControl(){
	m_backendURL  		= "";
	m_trustNumber		= "1234ABCD";
	m_pSocket           = new Socket();
	m_pStandbyControl   = new StandbyControl(GPIO_NUM_33); // GPIO_NUM_33 is a RTC pin, it is used as the Button pin -> necessary to wakeup from deep sleep (note: this pin is not connected in case of a SocketBoard)
	m_pCommHandler      = nullptr;
	m_stayAwake         = true;
	m_deviceType        = SysControl::DeviceType::SOCKET;   // init to SOCKET here -> but will be set later anyway!
}

/**
 * @brief Initialize SysControl - init StandbyControl and get device data (trust number and backend URL) from Flash
 */
void SysControl::init(){
    m_pStandbyControl->init();

    char trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER+1];
    char backendURL[DEVICE_DATA_SIZE_BACKENDURL+1];

    trustNumber[DEVICE_DATA_SIZE_TRUSTNUMBER] = '\0';
    backendURL[DEVICE_DATA_SIZE_BACKENDURL] = '\0';
    readDeviceData(trustNumber, backendURL);
    ESP_LOGD(LOG_TAG, "Trust number from flash = %s", trustNumber);
    ESP_LOGD(LOG_TAG, "Backend URL from flash = %s", backendURL);
    setTrustNumber(trustNumber);
    setBackendURL(backendURL);
}

/**
 * @brief Set backend URL
 */
void SysControl::setBackendURL(std::string backendURL){
	m_backendURL = backendURL;		// use string copy function??
}

/**
 * @brief Set trust number
 */
void SysControl::setTrustNumber(std::string trustNumber){
    m_trustNumber = trustNumber;
}

/**
 * @brief Set pointer to CommHandler
 */
void SysControl::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
}

/**
 * @brief Get backend URL
 */
std::string SysControl::getBackendURL(){
	return m_backendURL;
}

/**
 * @brief Get trust number
 */
std::string SysControl::getTrustNumber(){
	return m_trustNumber;
}

/**
 * @brief Get pointer to CommHandler
 */
CommHandler* SysControl::getCommHandler(){
    return m_pCommHandler;
}

/**
 * @brief Get pointer to TCP socket
 */
Socket* SysControl::getSocket(){
    return m_pSocket;
}

/**
 * @brief Connect to Rewardi server -> connect using SSL -> send Websocket handshake request (including the unique trust number to authenticate)
 */
bool SysControl::connectToServer(){
    std::string websocketHandShakeRequest = "GET /ws/";
    if(getDeviceType() == SysControl::DeviceType::BOX){
        websocketHandShakeRequest += "boxes/";
    }
    else if(getDeviceType() == SysControl::DeviceType::SOCKET){
        websocketHandShakeRequest += "sockets/";
    }
    websocketHandShakeRequest += getTrustNumber();
    websocketHandShakeRequest += " HTTP/1.1\r\nHost: ";
    websocketHandShakeRequest += getBackendURL();
    websocketHandShakeRequest += ":443\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n";

    m_pSocket->createSocket(false);  // create TCP socket for backend server connection
    m_pSocket->setSSL(true);         // initialize SSL (mbedTLS stack)
    m_pSocket->connect((char*) getBackendURL().c_str(), 443);     // establish TCP connection to backend server
    m_pSocket->sslHandshake();       // perform SSL handshake to establish SSL connection
        ESP_LOGD(LOG_TAG, " Websocket connected - SSL handshake done");     // HN-CHECK DEBUG
        ESP_LOGD(LOG_TAG, " Websocket handshake = %s", websocketHandShakeRequest.c_str());  // HN-CHECK DEBUG
    m_pSocket->send(websocketHandShakeRequest);  // send websocket handshake request -> this requests a protocol change from HTTP to Websocket
    vTaskDelay(pdMS_TO_TICKS(500));
    uint8_t data[1024];
    m_pSocket->receive(data, 1024, false);       // receive websocket handshake response
    data[1023] = '\0';
        ESP_LOGD(LOG_TAG, " Websocket answer = %s", data);  // HN-CHECK DEBUG
    return true;
}

/**
 * @brief Enter deep sleep mode
 */
void SysControl::enterDeepSleepMode(){
    m_pStandbyControl->enterSleepMode();
}

/**
 * @brief Read device data from flash (trust number and backend URL)
 */
bool SysControl::readDeviceData(char* trustNumber, char* backendURL){
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

/**
 * @brief Set the device type (tell SysControl if the device is a Box or a SocketBoard)
 */
void SysControl::setDeviceType(SysControl::DeviceType type){
    m_deviceType = type;
}

/**
 * @brief Get device type
 */
SysControl::DeviceType SysControl::getDeviceType(){
    return m_deviceType;
}

/**
 * @brief Set flag that avoids entering deep sleep mode (see main loop)
 */
void SysControl::setStayAwake(bool stayAwake){
    m_stayAwake = stayAwake;
}

/**
 * @brief Get flag that avoids entering deep sleep mode
 */
bool SysControl::getStayAwake(){
    return m_stayAwake;
}
