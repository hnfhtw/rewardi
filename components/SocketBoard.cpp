/*
 * SocketBoard.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#include "SocketBoard.h"
#include <map>
#include <esp_log.h>
#include "CommHandler.h"
#include "messageIDs.h"

static const char* LOG_TAG = "SocketBoard";
static std::map<void*, SocketBoard*> socketBoardMap;
extern CommHandler commHandler;

/**
 * @brief Create a SocketBoard instance.
 */
SocketBoard::SocketBoard(){
	m_socketID	= 0;
	m_maxTime	= 60;
}

/**
 * @brief xx
 */
SocketBoard::~SocketBoard(){
    ::xTimerDelete(m_hTimeout, portMAX_DELAY);
    socketBoardMap.erase(m_hTimeout);
}

/**
 * @brief xx
 */
void SocketBoard::init(){
    m_hTimeout = xTimerCreate("sockMaxTime",  pdMS_TO_TICKS(m_maxTime*1000), pdFALSE, NULL, (TimerCallbackFunction_t) &SocketBoard::timeout);    // create and initialize maxTime timeout timer
    socketBoardMap.insert(std::make_pair(m_hTimeout, this));
}

/**
 * @brief xx
 */
void SocketBoard::setSocketID(uint32_t socketID){
	m_socketID = socketID;
}

/**
 * @brief xx
 */
void SocketBoard::setMaxTime(uint32_t maxTime){
	m_maxTime = maxTime;
}

/**
 * @brief xx
 */
void SocketBoard::setRelaisDriver(RelaisDriver* pRelaisDriver){
    m_pRelaisDriver = pRelaisDriver;
}

/**
 * @brief xx
 */
/*void SocketBoard::setSysControl(SysControl* pSysControl){
    m_pSysControl = pSysControl;
}*/

/**
 * @brief xx
 */
uint32_t SocketBoard::getSocketID(){
	return m_socketID;
}


/**
 * @brief xx
 */
uint32_t SocketBoard::getMaxTime(){
	return m_maxTime;
}

/**
 * @brief xx
 */
RelaisDriver* SocketBoard::getRelaisDriver(){
    return m_pRelaisDriver;
}

/**
 * @brief xx
 */
/*SysControl* SocketBoard::getSysControl(){
    return m_pSysControl;
}*/

/**
 * @brief xx
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
 * @brief xx
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
        commHandler.addSendMessage(sendData);
        //commHandler.sendSocketBoardEvent();           // die callback vom timer darf nicht zu lange blockieren sonst greift ein watchdog
    }
    return duration_sec;
}

/**
 * @brief xxx
 */
void SocketBoard::timeout(TimerHandle_t xTimer){
    SocketBoard* socketBoard = socketBoardMap.at(xTimer);
    socketBoard->switchOff(true);
}
