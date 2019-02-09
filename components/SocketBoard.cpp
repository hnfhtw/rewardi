/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : SocketBoard.cpp
 * Purpose    : Represents a Rewardi SocketBoard
 ********************************************************************************************/

#include "SocketBoard.h"
#include <map>
#include <esp_log.h>
#include "CommHandler.h"
#include "messageIDs.h"

static const char* LOG_TAG = "SocketBoard";
static std::map<void*, SocketBoard*> socketBoardMap;

/**
 * @brief SocketBoard constructor - needs the output pin where the relais is connected
 */
SocketBoard::SocketBoard(gpio_num_t relaisPin){
	m_socketID	= 0;
	m_maxTime	= 60;
	m_pRelaisDriver = new RelaisDriver(relaisPin);  // create RelaisDriver object
	m_hTimeout = 0;
	m_pCommHandler = nullptr;
}

/**
 * @brief Destructor
 */
SocketBoard::~SocketBoard(){
    ::xTimerDelete(m_hTimeout, portMAX_DELAY);
    socketBoardMap.erase(m_hTimeout);
}

/**
 * @brief Initialize SocketBoard -> init RelaisDriver and create timer that switches off relais automatically after max_time
 *        (if not switched off by the user earlier); map the SocketBoard object with this timer
 */
void SocketBoard::init(){
    m_pRelaisDriver->init();
    m_hTimeout = xTimerCreate("sockMaxTime",  pdMS_TO_TICKS(m_maxTime*1000), pdFALSE, NULL, (TimerCallbackFunction_t) &SocketBoard::timeout);    // create and initialize maxTime timeout timer
    socketBoardMap.insert(std::make_pair(m_hTimeout, this));
}

/**
 * @brief Set ID of the SocketBoard (not used)
 */
void SocketBoard::setSocketID(uint32_t socketID){
	m_socketID = socketID;
}

/**
 * @brief Set max time
 */
void SocketBoard::setMaxTime(uint32_t maxTime){
	m_maxTime = maxTime;
}

/**
 * @brief Set pointer to CommHandler object
 */
void SocketBoard::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
}

/**
 * @brief Get ID of SocketBoard (not used)
 */
uint32_t SocketBoard::getSocketID(){
	return m_socketID;
}

/**
 * @brief Get max time
 */
uint32_t SocketBoard::getMaxTime(){
	return m_maxTime;
}

/**
 * @brief Get pointer to RelaisDriver
 */
RelaisDriver* SocketBoard::getRelaisDriver(){
    return m_pRelaisDriver;
}

/**
 * @brief Switch on SocketBoard (relais) and start timer for auto switch-off after max time
 */
void SocketBoard::switchOn(){
    if(m_pRelaisDriver != nullptr && m_maxTime != 0){
        m_pRelaisDriver->switchOn();
        ESP_LOGD(LOG_TAG, "switchOn, maxTime = %d sec", m_maxTime);
        xTimerChangePeriod(m_hTimeout, pdMS_TO_TICKS(m_maxTime*1000), 0);
        xTimerStart(m_hTimeout, 0);   // start maxTime timeout timer
    }
}

/**
 * @brief Switch off SocketBoard (relais)
 */
uint32_t SocketBoard::switchOff(bool isTimeout){
    if(m_pRelaisDriver != nullptr){
        m_pRelaisDriver->switchOff();
        ESP_LOGD(LOG_TAG, "switchOff, isTimeout = %d", isTimeout);
    }

    uint32_t duration_sec = 0;
    if(!isTimeout){
        duration_sec = (m_maxTime*1000 - pdTICKS_TO_MS(xTimerGetExpiryTime(m_hTimeout)))/1000;
        xTimerStop(m_hTimeout, 0);
    }
    else{
        duration_sec = m_maxTime;
        CommHandlerSendData_t sendData;
        sendData.msgID = MSG_ID_SETSOCKETEVENT;
        sendData.value1 = duration_sec;
        sendData.value2 = 0;
        sendData.flag1 = false;
        m_pCommHandler->addSendMessage(sendData);
        //commHandler.sendSocketBoardEvent();           // die callback vom timer darf nicht zu lange blockieren sonst greift ein watchdog
    }
    return duration_sec;
}

/**
 * @brief Max_time timer -> when it expires this method is called and the SocketBoard relais is switched off
 */
void SocketBoard::timeout(TimerHandle_t xTimer){
    SocketBoard* socketBoard = socketBoardMap.at(xTimer);
    socketBoard->switchOff(true);
}
