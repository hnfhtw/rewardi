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
    m_pBoxCodeParser    = nullptr;
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
void Box::setOwner(User* pOwner){
	m_pOwner = pOwner;
}

/**
 * @brief xx
 */
void Box::setCommHandler(CommHandler* pCommHandler){
    m_pCommHandler = pCommHandler;
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

