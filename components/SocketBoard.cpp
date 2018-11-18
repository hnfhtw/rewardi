/*
 * SocketBoard.cpp
 *
 *  Created on: 22.10.2018
 *      Author: HN
 */

#include "SocketBoard.h"

/**
 * @brief Create a SocketBoard instance.
 */
SocketBoard::SocketBoard(){
	m_socketID	= 0;
	m_maxTime	= 0.0;
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
void SocketBoard::setMaxTime(double maxTime){
	m_maxTime = maxTime;
}


/**
 * @brief xx
 */
uint32_t SocketBoard::getSocketID(){
	return m_socketID;
}


/**
 * @brief xx
 */
double SocketBoard::getMaxTime(){
	return m_maxTime;
}
