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
Box::Box(){
	m_boxID				= 0;
	m_pOwner			= NULL;
	m_backendURL  		= "";
	m_rewardiPerOpen 	= 0.0;
	m_trustNumber		= "1234ABCD";
}


/**
 * @brief xx
 */
void Box::setBoxID(uint32_t boxID){
	m_boxID = boxID;
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
void Box::setBackendURL(std::string backendURL){
	m_backendURL = backendURL;		// use string copy function??
}


/**
 * @brief xx
 */
void Box::setRewardiPerOpen(double rewardiPerOpen){
	m_rewardiPerOpen = rewardiPerOpen;
}


/**
 * @brief xx
 */
uint32_t Box::getBoxID(){
	return m_boxID;
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
std::string Box::getBackendURL(){
	return m_backendURL;
}


/**
 * @brief xx
 */
double Box::getRewardiPerOpen(){
	return m_rewardiPerOpen;
}

/**
 * @brief xx
 */
std::string Box::getTrustNumber(){
	return m_trustNumber;
}
