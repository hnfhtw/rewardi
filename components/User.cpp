/*
 * User.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#include "User.h"

/**
 * @brief Create a User instance.
 */
User::User(){
	m_userID	= 0;
	m_boxCode   = 0;
}

/**
 * @brief Create a User instance.
 */
User::User(uint32_t userID){
    m_userID    = userID;
    m_boxCode   = 0;
}


/**
 * @brief xx
 */
void User:: setUserID(uint32_t userID){
	m_userID = userID;
}


/**
 * @brief xx
 */
void User::setBoxCode(uint8_t boxCode){
	m_boxCode = boxCode;
}


/**
 * @brief xx
 */
uint32_t User:: getUserID(){
	return m_userID;
}



/**
 * @brief xx
 */
uint8_t User::getBoxCode(){
	return m_boxCode;
}



