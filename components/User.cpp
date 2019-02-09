/********************************************************************************************
 * Project    : Rewardi
 * Created on : 22.10.2018
 * Author     : Harald Netzer
 * Version    : 001
 *
 * File       : User.cpp
 * Purpose    : Represents a Rewardi user -> used for the owner of the Rewardi Box
 ********************************************************************************************/

#include <esp_log.h>
#include "User.h"

static const char* LOG_TAG = "User";

/**
 * @brief User default constructor
 */
User::User(){
	m_userID	= 0;
	m_boxCode   = 0;
}

/**
 * @brief User constructor
 */
User::User(uint32_t userID){
    m_userID    = userID;
    m_boxCode   = 0;
}

/**
 * @brief User constructor
 */
User::User(uint32_t userID, uint8_t boxCode){
    m_userID    = userID;
    m_boxCode   = boxCode;
}

/**
 * @brief Set user ID
 */
void User:: setUserID(uint32_t userID){
	m_userID = userID;
}

/**
 * @brief Set 5-digit box code of user
 */
void User::setBoxCode(uint8_t boxCode){
    ESP_LOGD(LOG_TAG, "New boxCode set = %d", boxCode);
	m_boxCode = boxCode;
}

/**
 * @brief Get user ID
 */
uint32_t User:: getUserID(){
	return m_userID;
}

/**
 * @brief Get 5-digit box code of user
 */
uint8_t User::getBoxCode(){
	return m_boxCode;
}
