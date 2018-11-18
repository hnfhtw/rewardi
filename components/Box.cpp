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
	m_rewardiPerOpen 	= 0.0;
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
double Box::getRewardiPerOpen(){
	return m_rewardiPerOpen;
}


