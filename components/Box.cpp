/*
 * Box.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#include "Box.h"

/**
 * @brief Create a Box instance.
 */
Box::Box(gpio_num_t lockPin, gpio_num_t ledRedPin, gpio_num_t ledGreenPin, gpio_num_t ledBluePin, gpio_num_t buttonPin){
	m_pOwner			= nullptr;
    m_pLockDriver       = new LockDriver(lockPin);
    m_pRgbLedControl    = new RgbLedControl(ledRedPin, ledGreenPin, ledBluePin);
    m_pBoxCodeParser    = new BoxCodeParser(buttonPin);
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
    m_pBoxCodeParser->start();
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

/*void Box::setLockDriver(LockDriver* pLockDriver){
    m_pLockDriver = pLockDriver;
}

void Box::setRgbLedControl(RgbLedControl* pRgbLedControl){
    m_pRgbLedControl = pRgbLedControl;
}

void Box::setBoxCodeParser(BoxCodeParser* pBoxCodeParser){
    m_pBoxCodeParser = pBoxCodeParser;
}*/

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


