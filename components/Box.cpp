/*
 * Box.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#include "Box.h"
#include "messageIDs.h"

/**
 * @brief Create a Box instance.
 */
Box::Box(gpio_num_t lockPin, gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin, gpio_num_t buttonPin){
	m_pOwner			= nullptr;
    m_pLockDriver       = new LockDriver(lockPin);
    m_pRgbLedControl    = new RgbLedControl(ledRedPin, ledGreenPin, ledBluePin);
    m_pBoxCodeParser    = new BoxCodeParser(buttonPin, this);
    m_pCommHandler      = nullptr;
    m_userListSize      = 0;
    m_isLocked          = false;
}

/**
 * @brief xx
 */
void Box::init(){
    m_pLockDriver->init();
    m_pRgbLedControl->init();
    m_pBoxCodeParser->init();
}


/**
 * @brief xx
 */
void Box::start(){
    CommHandlerSendData_t sendData;         // obtain box owner and isLocked status at startup
    sendData.msgID = MSG_ID_GETBOXDATA;
    m_pCommHandler->addSendMessage(sendData);   // send MSG_ID_REQUESTOPEN to server

    m_pBoxCodeParser->start();  // start box code parser
}


/**
 * @brief xx
 */
void Box::setOwner(uint32_t ownerID){
    if(m_userListSize == 0){    // if user list is empty - add new user to position 0 and set user as current owner
        m_userList[0] = new User(ownerID);
        m_pOwner = m_userList[0];
        ++m_userListSize;
    }
    else{   // search if new user is already in list
        uint8_t i;
        bool userFound = false;
        for(i = 0; i<m_userListSize; ++i){
            if(m_userList[i]->getUserID() == ownerID){   // new user is in list at position i
                userFound = true;
                break;
            }
        }
        if(userFound){  // user was found in list at position i
            User* temp = m_userList[i];  // save user object - it will be relocated to position 0
            for(uint8_t j = i; j>0; --j){
                m_userList[j] = m_userList[j-1];    // move all users one position down
            }
            m_userList[0] = temp;  // set current user to position 0
            m_pOwner = m_userList[0];
        }
        else{
            if(m_userListSize == BOX_MAX_NR_OF_USERS){   // list is full - delete user on last position
                delete m_userList[BOX_MAX_NR_OF_USERS-1];
                --m_userListSize;
            }
            for(uint8_t i = m_userListSize; i>0; --i){
                m_userList[i] = m_userList[i-1];    // move all users one position down
            }
            m_userList[0] = new User(ownerID);
            m_pOwner = m_userList[0];
            ++m_userListSize;
        }
    }
}

/**
 * @brief xx
 */
void Box::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
}

/**
 * @brief xx
 */
void Box::setIsLocked(bool isLocked){
    m_isLocked = isLocked;
}

/**
 * @brief xx
 */
User* Box::getOwner(){
	return m_pOwner;
}

/**
 * @brief xx
 */
LockDriver* Box::getLockDriver(){
    return m_pLockDriver;
}

/**
 * @brief xx
 */
RgbLedControl* Box::getRgbLedControl(){
    return m_pRgbLedControl;
}

/**
 * @brief xx
 */
BoxCodeParser* Box::getBoxCodeParser(){
    return m_pBoxCodeParser;
}

/**
 * @brief xx
 */
bool Box::getIsLocked(){
    return m_isLocked;
}

/**
 * @brief xx
 */
void Box::requestBoxOpen(uint8_t boxCode){
    if(boxCode == m_pOwner->getBoxCode()){  // send open box request to server if the box code of the current user (owner) was entered
        CommHandlerSendData_t sendData;
        sendData.msgID = MSG_ID_REQUESTOPEN;
        m_pCommHandler->addSendMessage(sendData);   // send MSG_ID_REQUESTOPEN to server
    }
}

/**
 * @brief xx
 */
void Box::open(){
    m_pLockDriver->switchOn();
    // HN-CHECK switch LED to GREEN for 5s
}

